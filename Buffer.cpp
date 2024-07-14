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
        deque<pair<char,bool>> requerimientos;
        
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

class Buffer {
    private:
        Disco* my_disk;
        int Buf_size;
        vector<Frame> BufferPool;
        unordered_map<int, int> PageTable; // ID PAGINA - ID FRAME
        Clock* my_clock; 
        LRU* my_LRU;
        int choice_replacer;
        string dirPaginas = "dirPaginas.txt";

        int hit_count;
        int miss_count;
        int request_count;

        bool Delete_Page(){
            int frameVictima, pagevictima = my_LRU->victima();
            if(choice_replacer == 0 && pagevictima != -1) frameVictima = PageTable[pagevictima];
            else if(choice_replacer == 1) frameVictima = my_clock->victima();
            
            if(frameVictima == -1 || pagevictima == -1) {  cout<<" No podemos eliminar ninguna pagina! Eliminar manualmente! \n ";return false;}
            
            while(BufferPool[frameVictima].page->getPinCount() > 0){
                int choice;
                cout<<" Desea liberar el frame "<<BufferPool[frameVictima].idFrame<<" ? (Y = 1 | N = 0)  ";cin>> choice;
                if(choice == 1) unpinPage(BufferPool[frameVictima].page->getIdPage());
                else {cout<< " No podemos eliminar ninguna pagina! Eliminar manualmente! \n"; return false;}//return victima();
            }
            my_LRU->deletePage(pagevictima);

            remove(BufferPool[frameVictima].page->getDirPage().c_str()); // ARCHIVOS !!
            PageTable.erase(BufferPool[frameVictima].page->getIdPage()); 
            BufferPool[frameVictima].releaseFrame();  
            return true;
        }

    public:
        // Constructor de Buffer
        Buffer(int _Buf_size, int _page_size, Disco* & _Disco, int _choice) : Buf_size(_Buf_size), my_disk(_Disco), hit_count(0), miss_count(0), request_count(0) , choice_replacer(_choice) {
            // Crear n frames de acuerdo a cuántos bloques (páginas puedan entrar)
            int nFrames = _Buf_size / _page_size;
            for (int i = 0; i < nFrames; ++i) { BufferPool.push_back(Frame(i));}
            if(_choice == 1) my_clock = new Clock(nFrames);
            else if(_choice == 0)my_LRU = new LRU();
            // CREAR HEAP FILE
            //crearDirPaginas();
            _mkdir("BUFFERPOOL");
        }

        /*void crearDirPaginas(){
            std::ifstream origen("dirBloques.txt");
            // Abrir el archivo de destino en modo escritura
            std::ofstream destino(dirPaginas);

            // Leer del archivo de origen y escribir en el archivo de destino
            std::string linea;
            while (std::getline(origen, linea)) {
                destino << linea << std::endl;
            }

            // Cerrar ambos archivos
            origen.close();
            destino.close();
        }*/
    
        void flushPage(int _idPage){
            ofstream block(getdirBloque(_idPage)); 
            ifstream page(getdirPage(_idPage));
            string line = "";
            while (getline(page, line)){ block<<line<<endl;}
            page.close();
            block.close();
            my_disk->guardarBloqueSector(_idPage); //!!!!!!!!!!!!!
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
                    cout << " Desea guardar los cambios? (S/N): "; cin >> guardar;
                    if (guardar == 'S' || guardar == 's') { this->flushPage(idPage); }   
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

        // Función para agregar una nueva página al búfer
        Page* newPage(int idPage, char func, bool _pinned) {
            request_count++;
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

        void printBuffer() const {
            cout << "_____________ BUFFER POOL ___________________________\n\n";
            for (const auto& frame : BufferPool) {
                if (frame.page) { cout<<"Frame ID: "<<frame.idFrame<<", Page ID: "<<frame.page->getIdPage()<< ", Dity bit: "<< frame.page->getDirtyBit() << ", Pin Count: " << frame.page->getPinCount() << ", Pinned: " <<frame.page->getPinned()<< " \n";
                }else{ cout<<"Frame ID: "<<frame.idFrame<<" > FRAME VACIO! \n";}
            }
            if(choice_replacer == 1)my_clock->printClock();
            else if(choice_replacer == 0) my_LRU->printLRU();
        }
        void printStats() const {
            cout << "Total Requests: " << request_count << "\n";
            cout << "Hit Count: " << hit_count << "\n";
            cout << "Miss Count: " << miss_count << "\n";
        }
        void printPageTable() {
            for (const auto& pair : PageTable) {
                std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
            }
        }
        
        void modificarPage(int _idPage){
            int op;
            cout << "__________________________________________________\n";
            cout << "\n----- MODIFICANDO PAGINA "<<_idPage<<" -----\n";
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
                        bool insertado = false;
                        for (const auto& frame : BufferPool) {
                            if (frame.page) {
                                cout << " UTILIZANDO NUEVA PAGINA> " << frame.page->getIdPage() << "\n";
                                if (adicionarRegistroPage(frame.page->getIdPage(), registro, relacion, R)) {
                                    cout<<" > REGISTRO INSERTADO \n";
                                    insertado = true;
                                    break;
                                }
                            }
                        }
                        if (!insertado) {
                            std::cout << " No hay espacio suficiente en PAGES! Liberar o agregar nueva pagina! \n";
                            return;
                        }
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
};

void displayMenu() {
    cout << "__________________________________________________\n";
    cout << "\n----- MENU -----\n";
    cout << "1. New page\n";
    cout << "2. Pin page\n";
    cout << "3. Unpin page\n";
    cout << "4. Get page\n";
    cout << "5. Modificar Page\n";
    cout << "6. Mostrar estado del buffer\n";
    cout << "7. Imprimir Pagina / Bloque\n";
    cout << "8. Salir\n";
    cout << "__________________________________________________\n\n";
    cout << "Elija una opcion: ";
}

void MenuBuffer(Disco* &my_disk) {
    int choice;
    cout << " > Metodo de reemplazo " << endl;
    cout << " LRU (0) o CLOCK (1) > ";cin>>choice;

    Buffer buffer(my_disk->getCapacidadBloque() *3, my_disk->getCapacidadBloque(), my_disk, choice);
    
    do {
        buffer.printBuffer();
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
                buffer.newPage(pageId, funcion, pinned);
                break;
            }
            case 2: {
                int pageId; char funcion; bool pinned;
                cout << "\n-----  Pin page  ---------\n";
                cout << "Ingrese el ID de la pagina a fijar: ";
                cin >> pageId;
                cout << "R/W?     ";cin>>funcion;
                cout << "PINNED?  ";cin>>pinned;
                buffer.pinPage(pageId, funcion, pinned);
                break;
            }
            case 3: {
                cout << "\n-----  Unpin page  ---------\n";
                int pageId;
                cout << "Ingrese el ID de la pagina a liberar: ";
                cin >> pageId;
                buffer.unpinPage(pageId);
                break;
            }
            case 4:{
                cout << "\n-----  Get page  ---------\n";
                int pageId; char funcion;  bool pinned;
                cout << "Ingrese el ID de la pagina a conseguir: ";
                cin >> pageId;
                cout << "R/W?     ";cin>>funcion;
                cout << "PINNED?  ";cin>>pinned;
                buffer.getPage(pageId, funcion, pinned);
                break;
            }
            case 5:{
                cout << "\n-----  Modificar page  ---------\n";
                int pageId;
                cout << "Ingrese el ID de la pagina a modificar: ";
                cin >> pageId;
                buffer.pinPage(pageId, 'W', 0);
                buffer.modificarPage(pageId);
                break;
            }
            case 6: {
                buffer.printBuffer();
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