#ifndef __LRU_H__
#define __LRU_H__

#include <vector>

using namespace std;

class LRU {
    private:
        std::vector<int> Pages;

    public:

        LRU(){};
        void addPage(int ID_Page){
            // BUSCAR PAGINA
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i]){             // si pagina esta 
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                    Pages.push_back(ID_Page);
                }
            }     
            // si pagina no esta
            Pages.push_back(ID_Page);//añadir al ultimo
        }
        void deletePage(int ID_Page){
            for(int i=0; i<Pages.size();i++) {
                if(ID_Page == Pages[i]){             // si pagina esta 
                    Pages.erase(Pages.begin() + i);// eliminar pagina y volverla agregar al ultimo
                }
            }    
        }
        int getLRU(int i){ // 0 ...
            return Pages[Pages.size() - i - 1];
        }
};


#endif 