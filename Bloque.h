
#include <fstream>
#include <sstream> 
#include <iostream>

void consultarBloque(int _nroBloque){
    std::ifstream dirBloque("dirBloques.txt");
    std::string bloque, sector, dir;

    for(int i=0; i<_nroBloque;i++){std::getline(dirBloque, bloque);}
    std::stringstream strB(bloque);

    std::cout<<"Los registros del Bloque "<<_nroBloque<<" estan > \n";
    std::getline(strB, sector, '_'); // NOS SALTAMOS BLOQUE
    while (std::getline(strB, sector, '_')){
        std::stringstream strS(sector);
        getline(strS, dir, '#'); // NOS SALTAMOS CAPACIDAD
        getline(strS, dir, '/');
        std::cout<<"Plato "<<dir;
        getline(strS, dir, '/');
        std::cout<<" Superficie "<<dir;
        getline(strS, dir, '/');
        std::cout<<" Pista "<<dir;
        getline(strS, dir, '#');
        std::cout<<" Sector "<<dir<<"\n";   
    }
    dirBloque.close();
}
