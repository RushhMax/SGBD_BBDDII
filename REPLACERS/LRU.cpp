#include <vector>
#include <iostream>
#include <deque>

using namespace std;

struct page_helper{
    int idPage;
    int pinCount;
    int dirtyBit;
    bool pinned;
    deque<pair<char,int>> requerimientos;
    
    void actualizarDirtyBit() {
        if(requerimientos.size() == 0) { dirtyBit = 0; return;}
        for(int i=requerimientos.size()-1; i>0; i--){
            if(requerimientos[i].second == 1){
                requerimientos[i-1].second = 1;
            }
        }
        dirtyBit = requerimientos[0].second;
    }
    page_helper(int _idPage, int _dirtyBit, int _pinCount, int _pinned, deque<pair<char,int>> _Requerimientos) : idPage(_idPage), pinCount(_pinCount), dirtyBit(_dirtyBit), pinned(_pinned), requerimientos(_Requerimientos) {}
};

class LRU {
    private:
        std::vector<page_helper> Pages; // PAGE - PIN COUNT

    public:

        LRU(){};
        void pin(int ID_Page, char func, bool pinned){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){          // si pagina esta 
                    Pages[i].requerimientos.push_back(make_pair(func, changeFuncInt(func)));
                    Pages[i].actualizarDirtyBit();
                    if(Pages[i].pinned) pinned = false;
                    Pages.push_back(page_helper(ID_Page, Pages[i].dirtyBit, Pages[i].pinCount+1, pinned, Pages[i].requerimientos));
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                    return;
                }
            }  
            newPage(ID_Page, func, pinned);   
        }
        void unpin(int ID_Page){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){             // si pagina esta 
                    if(Pages[i].pinCount > 0) Pages[i].pinCount--; 
                    if(Pages[i].requerimientos.size() > 0){
                        Pages[i].requerimientos.pop_front();
                    } 
                    Pages[i].actualizarDirtyBit();
                }
            }
        }
        void newPage(int ID_Page, char func, bool pinned){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage) {pin(ID_Page, func, pinned); return;}
            }     
            // si pagina no esta
            deque<pair<char, int>> _requerimentos;
            Pages.push_back(page_helper(ID_Page, changeFuncInt(func), 1, pinned, _requerimentos));
            Pages[Pages.size()-1].requerimientos.push_back(make_pair(func, changeFuncInt(func)));
            Pages[Pages.size()-1].actualizarDirtyBit();
        }
        void deletePage(int ID_Page){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i].idPage){             // si pagina esta 
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                }
            }    
        }
        int victima(){ 
            for(int i=0; i<Pages.size(); i++)
                if(Pages[i].dirtyBit == 0 && !Pages[i].pinned) return Pages[i].idPage;
            for(int i=0; i<Pages.size(); i++)
                if(Pages[i].dirtyBit == 1 && !Pages[i].pinned) return Pages[i].idPage;
            cout<<" TODAS LAS PAGINAS CON PINNED! Liberar paginas manualmente!!\n";
            return -1;
        }
        void printLRU(){
            cout << "________ MY LRU _________________\n";
            cout<<"\n LRU >   \n";
            for(int i=0; i<Pages.size(); i++){
                cout<<" PAGE : "<<Pages[i].idPage<<", pin_count : "<<Pages[i].pinCount<<", dirty bit : "<<Pages[i].dirtyBit<<", pinned : "<<Pages[i].pinned<<endl;
            }
        }
};