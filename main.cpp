#include <iostream>
#include <vector>
#include <sstream>

#include "Disco.h"

using namespace std;

int main(){
    long long int capacidad;
    int platos, pistas, sectores, bloques, tamSector, tamBloque;
    /*
    std::cout<<"\n Cantidad de platos> ";cin>>platos;
    std::cout<<"\n Cantidad de pistas> ";cin>>pistas;
    std::cout<<"\n Cantidad de Sectores> ";cin>>sectores;
    std::cout<<"\n Capacidad de sector> ";cin>>tamSector;
    //std::cout<<"\n Cantidad de bloques> ";cin>>bloques;*/
    //std::cout<<"\n Capacidad de bloque> ";cin>>tamBloque;

    Disco Disco1;
    Disco1.printDisco();

    //Disco Disco1(platos, pistas, sectores, tamSector, tamBloque);


    // ADICIONANDO RELACION POR ARCHIVO
    Disco1.adicRelacion("Titanic.csv");
    Disco1.adicRelacion("Housing.csv");

    // ADICIONAN REGITROS (1, N, TODO EL ARCHIVO)
    //Disco1.adicionarReg("1;0;3;Braund, Mr. Owen Harris;male;22;1;0;A/5 21171;7.25;;S", "Titanic", 0);
    //Disco1.adicionarReg("13300000;7420;4;2;3;yes;no;no;no;yes;2;yes;furnished", "Housing", 0);
    //Disco1.adicionarReg("2;1;1;Cumings, Mrs. John Bradley (Florence Briggs Thayer);female;38;1;0;PC 17599;71.2833;C85;C", "Titanic");
        Disco1.adicionarReg(10, "Titanic.csv", "Titanic");
        Disco1.adicionarReg(10, "Housing.csv", "Housing");
        Disco1.adicionarReg(10, "Titanic.csv", "Titanic");
        Disco1.adicionarReg(10, "Housing.csv", "Housing");
    //Disco1.adicionarCSV("Housing.csv", "Housing");
    Disco1.adicionarCSV("Titanic.csv", "Titanic");

    consultarBloque(1);

    Disco1.consulta("10", "Titanic", " ");
    Disco1.consulta("*", "Titanic", "PassengerId = 5");
    
    //Disco1.adicionarReg("01;Vanessa", "Tabla1");
    Disco1.printDisco();

    //capacMaxRegistro("Titanic.csv");

    
}