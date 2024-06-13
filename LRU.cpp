#include <vector>
#include <iostream>
using namespace std;

struct page_helper{
    int idPage;
    int pinCount;
    int dirtyBit;
    bool pinned;

    page_helper(int _idPage, int _dirtyBit, int _pinCount, int _pinned) : idPage(_idPage), pinCount(_pinCount), dirtyBit(_dirtyBit), pinned(_pinned) {}
};

class LRU {
    private:
        std::vector<page_helper> Pages; // PAGE - PIN COUNT

    public:

        LRU(){};
        void pin(int ID_Page, char func, bool pinned){
            // BUSCAR PAGINA
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){          // si pagina esta 
                    if(func == 'W') Pages[i].dirtyBit = 1;
                    if(Pages[i].pinned) pinned = 0;
                    Pages.push_back(page_helper(ID_Page, Pages[i].dirtyBit, Pages[i].pinCount++, pinned));
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                    return;
                }
            }  
            newPage(ID_Page, func, pinned);   

        }
        void unpin(int ID_Page){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){             // si pagina esta 
                    if(Pages[i].pinCount>0) Pages[i].pinCount--;
                }
            }
        }
        void newPage(int ID_Page, char func, bool pinned){
            // BUSCAR PAGINA
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){   
                    if(func == 'W') Pages[i].dirtyBit = 1;
                    if(Pages[i].pinned) pinned = 0;
                    Pages.push_back(page_helper(ID_Page, Pages[i].dirtyBit, Pages[i].pinCount++, pinned));
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                    return;
                }
            }     
            // si pagina no esta
            if(func == 'W') Pages.push_back(page_helper(ID_Page, 1, 1, pinned ));
            else Pages.push_back(page_helper(ID_Page, 0, 1, pinned));
        }
        void deletePage(int ID_Page){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){             // si pagina esta 
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                }
            }    
        }
        int victima(int i){ // 0 ...
            for(int i=0; i<Pages.size(); i++){
                if(Pages[i].dirtyBit == 0 && !Pages[i].pinned){
                    return Pages[i].idPage;
                }
            }// si paso todos
            for(int i=0; i<Pages.size(); i++){
                if(Pages[i].dirtyBit == 1 && !Pages[i].pinned){
                    int choice;
                    cout<<" Desea liberar la pagina "<<Pages[i].idPage<<" ? (Y = 1 | N = 0)  ";cin>> choice;
                    if(choice == 1) return Pages[i].idPage;
                }
            }
            cout<<" NO PODEMOS ELIMINAR NINGUNA PAGINA! ";
            return -1;
        }
        void printLRU(){
            cout<<"\n LRU >   \n";
            for(int i=0; i<Pages.size(); i++){
                cout<<" PAGE : "<<Pages[i].idPage<<", pin_count : "<<Pages[i].pinCount<<", dirty bit : "<<Pages[i].dirtyBit<<", pinned : "<<Pages[i].pinned<<endl;
            }
        }
};