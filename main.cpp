#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

#include "Disco.h"

using namespace std;

int main(){
    int opcion; std::string archivo = "", criterio ="", registro = "";
    int n, R; std::string relacion;
    std::string nd;

    Disco* Disco1;
    int platos, pistas, sectores, bloques, tamSector, tamBloque;
    cout << " > Crear DISCO " << endl;
    cout << " DEFAULT (0) o MANUAL (1) > ";cin>>opcion;
    if(opcion == 0) {  Disco1 = new Disco(); }
    else if(opcion == 1) {
        
        std::cout<<" Cantidad de platos> ";cin>>platos;
        std::cout<<" Cantidad de pistas> ";cin>>pistas;
        std::cout<<" Cantidad de Sectores> ";cin>>sectores;
        std::cout<<" Capacidad de sector> ";cin>>tamSector;
        std::cout<<" Capacidad de bloque> ";cin>>tamBloque;
        Disco1 = new Disco(platos, pistas, sectores, tamSector, tamBloque);
    }
    std::cout<<" DISCO  CREADO >>\n";
    Disco1->printDisco();


    do {
        // Presentación del menú
        cout << "=== Menu ===" << endl;
        cout << "1. Adicionar relacion desde archivo " << endl;
        cout << "2. Adicionar N registros " << endl;
        cout << "3. Adicionar todo CSV " << endl;
        cout << "4. Eliminar registro "<<endl;
        cout << "5. Consultar registros " << endl;
        cout << "6. Imprimir Disco "<< endl;
        cout << "7. Consultar Bloque "<< endl;
        cout << "8. Salir del Menu ..."<< endl;
        cout << "Ingrese su opcion: ";
        
        // Capturar la opción del usuario
        cin >> opcion;

        // Manejar la opción seleccionada
        switch(opcion) {
            case 1:
                cout << " Nombre del Archivo > ";cin>>archivo;
                //Disco1->adicRelacion(archivo);
                Disco1->guardarBloqueSector(1);
                break;
            case 2:
                std::cout<<" Adicionando (n) registros de LV(0) LF(1) del (archivo) a la (Relacion) >\n";
                std::cout<<" n> ";cin>>n; 
                std::cout<<" Archivo > ";cin>>archivo;
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Registro Long FIJA (1) O Varible(0) > ";cin>>R;
                Disco1->adicionarReg(n, archivo, relacion, R);
                break;
            case 3:
                std::cout<<" Adicionando todos los registros del (archivo) a la (Relacion) >\n";
                std::cout<<" Archivo > ";cin>>archivo;
                std::cout<<" Relacion > ";cin>>relacion;
                std::cout<<" Registro Long FIJA (1) O Variable(0) > ";cin>>R;
                Disco1->adicionarCSV(archivo, relacion, R);
                break;
            case 4:
                std::cout<<" Eliminar (registro) de la (relacion) >\n";
                std::cout<<" Registros> ";cin>>registro;
                std::cout<<" Relacion> ";cin>>relacion;
                break;
            case 5:
                cout << " Consultando (N) registros de la (Relacion) con este (criterio) >\n";
                std::cout<<" N (int o *) > ";cin>>nd;
                std::cout<<" Relacion > ";cin>>relacion;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                std::cout<<" Criterio (0 = sin criterio) > ";getline(cin, criterio);
                Disco1->consulta(nd, relacion, criterio);
                break;
            case 6: 
                Disco1->printDisco();
                break;
            case 7: 
                std::cout<<" Que bloque quiere consultar? > ";cin>>n;
                consultarBloque(n);
                break;
            case 8: 
                std::cout<<" SALIENDO DEL MENU //";
                break;
            default:
                cout << "Opcion invalida. Por favor, seleccione una opción valida." << endl;
        }
    } while(opcion != 8);

    return 0;
}