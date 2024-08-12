#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <direct.h>
#include <deque>

#include "Disco.cpp"
#include "REPLACERS/Clock.cpp"
#include "REPLACERS/LRU.cpp"

using namespace std;


class Page {
    private:
        int idPage;
        string dir_page;
        bool dirty_bit;
        int pin_count;
        bool pinned;
    
    public:
        deque<pair<char,bool>> requerimientos; // COLA DE REQUERIMIENTOS
        std::vector<pair<string, vector<Cambios>>> cambios; // cambio (Adicionar o eliminar, clave de Busqueda, ruta)
        string relacion; 

        // CONSTRUCTOR 
        Page(int _id, bool _pinned) :idPage(_id), dirty_bit(false), pin_count(1), pinned(_pinned){
            dir_page = "BUFFERPOOL/Page" + to_string(_id) + ".txt";
            ifstream block("DISCO/BLOQUES/Bloque" + to_string(_id) + ".txt"); 
            ofstream page(dir_page);
            string line = "";
            while (getline(block, line)){ page<<line<<endl;}
            page.close();
            block.close();
        }

        int getIdPage() { return idPage; }
        string getDirPage() { return dir_page; }
        bool getDirtyBit() { return dirty_bit; }
        int getPinCount() { return pin_count; }
        bool getPinned() {return pinned; }

        void Pinned() { pinned =true; }
        void unPinned() { pinned = false; }
        void turnDirtyBit() { dirty_bit = true;}
        void incrementPinCount() { pin_count++;} 
        void decrementPinCount() { if (pin_count > 0) pin_count--; }  
        void resetPinCount() { pin_count = 0; }
        void setDirtyBit(bool _dirtyBit){ dirty_bit = _dirtyBit; }

        void actualizarDirtyBit() {
            for(int i=requerimientos.size()-1; i>0; i--){
                if(requerimientos[i].second == 1){
                    requerimientos[i-1].second = 1;
                }
            }
            dirty_bit = requerimientos[0].second;
        }
};

// ESTRUCTURA DE FRAME
struct Frame {
    unique_ptr<Page> page;
    int idFrame;

    Frame(int _id) : idFrame(_id), page(nullptr){}
    void setPage(Page* _page){page.reset(_page);}
    void releaseFrame(){ 
        if(page){ 
            page.reset(nullptr);
        }
    } 
};
// CLASE BUFFER
class Buffer {
    private:
        Disco* my_disk;
        int Buf_size;
        vector<Frame> BufferPool;
        unordered_map<int, int> PageTable; // ID PAGINA - ID FRAME
        Clock* my_clock; 
        LRU* my_LRU;
        int choice_replacer;
        vector<BPlusTree<int>*> indices;
        vector<HeapFile*> heapsfiles;

        int hit_count;
        int miss_count;
        int request_count;

        bool Delete_Page(){
            //cout<<" ELIMINANDO ! ";
            int frameVictima = -1, pagevictima = -1;
            
            //if(choice_replacer == 0 && pagevictima != -1) frameVictima = PageTable[pagevictima];
            if(choice_replacer == 0){
                int pagevictima = my_LRU->victima();
                if( pagevictima != -1) frameVictima = PageTable[pagevictima];
            }
            else if(choice_replacer == 1) frameVictima = my_clock->victima();

            //printBuffer();
            
            if(choice_replacer == 0 && pagevictima == -1) { cout<<" No podemos eliminar ninguna pagina! Eliminar manualmente! \n ";return false; }
            if(choice_replacer == 1 && frameVictima == -1) {  cout<<" No podemos eliminar ninguna pagina! Eliminar manualmente! \n ";return false;}
            
            while(BufferPool[frameVictima].page->getPinCount() > 0){
                int choice = 1;
                //cout<<" Desea liberar el frame "<<BufferPool[frameVictima].idFrame<<" ? (Y = 1 | N = 0)  ";cin>> choice;
                char guardar = 'S';
                //cout << " Desea guardar los cambios? (S/N): "; cin >> guardar;
                if(choice == 1) superUnpinPage(BufferPool[frameVictima].page->getIdPage(), guardar);
                else {cout<< " No podemos eliminar ninguna pagina! Eliminar manualmente! \n"; return false;}//return victima();
            }
            if(choice_replacer == 0) my_LRU->deletePage(pagevictima);

            remove(BufferPool[frameVictima].page->getDirPage().c_str()); // ARCHIVOS !!
            PageTable.erase(BufferPool[frameVictima].page->getIdPage()); 
            BufferPool[frameVictima].releaseFrame();  
            return true;
        }

    public:
        // Constructor de Buffer (tamanio del buffer, tamaio de paginas, Disco, REPLACER CLOCK-LRU)
        Buffer(int _Buf_size, int _page_size, Disco* & _Disco, int _choice) : Buf_size(_Buf_size), my_disk(_Disco), hit_count(0), miss_count(0), request_count(0) , choice_replacer(_choice) {
            // Crear n frames de acuerdo a cuántos bloques (páginas puedan entrar)
            int nFrames = _Buf_size / _page_size;
            for (int i = 0; i < nFrames; ++i) { BufferPool.push_back(Frame(i));}
            if(_choice == 1) my_clock = new Clock(nFrames);
            else if(_choice == 0)my_LRU = new LRU();
            indices = _Disco->indices;
            crearHeapsFiles();
            _mkdir("BUFFERPOOL");
        }

        // CREA HEAP FILEES PARA TODAS LAS RELACIONES DEL DISCO
        void crearHeapsFiles(){
            cout<<" Creando HEAPSFILES \n";
            string aux;
            vector<string> relaciones;
            std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
            while(getline(diccionario, aux)){
                stringstream relacion(aux);
                getline(relacion, aux, '#');
                relaciones.push_back(aux);
            }
            diccionario.close();
            
            for(int i=0; i<relaciones.size(); i++){
                //cout<<" Creando para la relacion "<<relaciones[i]<<endl;
                heapsfiles.push_back(new HeapFile(relaciones[i]));
                //heapsfiles[i]->print();
            }
        }
        // RETORNAR PAGINA
        void flushPage(int _idPage){
            ofstream block(getdirBloque(_idPage)); 
            ifstream page(getdirPage(_idPage));
            string line = "";
            while (getline(page, line)){ block<<line<<endl;}
            page.close();
            block.close();
            my_disk->guardarBloqueSector(_idPage); 
        }
        // Indicar que la pagina esta en uso
        void pinPage(int idPage, char func, bool pinned) {
            //request_count++;
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) { 
                // PIN COUNT
                BufferPool[it->second].page->incrementPinCount(); // PINNING
                // DIRTY BIT
                BufferPool[it->second].page->requerimientos.push_back(make_pair(func, changeFuncInt(func)));
                BufferPool[it->second].page->actualizarDirtyBit();
                // PINNED
                if(pinned) BufferPool[it->second].page->Pinned();
                // REPLACER
                if(choice_replacer == 1) my_clock->pin(BufferPool[it->second].idFrame, func, pinned);
                else if(choice_replacer == 0) my_LRU->pin(idPage, func, pinned);

            } else { 
                newPage(idPage, func, pinned);
            }
        }
        // Indicar que la pagina esta un uso menos
        void unpinPage(int idPage) {
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) {
                // PIN COUNT
                BufferPool[it->second].page->decrementPinCount();
                // LIBERAR PROCESOS
                if(BufferPool[it->second].page->requerimientos.size() == 0){
                    BufferPool[it->second].page->setDirtyBit(0);
                    cout<<" No hay requerimiento que eliminar! ";
                }else{
                    if(BufferPool[it->second].page->requerimientos[0].first == 'W' ){
                        cout << "\n Liberando proceso de Escritura >> \n";
                        char guardar;
                        //cout << " Desea guardar los cambios? (S/N): "; cin >> guardar;
                        guardar = 'S';
                        if (guardar == 'S' || guardar == 's') { 
                            my_disk->addChanges(BufferPool[it->second].page->cambios, BufferPool[it->second].page->relacion);
                            this->flushPage(idPage); 
                        }   
                    }
                    else cout << "\n Liberando proceso de Lectura >> \n";
                    BufferPool[it->second].page->requerimientos.pop_front();
                    if(BufferPool[it->second].page->requerimientos.size() == 0) BufferPool[it->second].page->setDirtyBit(0);
                    else BufferPool[it->second].page->actualizarDirtyBit();
                }

                // REPLACER
                if(choice_replacer == 1)my_clock->unpin(BufferPool[it->second].idFrame);
                else if(choice_replacer == 0) my_LRU->unpin(idPage);
            }
        }
        
        void superUnpinPage(int idPage, char guardar){
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) {
                // PIN COUNT
                BufferPool[it->second].page->resetPinCount();
                // LIBERAR PROCESOS
                
                //BufferPool[it->second].page->requerimientos.push_back(make_pair('W',changeFuncInt('W')));
                if(BufferPool[it->second].page->requerimientos.size() == 0){
                    BufferPool[it->second].page->setDirtyBit(0);
                    cout<<" No hay requerimiento que eliminar! ";
                }else{
                    if(BufferPool[it->second].page->requerimientos[0].first == 'W' ){
                        BufferPool[it->second].page->requerimientos.clear();
                        cout << "\n Liberando proceso de Escritura >> \n";
                        //cout << " Desea guardar los cambios? (S/N): ";// cin >> guardar;
                        //guardar = 'S';
                        if (guardar == 'S' || guardar == 's') { 
                            my_disk->addChanges(BufferPool[it->second].page->cambios, BufferPool[it->second].page->relacion);
                            this->flushPage(idPage); 
                        }   
                    }
                    //else cout << "\n Liberando proceso de Lectura >> \n";
                    BufferPool[it->second].page->requerimientos.pop_front();
                    if(BufferPool[it->second].page->requerimientos.size() == 0) BufferPool[it->second].page->setDirtyBit(0);
                    else BufferPool[it->second].page->actualizarDirtyBit();
                }

                // REPLACER
                if(choice_replacer == 1)my_clock->superUnpin(BufferPool[it->second].idFrame);
                else if(choice_replacer == 0) my_LRU->unpin(idPage);
            }   
        }
        // Función para agregar una nueva página al búfer
        Page* newPage(int idPage, char func, bool _pinned) {
            request_count++;
            //this->printBuffer();
            // comprobar que algun frame  este vacio
            for(auto &Frame : BufferPool){
                if(Frame.page.get()){  // si la pag del frame existe
                    if(Frame.page->getIdPage() == idPage){ // SI ENCUENTRA LA PAGINA
                        hit_count++;
                        pinPage(idPage, func, _pinned);
                        return Frame.page.get();
                    }
                }
                if(!Frame.page.get()){ // SI EL FRAME ESTA VACIO COLOCA AHI LA PAG
                    miss_count++;
                    Page* NewPage = new Page(idPage, _pinned);
                    editPageHeapFile(idPage);
                    Frame.setPage(NewPage);
                    Frame.page->requerimientos.push_back(make_pair(func, changeFuncInt(func)));
                    Frame.page->actualizarDirtyBit();
                    PageTable[idPage] = Frame.idFrame;
                    if(choice_replacer == 1)my_clock->newPage(Frame.idFrame, func, _pinned);
                    else if(choice_replacer == 0) my_LRU->newPage(idPage, func, _pinned);
                    return NewPage;
                }
            }
            if(Delete_Page()) return newPage(idPage, func, _pinned);
            return nullptr;
        }
        // OBTENER PAGINA SOLICITADA POR ID
        Page* getPage(int _idPage, char func, bool pinned){        
            auto it = PageTable.find(_idPage);
            if (it != PageTable.end()) {
                BufferPool[it->second].page->incrementPinCount(); // PINNING
                BufferPool[it->second].page->requerimientos.push_back(make_pair(func, changeFuncInt(func)));
                BufferPool[it->second].page->actualizarDirtyBit();
                if(pinned) BufferPool[it->second].page->Pinned();
                if(choice_replacer == 1)my_clock->pin(BufferPool[it->second].idFrame, func, pinned);
                else if(choice_replacer == 0) my_LRU->pin(_idPage, func, pinned);
                return BufferPool[it->second].page.get();
            }
            return newPage(_idPage, func, pinned);
        }
        // IMPRIMIR CARACTERISTICAS DEL BUFFER
        void printBuffer() const {
            cout << "\n_____________ BUFFER POOL ___________________________\n\n";
            if(choice_replacer == 1) cout<<" > CLOCK \n";
            else if(choice_replacer == 0) cout<<" > LRU \n";
            for (const auto& frame : BufferPool) {
                if (frame.page) { cout<<"Frame ID: "<<frame.idFrame<<", Page ID: "<<frame.page->getIdPage()<< ", Dity bit: "<< frame.page->getDirtyBit() << ", Pin Count: " << frame.page->getPinCount() << ", Pinned: " <<frame.page->getPinned()<< " \n";
                }else{ cout<<"Frame ID: "<<frame.idFrame<<" > FRAME VACIO! \n";}
            }
            //if(choice_replacer == 1)my_clock->printClock();
            //else if(choice_replacer == 0) my_LRU->printLRU();
        }
        // IMPRIMIR HITS
        void printStats() const {
            cout << "Total Requests: " << request_count << "\n";
            cout << "Hit Count: " << hit_count << "\n";
            cout << "Miss Count: " << miss_count << "\n";
        }
        // IMRPIMIR PAGE TABLE
        void printPageTable() {
            for (const auto& pair : PageTable) {
                std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
            }
        }
        
        void modificarPage(){
            int op;
            cout << "__________________________________________________\n";
            //cout << "\n----- MODIFICANDO PAGINA "<<_idPage<<" -----\n";
            int _idPage;
            cout << "---- MODIFICANDO PAGINA  -> Ingrese el ID de la pagina a modificar: ";
            cin >> _idPage;
            pinPage(_idPage, 'W', 0);

            cout << "1. Adicionar registros \n";
            cout << "2. Eliminar registros\n";
            cout << "3. Modificar registros\n";
            cout << "__________________________________________________\n\n";
            cout << "Elija una opcion: ";
            cin >> op;
            if (op == 1) {
                int R;
                string n; 
                string archivo, relacion;
                cout << "\n----- ADICIONANDO REGISTROS -----\n";
                std::cout<<" Adicionando (n) registros de LV(0) LF(1) del (archivo) a la (Relacion) >\n";
                std::cout<<" n> (n/*)   \t";cin>>n;  ///1
                std::cout<<" Archivo >  \t "; cin>>archivo;
                std::cout<<" Relacion > \t";cin>>relacion;
                std::cout<<" Registro Long FIJA (1) O Varible(0) > ";cin>>R;
                archivo =  "DATABASE/" + archivo;
                // saltandonos encabezado
                std::ifstream data(archivo);
                std::string registro;
                std::getline(data, registro); 
                int loopLimit = (n == "*") ? INT_MAX : stoi(n);

                for (int i = 0; i < loopLimit; ++i) {
                    if (!std::getline(data, registro)) {
                        break; // Salir del bucle si no hay más registros
                    }

                    if (!adicionarRegistroPage(_idPage, registro, relacion, R)) {
                        // adquiri nueva pagina indices
                        std::cout << " No hay espacio suficiente en PAGES. Liberar o agregar nueva pagina! \n";
                        return;
                    }
                }
                data.close();
            } else if (op == 2) {
                string relacion, condicion;
                std::cout<<" Eliminando registro de la (relacion) donde (condicion) >\n";
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Condicion > ";getline(std::cin >> std::ws, condicion);
                registroPage(_idPage, relacion, condicion, "", 0);
            } else if (op == 3) {
                string relacion, condicion, atributo;
                std::cout<<" Actualizando en la (relacion) el (atributo) del registro donde (condicion) >\n";
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Atributo > ";getline(std::cin >> std::ws, atributo);
                std::cout<<" Condicion > ";getline(std::cin >> std::ws, condicion);
                registroPage(_idPage, relacion, condicion, atributo, 1);
            } else { cout << " Ingrese una opción valida! \n "; return; }
        }
        // MOSTRAR MENU
        void displayMenu() {
            cout << "__________________________________________________\n";
            cout << "\n----- MENU -----\n";
            cout << "1. New page\n";
            cout << "2. Pin page\n";
            cout << "3. Unpin page\n";
            cout << "4. Get page\n";
            cout << "5. BUFFER MANAGER \n";
            cout << "6. Mostrar estado del buffer\n";
            cout << "7. Imprimir Pagina / Bloque\n";
            cout << "8. Salir\n";
            cout << "__________________________________________________\n\n";
            cout << "Elija una opcion: ";
        }
        // MENU BUFFER
        void MenuBuffer() {
            int choice;
            do {
                printBuffer();
                displayMenu();
                cin >> choice;

                switch (choice) {
                    case 1: {
                        int pageId; char funcion; bool pinned;
                        cout << "\n-----  New page  ---------\n";
                        cout << "Ingrese el ID de la nueva pagina: ";
                        cin >> pageId;
                        cout << "R/W?     ";cin>>funcion;
                        cout << "PINNED?  ";cin>>pinned;
                        newPage(pageId, funcion, pinned);
                        break;
                    }
                    case 2: {
                        int pageId; char funcion; bool pinned;
                        cout << "\n-----  Pin page  ---------\n";
                        cout << "Ingrese el ID de la pagina a fijar: ";
                        cin >> pageId;
                        cout << "R/W?     ";cin>>funcion;
                        cout << "PINNED?  ";cin>>pinned;
                        pinPage(pageId, funcion, pinned);
                        break;
                    }
                    case 3: {
                        cout << "\n-----  Unpin page  ---------\n";
                        int pageId;
                        cout << "Ingrese el ID de la pagina a liberar: ";
                        cin >> pageId;
                        unpinPage(pageId);
                        break;
                    }
                    case 4:{
                        cout << "\n-----  Get page  ---------\n";
                        int pageId; char funcion;  bool pinned;
                        cout << "Ingrese el ID de la pagina a conseguir: ";
                        cin >> pageId;
                        cout << "R/W?     ";cin>>funcion;
                        cout << "PINNED?  ";cin>>pinned;
                        getPage(pageId, funcion, pinned);
                        break;
                    }
                    case 5:{
                        cout << "\n-----  Modificar page  ---------\n";
                        modificarPage();
                        break;
                    }
                    case 6: {
                        printBuffer();
                        break;
                    }
                    case 7: {
                        int pageId;
                        cout << "Ingrese el ID de la pagina/bloque a imprimir: ";
                        cin >> pageId;
                        imprimirArchivo(getdirPage(pageId));
                        imprimirArchivo(getdirBloque(pageId));
                        break;
                    }
                    case 8: {
                        cout << "Saliendo...\n";
                        break;
                    }
                    default: {
                        cout << "Opción no válida. Intente de nuevo.\n";
                    }
                }
            } while (choice != 8);

        }

        HeapFile* getHeapFile(string _relacion){
            for(int i=0; i<heapsfiles.size(); i++){
                if(heapsfiles[i]->nombreRelacion == _relacion) return heapsfiles[i];
            }
            return nullptr;
        }
        void editPageHeapFile(int idPage){
            for(int i=0; i<heapsfiles.size(); i++){
                for(int j=0; j<heapsfiles[i]->Bloque_espacioLibre.size(); j++){
                    if(heapsfiles[i]->Bloque_espacioLibre[j].first == idPage){
                        heapsfiles[i]->Bloque_espacioLibre[j].second = getCapacLibreBloque(idPage);
                        return;
                    }
                }
            }
        }
        void updateCapacBloqueHF(string _relacion, int _idPage, int _espacioLibre){
            HeapFile* heapfile = getHeapFile(_relacion);
            heapfile->editCapacidad(make_pair(_idPage, _espacioLibre));
        }

        BPlusTree<int>* getIndice(string _relacion, string _claveBusqueda){
            for(int i=0; i<indices.size(); i++){
                if(indices[i]->relacion == _relacion && indices[i]->claveBusqueda == _claveBusqueda){
                    return indices[i];
                }
            }
            return nullptr;
        }

        BPlusTree<int>*  createNewIndice(string _relacion, string _claveBusqueda){
            BPlusTree<int>* newIndice = new BPlusTree<int>(6, _relacion, _claveBusqueda);
            indices.push_back(newIndice);
            return newIndice;
        }

        void addIndice(string _relacion, string _claveBusqueda){
            BPlusTree<int>* indice = getIndice(_relacion, _claveBusqueda);
            if(!indice) createNewIndice(_relacion, _claveBusqueda);
        }

        int getBloque(string _relacion, string _claveBusqueda, int key){
            BPlusTree<int>* indice = getIndice(_relacion, _claveBusqueda);
            pair<int, int> ruta;
            HeapFile* heapfile = getHeapFile(_relacion);

            if(!indice){
                // cout<<" INDICE NO EXISTE \n"<<endl;
                indice = createNewIndice(_relacion, _claveBusqueda);
                return heapfile->getNewFree();
            }else{
                // cout<<" INDICE EXISTE! \n "<<endl;
                ruta = indice->getRuta(key);
                if(ruta.first == 0 && ruta.second == 0){ // quiere decir que no existe un registro con
                    // cout<<"\n RUTA NO ENCONTRADA GET NEW FREE !\n";
                    return heapfile->getNewFree();
                }else{
                    return ruta.first;
                }
            }
        }
        int getNewBloque(string _relacion){
            HeapFile* heapfile = getHeapFile(_relacion);
            return heapfile->getNewFree();
        }

        void addRuta(string _relacion, string _claveBusqueda, int key, pair<int, int> ruta){
            BPlusTree<int>* indice = getIndice(_relacion, _claveBusqueda);

            if(!indice){
                indice = createNewIndice(_relacion, _claveBusqueda);
            }
            indice->set(key, ruta); 
            cout<<"\n INDICE EN TEMPORAL DE BUFFER ACTUALIZADO !\n";
            indice->print();
        }
        void deleteRuta(string _relacion, string _claveBusqueda, int key){
            BPlusTree<int>* indice = getIndice(_relacion, _claveBusqueda);

            if(indice){
                indice->remove(key);
                // cout<<"\n INDICE TEMPORAL DE BUFFER ACTUALIZADO !\n";
                // indice->print();
            }
        }
        void printIndice(string _relacion){
            BPlusTree<int>* indice = getIndice(_relacion, "");
            if(indice){
                cout<<"\n INDICE TEMPORAL DE BUFFER ACTUALIZADO !\n";
                indice->print();
            }
        }

        // AÑADIR CAMBIOS (INSERCIONES - ELIMINACIONES) A PAGINA 
        void addChanges(int _idPage, int key ,bool _change, string _relacion, string _claveBusqueda){ // change 1 -adicionar /  0 - eliminar
            
            cout<<" CHANGES > "<<_change<<endl;
            auto it = PageTable.find(_idPage);
            if (it != PageTable.end()) {
                BufferPool[it->second].page->relacion = _relacion;
                for(int j=0; j<BufferPool[it->second].page->cambios.size(); j++){
                    cout<<" CLAV DE BSUSQEUDA "<<BufferPool[it->second].page->cambios[j].first<<endl;
                    if(BufferPool[it->second].page->cambios[j].first == _claveBusqueda){
                        BufferPool[it->second].page->cambios[j].second.push_back(Cambios(_change, key, _idPage));
                        return;
                    }
                }
                vector<Cambios> vector;
                vector.push_back(Cambios(_change, key, _idPage));
                cout<<" creando "<<endl;
                BufferPool[it->second].page->cambios.push_back(make_pair(_claveBusqueda, vector));
            }else{ cout<<" NO ENCONTRE "<<endl; }
        }
};