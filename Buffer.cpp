#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <direct.h>

#include "Disco.cpp"
#include "Clock.cpp"
#include "LRU.cpp"

using namespace std;

class Page {
    private:
        int idPage;
        string dir_page;
        bool dirty_bit;
        int pin_count;
        bool pinned;

    public:
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
        void Pinned() { pinned =true; }
        void unPinned() { pinned = false; }
        void turnDirtyBit() { dirty_bit = true;}
        void incrementPinCount() { pin_count++;} 
        void decrementPinCount() { if (pin_count > 0) pin_count--; }      
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

        int hit_count;
        int miss_count;
        int request_count;

        void Delete_Page(int idFrame){
            if(BufferPool[idFrame].page->getDirtyBit() == 1){ // si la pagina fue modificada
                cout<<" Devolviendo pagina "<<BufferPool[idFrame].page->getIdPage()<<" a Disco . . . \n";
                //flushPage(BufferPool[idFrame].page->getIdPage()); 
            }
            remove(BufferPool[idFrame].page->getDirPage().c_str()); // ARCHIVOS !!
            PageTable.erase(BufferPool[idFrame].page->getIdPage()); 
            BufferPool[idFrame].releaseFrame();  
        }

    public:
        // Constructor de Buffer
        Buffer(int _Buf_size, int _page_size, Disco* & _Disco, int _choice) : Buf_size(_Buf_size), my_disk(_Disco), hit_count(0), miss_count(0), request_count(0) , choice_replacer(_choice) {
            // Crear n frames de acuerdo a cuántos bloques (páginas puedan entrar)
            int nFrames = _Buf_size / _page_size;
            for (int i = 0; i < nFrames; ++i) { BufferPool.push_back(Frame(i));}
            if(_choice == 1) my_clock = new Clock(nFrames);
            else if(_choice == 0)my_LRU = new LRU();
            _mkdir("BUFFERPOOL");
        }

        // Indicar que la pagina esta en uso
        void pinPage(int idPage, char func, bool pinned) {
            //request_count++;
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) { // SI ESTA
                //hit_count++;
                BufferPool[it->second].page->incrementPinCount(); // PINNING
                if(func == 'W') BufferPool[it->second].page->turnDirtyBit();
                if(pinned) BufferPool[it->second].page->Pinned();
                if(choice_replacer == 1)my_clock->pin(BufferPool[it->second].idFrame, func, pinned);
                else if(choice_replacer == 0) my_LRU->pin(idPage, func, pinned);

            } else { // NO ESTA
                //miss_count++;
                newPage(idPage, func, pinned);
            }
        }

        // Indicar que la pagina esta un uso menos
        void unpinPage(int idPage) {
            //std::cout<<"EN UNPIN PAGE \n";
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) {
                BufferPool[it->second].page->decrementPinCount();
                if(choice_replacer == 1)my_clock->unpin(BufferPool[it->second].idFrame);
                else if(choice_replacer == 0) my_LRU->unpin(idPage);
            }
        }

        // Función para agregar una nueva página al búfer
        Page* newPage(int idPage, char func, bool _pinned) {
            request_count++;

            int idF;
            // comprobar que algun frame  este vacio
            for(auto &Frame : BufferPool){
                if(Frame.page.get()){  // si la pag del frame existe
                    if(Frame.page->getIdPage() == idPage){ // SI ENCUENTRA LA PAGINA
                        hit_count++;

                        Frame.page->incrementPinCount();
                        if(func == 'W') Frame.page->turnDirtyBit();
                        if(_pinned) Frame.page->Pinned();
                        if(choice_replacer == 1)my_clock->pin(Frame.idFrame, func, _pinned);
                        else if(choice_replacer == 0) my_LRU->pin(idPage, func, _pinned);
                        return Frame.page.get();
                    }
                }
                if(!Frame.page.get()){ // SI EL FRAME ESTA VACIO COLOCA AHI LA PAG
                    miss_count++;

                    Page* NewPage = new Page(idPage, _pinned);
                    if(func == 'W') NewPage->turnDirtyBit();
                    Frame.setPage(NewPage);
                    PageTable[idPage] = Frame.idFrame;
                    if(choice_replacer == 1)my_clock->newPage(Frame.idFrame, func, _pinned);
                    else if(choice_replacer == 0) my_LRU->newPage(idPage, func, _pinned);
                    return NewPage;
                }
            }
            // FUNCIONES ELIMINAR
            int frameVictima ;
            if(choice_replacer == 0){
                frameVictima = my_LRU->victima(0);
                my_LRU->deletePage(frameVictima);
                auto it = PageTable.find(frameVictima);
                if (it != PageTable.end()) {
                    frameVictima = BufferPool[it->second].idFrame;
                }
            }
            else if(choice_replacer == 1){ frameVictima = my_clock->victima();}
            if(frameVictima == -1) {  cout<<" No podemos eliminar ninguna pagina!\n ";return nullptr;}
            Delete_Page(frameVictima);
            std::cout<<" Frame liberado>  "<<frameVictima<<endl;
            return newPage(idPage, func, _pinned);
        }

        // OBTENER PAGINA SOLICITADA POR ID
        Page* getPage(int _idPage, char func, bool pinned){        
            auto it = PageTable.find(_idPage);
            if (it != PageTable.end()) {
                BufferPool[it->second].page->incrementPinCount(); // PINNING
                if(func == 'W') BufferPool[it->second].page->turnDirtyBit();
                if(pinned) BufferPool[it->second].page->Pinned();
                if(choice_replacer == 1)my_clock->pin(BufferPool[it->second].idFrame, func, pinned);
                else if(choice_replacer == 0) my_LRU->pin(_idPage, func, pinned);
                return BufferPool[it->second].page.get();
            }
            return newPage(_idPage, func, pinned);
        }

        void flushPage(int _idPage){
            string dirPage = "BUFFERPOOL/Page" + to_string(_idPage) + ".txt";
            ofstream block("DISCO/BLOQUES/Bloque" + to_string(_idPage) + ".txt"); 
            ifstream page(dirPage);
            string line = "";
            while (getline(page, line)){ block<<line<<endl;}
            page.close();
            block.close();
            my_disk->guardarBloqueSector(_idPage);
        }

        void FlushAllPages(){}

        void printBuffer() const {
            for (const auto& frame : BufferPool) {
                if (frame.page) { cout<<"Frame ID: "<<frame.idFrame<<", Page ID: "<<frame.page->getIdPage()<< ", Dity bit: "<< frame.page->getDirtyBit() << ", Pin Count: " << frame.page->getPinCount() << " \n";
                }else{ cout<<"Frame ID: "<<frame.idFrame<<" > FRAME VACIO! \n";}
            }
            if(choice_replacer == 1)my_clock->printClock();
            else if(choice_replacer == 0) my_LRU->printLRU();
        }
        void printStats() const {
            cout << "Total Requests: " << request_count << "\n";
            cout << "Hit Count: " << hit_count << "\n";
            cout << "Miss Count: " << miss_count << "\n";
            //cout << "Hit Ratio: " << (request_count ? static_cast<double>(hit_count) / request_count : 0) << "\n";
        }
        void printPageTable() {
            for (const auto& pair : PageTable) {
                std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
            }
        }
        
        void modificarPage(int _idPage){
            // VERIFICAR SI PAGE ES W
            int op;
            cout << "Seleccione una opcion: 1-Adicionar, 2-Eliminar, 3-Modificar: ";
            cin >> op;
            if (op == 1) {
                int R;
                string n; 
                string archivo, relacion;
                std::cout<<" Adicionando (n) registros de LV(0) LF(1) del (archivo) a la (Relacion) >\n";
                std::cout<<" n> (n/*) ";cin>>n;  ///1
                std::cout<<" Archivo > "; cin>>archivo;
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Registro Long FIJA (1) O Varible(0) > ";cin>>R;
                //adicionarRegistrosPage( _idPage, n , archivo, relacion, R);
                std::ifstream data(archivo);
                std::string registro;
                std::getline(data, registro); // saltandonos encabezado
                bool insertado = false;
                // CAPACIDAD MAXIMA DE REGISTRO calcular si TIPO DE DATO 1(Variable)
                int capMax =0;
                if(R == 1) {capMax = capacMaxRegistro(archivo);} //IF VARIABLE
                //std::cout<<" CAP MAX ES>"<<capMax<<"\n";
                std::cout<<"n>"<<n<<"\n";
                if(n == "*"){
                    while(std::getline(data, registro)){
                        if(R){ insertado = adicionarRegistroP(_idPage, registro, relacion, capMax); }
                        else{insertado = adicionarRegistroP(_idPage, registro, relacion, 0);}
                        if(!insertado) {
                                                    // CONSEGUIR OTRO IDPAGE
                            for (const auto& frame : BufferPool) {
                                if (frame.page) { 
                                    if(R){ insertado = adicionarRegistroP(frame.page->getIdPage(),registro, relacion, capMax);}
                                    else{ insertado = adicionarRegistroP(frame.page->getIdPage(), registro, relacion, 0);}
                                    //insertado = adicionarRegistroP(frame.page->getIdPage(), registro, relacion, 0); 
                                }
                                if(insertado) break;
                            }
                            if(!insertado) std::cout<<" No hay espacio suficiente en PAGES \n";
                        }
                    
                    }
                    data.close(); 
                }
                else{
                    for(int i=0; i< stoi(n) ; i++){
                        std::getline(data, registro);
                        // rLF = funcion RLV = 0
                        if(R){ insertado = adicionarRegistroP(_idPage,registro, relacion, capMax);}
                        else{ insertado = adicionarRegistroP( _idPage, registro, relacion, 0);}
                        if(!insertado) {
                            // CONSEGUIR OTRO IDPAGE
                            for (const auto& frame : BufferPool) {
                                if (frame.page) { 
                                    if(R){ insertado = adicionarRegistroP(frame.page->getIdPage(),registro, relacion, capMax);}
                                    else{ insertado = adicionarRegistroP(frame.page->getIdPage(), registro, relacion, 0);}
                                    //insertado = adicionarRegistroP(frame.page->getIdPage(), registro, relacion, 0); 
                                }
                                if(insertado) break;
                            }
                            if(!insertado) std::cout<<" No hay espacio suficiente en PAGES \n";
                        }
                    }
                    data.close();
                }
            } else if (op == 2) {
                string relacion, condicion;
                std::cout<<" Eliminando registro de la (relacion) donde (condicion) >\n";
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Condicion > ";getline(std::cin >> std::ws, condicion);
                eliminarRegistroPage(_idPage, relacion, condicion);
            } else if (op == 3) {
                string relacion, condicion, atributo;
                std::cout<<" Actualizando en la (relacion) el (atributo) del registro donde (condicion) >\n";
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Atributo > ";getline(std::cin >> std::ws, atributo);
                std::cout<<" Condicion > ";getline(std::cin >> std::ws, condicion);
                modificarRegistroPage(_idPage, relacion, atributo, condicion);
            } else { cout << " Ingrese una opción valida! \n "; return; }
        }
};

void displayMenu() {
    cout << "\n----- MENU -----\n";
    cout << "1. New page\n";
    cout << "2. Pin page\n";
    cout << "3. Unpin page\n";
    cout << "4. Get page\n";
    cout << "5. Modificar Page\n";
    cout << "6. Mostrar estado del buffer\n";
    cout << "7. ALL PIN_COUNT 0\n";
    cout << "8. Salir\n";
    cout << "Elija una opcion: ";
}

void MenuBuffer(Disco* &my_disk) {
    //int bufferSize = 8000;
    //int pageSize = 2000;
    int choice;
    cout << " > Metodo de reemplazo " << endl;
    cout << " LRU (0) o CLOCK (1) > ";cin>>choice;

    Buffer buffer(my_disk->getCapacidadBloque() *4, my_disk->getCapacidadBloque(), my_disk, choice);
    // PREGUNTAR METODO DE REEMPLAZO


    
    do {
        buffer.printBuffer();
        displayMenu();
        cin >> choice;


        switch (choice) {
        case 1: {
            int pageId; char funcion; bool pinned;
            cout << "Ingrese el ID de la nueva pagina: ";
            cin >> pageId;
            cout << " R/W? ";cin>>funcion;
            cout << "PINNED? ";cin>>pinned;
            buffer.newPage(pageId, funcion, pinned);
            break;
        }
        case 2: {
            int pageId; char funcion; bool pinned;
            cout << "Ingrese el ID de la pagina a fijar: ";
            cin >> pageId;
            cout << " R/W? ";cin>>funcion;
            cout << "PINNED? ";cin>>pinned;
            buffer.pinPage(pageId, funcion, pinned);
            break;
        }
        case 3: {
            int pageId;
            cout << "Ingrese el ID de la pagina a liberar: ";
            cin >> pageId;
            buffer.unpinPage(pageId);
            break;
        }
        case 4:{
            int pageId; char funcion;  bool pinned;
            cout << "Ingrese el ID de la pagina a conseguir: ";
            cin >> pageId;
            cout << " R/W? ";cin>>funcion;
            cout << "PINNED? ";cin>>pinned;
            buffer.getPage(pageId, funcion, pinned);
            break;
        }
        case 5:{
            int pageId;
            cout << "Ingrese el ID de la pagina a modificar: ";
            cin >> pageId;
            buffer.pinPage(pageId, 'W', 0);
            buffer.modificarPage(pageId);
            char guardar;
            cout << "¿Desea guardar los cambios? (S/N): ";
            cin >> guardar;
            if (guardar == 'S' || guardar == 's') {
                buffer.flushPage(pageId);
                buffer.unpinPage(pageId);
            }
            break;
        }
        case 6: {
            buffer.printBuffer();
            break;
        }
        case 7: {
            //buffer.resetPin_count();
            //buffer.printBuffer();
            //buffer.printStats();
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