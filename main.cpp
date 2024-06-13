#include <iostream>

#include "Buffer.cpp"

using namespace std;

int main(){
    Disco* Disco1;
    int opcion; 

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

    int choice, tamBloquePage;
    do {
        cout << "\n----- MENU -----\n";
        cout << "1. MENU DISCO \n";
        cout << "2. MENU BUFFER\n";
        cout << "3. Salir\n";
        cout << "Elija una opcion: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            MenuDisco(Disco1);
            break;
        }
        case 2: {
            MenuBuffer(Disco1);
            break;
        }
        case 3: {
            cout << "Saliendo...\n";
            break;
        }
        default: {
            cout << "Opción no válida. Intente de nuevo.\n";
        }
        }
    } while (choice != 3);
}