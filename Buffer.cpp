#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <direct.h>

//#include "Fauxiliares.cpp"
#include "Disco.cpp"

using namespace std;

class Page {
    private:
        int idPage;
        string dir_page;
        bool dirty_bit;
        int pin_count;
        int last_used; // LRU

    public:
        Page(int _id) :idPage(_id), dirty_bit(false), pin_count(1), last_used(1) {
            dir_page = "BUFFERPOOL/Page" + to_string(_id) + ".txt";
            ifstream block("DISCO/BLOQUES/Bloque" + to_string(_id) + ".txt"); 
            ofstream page(dir_page);
            string line = "";
            while (getline(block, line)){ page<<line<<endl;}
            page.close();
            block.close();
        }

        int getIdPage() { return idPage; }
        bool getDirtyBit() { return dirty_bit; }
        int getPinCount() { return pin_count; }
        int getLastUsed() { return last_used; }
        string getDirPage() { return dir_page; }

        void turnDirtyBit() { dirty_bit = true;}
        void incrementPinCount() { pin_count++;} // PINNING
        void decrementPinCount() { if (pin_count > 0) pin_count--; }
        void incrementlastUsted() { last_used++; }
        void lastUsedNOW(){ last_used = 1; }

        void printPage() {
            ifstream page(dir_page);
            string line;
            while (getline(page, line)) {
                cout << line << "\n";
            }
        }

        void adicionarReg(std::string _registro){ // FUNCION QUE SOLO AGREGA YA SE COMPROBO QUE HAY ESPACIO
            // !! PAGE DE RLV O RLF
            std::ofstream _page(dir_page, std::ios::app);
            _page<<_registro<<"\n";
            _page.close();
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
        Disco* Disco1;
        vector<Frame> BufferPool;
        int Buf_size;
        unordered_map<int, int> PageTable; // ID PAGINA - ID FRAME

        //LRU replacer;

        int hit_count;
        int miss_count;
        int request_count;

        void Delete_LRU() { // FREE PAGE
            int mayorLU = 0; int idF = 0;
            for( auto &Frame : BufferPool){
                if(Frame.page.get()){
                    if ( Frame.page->getLastUsed() > mayorLU && Frame.page->getPinCount() == 0 ){
                        mayorLU = Frame.page->getLastUsed();
                        idF = Frame.idFrame;
                    }
                }
            }
            if(mayorLU == 0 && idF == 0){ std::cout<<" ERROR! No podemos eliminar ninguna pagina! \n"; return; }
            if(BufferPool[idF].page->getDirtyBit() == 1){ // si la pagina fue modificada
                flushPage(BufferPool[idF].page->getIdPage()); 
            }
            remove(BufferPool[idF].page->getDirPage().c_str()); // ARCHIVOS !!
            PageTable.erase(BufferPool[idF].page->getIdPage()); 
            BufferPool[idF].releaseFrame();
        }

    public:
        // Constructor de Buffer
        Buffer(int _Buf_size, int _page_size, Disco* & _Disco) : Buf_size(_Buf_size), Disco1(_Disco), hit_count(0), miss_count(0), request_count(0) {
            // Crear n frames de acuerdo a cuántos bloques (páginas puedan entrar)
            int nFrames = _Buf_size / _page_size;
            for (int i = 0; i < nFrames; ++i) { BufferPool.push_back(Frame(i));}
            _mkdir("BUFFERPOOL");
        }

        // Indicar que la pagina esta en uso
        void pinPage(int idPage, char func) {
            //request_count++;
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) { // SI ESTA
                //hit_count++;
                BufferPool[it->second].page->incrementPinCount(); // PINNING
                if(func == 'W') BufferPool[it->second].page->turnDirtyBit();
            } else { // NO ESTA
                //miss_count++;
                newPage(idPage, func);
            }
            //replacer.addPage(idPage);
        }

        // Indicar que la pagina esta un uso menos
        void unpinPage(int idPage) {
            //std::cout<<"EN UNPIN PAGE \n";
            auto it = PageTable.find(idPage);
            if (it != PageTable.end()) {
                //std::cout<<BufferPool[it->second].page->getIdPage()<<endl;
                if(BufferPool[it->second].page->getPinCount() > 0){
                    BufferPool[it->second].page->decrementPinCount();
                }
                if(BufferPool[it->second].page->getPinCount() == 0){
                    //if it becomes zero, put it in group of replacement candidates.
                    //Delete_LRU();
                }
            }
        }

        // Función para agregar una nueva página al búfer
        Page* newPage(int idPage, char func) {
            request_count++;
            bool created = false;
            int idF;
            // comprobar que algun frame  este vacio
            for(auto &Frame : BufferPool){
                if(Frame.page){  // si la pag del frame existe
                    Frame.page->incrementlastUsted(); 
                    if(Frame.page->getIdPage() == idPage){ // SI ENCUENTRA LA PAGINA
                        hit_count++;
                        Frame.page->lastUsedNOW();
                        Frame.page->incrementPinCount();
                        if(func == 'W') Frame.page->turnDirtyBit();
                        created = true;
                        idF = Frame.idFrame;
                    }
                }
                if(!Frame.page.get() && !created){ // SI EL FRAME ESTA VACIO COLOCA AHI LA PAG
                    miss_count++;
                    Page* NewPage = new Page(idPage);
                    if(func == 'W') NewPage->turnDirtyBit();
                    Frame.setPage(NewPage);
                    PageTable[idPage] = Frame.idFrame;
                    created = true;
                    idF = Frame.idFrame;
                }
            }
            //Si todos estan ocupados entonces LRU;
            if(!created) {
                Delete_LRU();
                for(auto &Frame : BufferPool){
                    if(!Frame.page){ // SI EL FRAME ESTA VACIO COLOCA AHI LA PAG
                        miss_count++;
                        Page* NewPage = new Page(idPage);
                        if(func == 'W') NewPage->turnDirtyBit();
                        Frame.setPage(NewPage);
                        PageTable[idPage] = Frame.idFrame;
                        return Frame.page.get();
                    }
                } return nullptr;
            }
            return BufferPool[idF].page.get();
        }

        // OBTENER PAGINA SOLICITADA POR ID
        Page* getPage(int _idPage, char func){        
            auto it = PageTable.find(_idPage);
            if (it != PageTable.end()) {
                BufferPool[it->second].page->incrementPinCount();
                //replacer.addPage(_idPage);
                return BufferPool[it->second].page.get();
            }
            return newPage(_idPage, func);
        }

        void flushPage(int _idPage){
            string dirPage = "BUFFERPOOL/Page" + to_string(_idPage) + ".txt";
            ofstream block("DISCO/BLOQUES/Bloque" + to_string(_idPage) + ".txt"); 
            ifstream page(dirPage);
            string line = "";
            while (getline(page, line)){ block<<line<<endl;}
            page.close();
            block.close();
            Disco1->guardarBloqueSector(_idPage);
        }

        void FlushAllPages(){}

        void printBuffer() const {
            for (const auto& frame : BufferPool) {
                if (frame.page) { cout<<"Frame ID: "<<frame.idFrame<<", Page ID: "<<frame.page->getIdPage()<< ", Dity bit: "<< frame.page->getDirtyBit() << ", Pin Count: " << frame.page->getPinCount() << ", Last Used: "<< frame.page->getLastUsed() << " \n";
                }else{ cout<<"Frame ID: "<<frame.idFrame<<" > FRAME VACIO! \n";}
            }
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
                adicionarRegistrosPage( _idPage, n , archivo, relacion, R);
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
                std::cout<<" Atributo > ";cin>>atributo;
                std::cout<<" Condicion > ";cin>>condicion;
                // FUNCION
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
    cout << "7. Mostrar estadisticas\n";
    cout << "8. Salir\n";
    cout << "Elija una opcion: ";
}

void MenuBuffer(Disco* &Disco1) {
    //int bufferSize = 8000;
    //int pageSize = 2000;
    Buffer buffer(Disco1->getCapacidadBloque() *5, Disco1->getCapacidadBloque(), Disco1);

    int choice;
    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
        case 1: {
            int pageId; char funcion;
            cout << "Ingrese el ID de la nueva pagina: ";
            cin >> pageId;
            cout << " L/W? ";cin>>funcion;
            buffer.newPage(pageId, funcion);
            break;
        }
        case 2: {
            int pageId; char funcion;
            cout << "Ingrese el ID de la pagina a fijar: ";
            cin >> pageId;
            cout << " L/W? ";cin>>funcion;
            buffer.pinPage(pageId, funcion);
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
            int pageId; char funcion;
            cout << "Ingrese el ID de la pagina a conseguir: ";
            cin >> pageId;
            cout << " L/W? ";cin>>funcion;
            buffer.getPage(pageId, funcion);
            break;
        }
        case 5:{
            int pageId;
            cout << "Ingrese el ID de la pagina a modificar: ";
            cin >> pageId;
            buffer.pinPage(pageId, 'W');
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
            buffer.printStats();
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