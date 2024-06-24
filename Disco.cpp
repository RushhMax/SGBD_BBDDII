#include <iostream>
#include <vector>
#include <fstream>
#include <direct.h>
#include <sstream> // para mis stringsteam
#include <filesystem>

#include "Fauxiliares.cpp"

using namespace std;

class Disco{
    private:
        std::string nombre; 
        long long int capacidadD; // 200GB en bits
        long long int capaclibre;
        int nroPlatos;
        int nroPistas;
        int nroSectores;
        long long int capacidadS;
        int nroBloques; // POSIBLE OTRA CLASE
        long long int capacBloque; // POSIBLE OTRA CLASE
        std::string diccionario = "diccionario.txt";
        std::string directorioBloques = "dirBloques.txt";

    public:
        // CONSTRUCTORES
        Disco(); // deafult
        Disco(int _platos, int _pistas, int _sectores, long long int _capacSector, int _capacBloque);
        void crearEstructura();
        void crearBloques(); // FUERA DE CLASE
        ~Disco(){ };

        int getCapacidadBloque(){ return capacBloque; };
        void adicRelacion(std::string _archivo);
        void guardarBloqueSector(int nBloque);
        void printDisco();
};

// CONSTRUCTOR DISCO DEFAULT 
Disco::Disco(){
    _mkdir("DISCO");
    this->nombre = "DISCO";
    this->nroPlatos = 4;
    this->nroPistas = 8;  
    this->nroSectores = 20;
    this->capacidadS = 500;
    this->capacidadD = capacidadS * nroSectores * nroPistas * nroPlatos * 2;
    this->capaclibre = capacidadD; 
    this->capacBloque = 2000;
    this->nroBloques = this->capacidadD / this->capacBloque;

    crearEstructura();
    crearBloques();
}

// CONSTRUCTOR DISCO POR PARAMETROS 
Disco::Disco(int _platos, int _pistas, int _sectores, long long int _capacSector, int _capacidadBloque){
    this->nombre = "DISCO";
    this->nroPlatos = _platos;
    this->nroPistas = _pistas;  
    this->nroSectores = _sectores;
    this->capacidadS = _capacSector;
    this->capacidadD = _capacSector * _sectores * _pistas * _platos * 2;
    this->capaclibre = capacidadD; 
    this->capacBloque = _capacidadBloque;
    this->nroBloques = this->capacidadD / this->capacBloque;

    crearBloques();
    crearEstructura();
}

// CONSTRUCTOR DE ESTRUCTURA DE DISCO 
void Disco::crearEstructura(){
    std::cout<<" Creando estructura "<<std::endl;
    _mkdir(nombre.c_str());
    for (int plato = 1; plato <= nroPlatos; plato++){
        std::string dirPlato = nombre + "/Plato" + std::to_string(plato);
        _mkdir(dirPlato.c_str());
        for (int superficie = 1; superficie <= 2; superficie++){
            std::string dirSuperficie = dirPlato + "/S" + std::to_string(superficie);
            _mkdir(dirSuperficie.c_str());
            for (int pista = 1; pista <= nroPistas; pista++){
                std::string dirPista = dirSuperficie + "/Pista" + std::to_string(pista);
                _mkdir(dirPista.c_str());
                for (int sector = 1; sector <= nroSectores; sector++){
                    std::string dirSector = dirPista + "/Sector" + std::to_string(sector) + ".txt";

                    std::ofstream nsector(dirSector);
                    nsector.close();
                }
            }
        }
    }
}

// POLITICA CILINDRICA
void Disco::crearBloques(){
    std::ofstream _dirBloques(directorioBloques);
    std::cout<<" Creando bloques "<<std::endl;
    _mkdir("DISCO/BLOQUES");
    int nroSectoresBLOQUE = capacBloque / capacidadS;

    int nPlato = 1, nS = 1 , nPista = 1, nSector = 1; 
    for(int i=0; i<nroBloques; i++){
        // {espacioLibre}#BLOQUE#1#{capacidadBloque}#SECTORES
        _dirBloques<<capacBloque<<"#2#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_";
        std::string newDir = "DISCO/BLOQUES/Bloque" + std::to_string(i+1) + ".txt";
        std::ofstream nB(newDir);
        nB<<capacBloque<<"#2#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_"; // cabecera de bloque en bloque
        
        // SECTORES 
        for(int j=0; j<nroSectoresBLOQUE; j++){ //deben copiarse solo nroSECTORESBLOQUE 
            _dirBloques<<capacidadS<<"#"<<nPlato<<"/"<<nS<<"/"<<nPista<<"/"<<nSector<<"##_";
            //nB<<capacidadS<<"#"<<nPlato<<"/"<<nS<<"/"<<nPista<<"/"<<nSector<<"##_";
            // FUNCION QUE CONTROLE CRECIEMIENTO DE VARIABLES 
            nS++;
            if(nS % 2 == 1 && nS != 1) {nS = 1; nPlato++;}
            if(nPlato % nroPlatos == 1 && nPlato != 1) {nPlato = 1; nSector++; }
            if(nSector % this->nroSectores == 1 && nSector != 1) { nSector = 1; nPista++;}   
            if(nPista % this->nroPistas == 1 && nPista != 1) { nPista = 1; }
        }
        _dirBloques<<std::endl;
        nB<<std::endl;
        nB.close();
    }_dirBloques.close(); 
}

// IMPORTAR TABLA MANUAL o AUTOMATICO
void Disco::adicRelacion(std::string _archivo){
    std::ofstream _diccionario(diccionario, std::ios::app); // abriendo archivo diccionario
    std::cout<<"\n > Adicionando tabla desde archivo "<<_archivo<<"\n";
    std::ifstream archivo(_archivo);
    std::string linea = "", nombre = ""; 

    char delimi = ';';
    //std::cout<<" DELIMITADOR ? ";std::std::cin>>delimi; 
    
    _diccionario << _archivo.substr(0, _archivo.length() - 4);
    int opc;
    std::cout<<"MANUAL(0) o AUTOMATICO(1)> "; std::cin>>opc;
    if(opc == 0){
        int longMax;
        std::getline(archivo, linea);
        std::stringstream str(linea);
        while(std::getline(str, linea, delimi)){
            _diccionario<<"#"<<linea;
            std::cout<<" TIPO DE DATO "<<linea<<" ? "; std::cin>>linea;
            _diccionario<<"#"<<linea;
            std::cout<<" LONG MAX DE DATO "<<linea<<" ? "; std::cin>>linea;
            _diccionario<<"#"<<linea;
        }
    }else{
        std::string linea1;
        std::getline(archivo, linea1);
        std::stringstream str(linea1);

        // Guardamos la linea 2 (donde estan los datos) con el fin de adquirir el tipo de dato
        std::string linea2;
        std::getline(archivo, linea2);
        std::stringstream str2(linea2);

        // Creamos variables para guardar los encabezados y los tipos de datos
        std::string dato = "";
        std::string tip_dato = "";

        while(std::getline(str, dato, delimi)){ // obtengo ENCABEZADO
            _diccionario<<"#"<<dato;
            tip_dato = ""; //reiniciando (evitar errores)
            std::getline(str2, tip_dato, delimi); // obtengo TIPO DE DATO
            // AHORA AVERIGUAREMOS EL TIPO DE DATO CON TIP_DATO
            if(isalpha(tip_dato[0])) { _diccionario<<"#string#30"; } // si dato comienza con char -> string
            else if(is_IntFlo(tip_dato) == 'T'){ _diccionario<<"#int#4";} // si dato es entero
            else if(is_IntFlo(tip_dato) == 'F'){ _diccionario<<"#float#4";} // si dato es float
            else if(is_IntFlo(tip_dato) == 'S'){ _diccionario<<"#string#30"; } // si dato es string
        }
    }
    _diccionario<<std::endl;
    archivo.close();
}

 // ESTA FUNCION!!  
void Disco::guardarBloqueSector(int nBloque){
    std::ifstream directorio(directorioBloques);

    string R = "";
    // RECOPILAR SECTORES
    std::vector<string> sectores;// VECTOR DE SECTORES EN ESTE BLOQUE
    for(int i=0; i<nBloque; i++) { 
        std::getline(directorio,R);
        cout<<i<<"> "<<R<<endl;
    }
    stringstream cabeceraBloque(R);
    std::getline(cabeceraBloque, R, '_'); // CABECERA BLOQUE
    for(int i=0; i<(capacBloque/capacidadS) + 1; i++){
        std::getline(cabeceraBloque, R, '_');
        stringstream sector(R);
        std::getline(sector, R, '#');
        std::getline(sector, R, '#');
        sectores.push_back(R);
    }
    sectores.pop_back();
    directorio.close();

    for(int i=0; i<sectores.size(); i++){
        std::ofstream sector( getdirSector(sectores[i]), std::ios::out); // Abrir para reescribir
        sector.close();
        std::cout<<" SECTOR >"<<sectores[i]<<endl;
    }

    ifstream Bloque(getdirBloque(nBloque));
    int nSector = 0, capacidadSectori = capacidadS;
    if(!Bloque.is_open()) std::cout<<" ERROR ARCHIVO ! ";
    std::getline(Bloque, R);
    std::getline(Bloque, R);
    //std::cout<<R;
    int capacB = 2000 - R.size();
    while(R.size() > 0){
        std::cout<<"\nR.SIZE "<<R.size()<<" capacB <<"<<capacB<<"\n";

        if(R.size() <= capacidadSectori){ // 1/1/1/1
            adicionarRLFArchivo(R, getdirSector(sectores[nSector]));
            capacidadSectori = capacidadSectori - R.size();
            std::cout<<"\nCAPCA. SECTOr>"<<capacidadSectori<<"\n";
            editarCabeceras(nBloque, nSector+1, "|" ,to_string(capacidadSectori),2,directorioBloques);
            R = "";
        }else{
            string R1 = R.substr(0, capacidadSectori);
            adicionarRLFArchivo(R1, getdirSector(sectores[nSector]));
            editarCabeceras(nBloque, nSector+1, "|" ,to_string(0), 2,directorioBloques); 
            nSector++;
            R = R.substr(capacidadSectori);
            capacidadSectori = capacidadS;
        }
    }

    Bloque.close();  
    editarCabeceras(nBloque, 0, "|",to_string(capacB), 2,directorioBloques);
    while(nSector < sectores.size()-1){
        nSector++;
        //editarCabeceras(nBloque, nSector+1, "|" , to_string(capacidadS), 2, directorioBloques);
    }
}

// FUNCION QUE IMPRIME CARACTERISCAS DE DISCO
void Disco::printDisco(){
    std::cout<<" Nombre  > "<<nombre<<"\n";
    std::cout<<" Capacidad > "<<capacidadD<<std::endl;
    std::cout<<" Capacidad Libre > "<<capaclibre<<std::endl;
    std::cout<<" Capacidad Ocupada> "<<capacidadD - capaclibre<<std::endl;
    std::cout<<"  >  Nro de Platos "<<nroPlatos<<std::endl;
    std::cout<<"  >  Nro de Pistas "<<nroPistas<<std::endl;
    std::cout<<"  >  Nro de Sectores "<<nroSectores<<std::endl;
    std::cout<<"  >  Capacidad de Sector "<<capacidadS<<std::endl;
    std::cout<<"  >  Nro de Bloques "<<nroBloques<<std::endl;
    std::cout<<"  >  Capacidad del Bloque "<<capacBloque<<std::endl;
}

void MenuDisco(Disco* &Disco1){
    int opcion; 
    std::string archivo = "", criterio ="", registro = "";
    int n, R; std::string relacion;
    std::string nd;

    /*int platos, pistas, sectores, bloques, tamSector, tamBloque;
    cout << " > Crear DISCO " << endl;
    cout << " DEFAULT (0) o MANUAL (1) > ";std::cin>>opcion;
    if(opcion == 0) {  Disco1 = new Disco(); }
    else if(opcion == 1) {
        
        std::cout<<" Cantidad de platos> ";std::cin>>platos;
        std::cout<<" Cantidad de pistas> ";std::cin>>pistas;
        std::cout<<" Cantidad de Sectores> ";std::cin>>sectores;
        std::cout<<" Capacidad de sector> ";std::cin>>tamSector;
        std::cout<<" Capacidad de bloque> ";std::cin>>tamBloque;
        Disco1 = new Disco(platos, pistas, sectores, tamSector, tamBloque);
    }
    std::cout<<" DISCO  CREADO >>\n";
    Disco1->printDisco();*/
    do {
        // Presentación del menú
        cout << "=== Menu DISCO ===" << endl;
        cout << "1. Adicionar relacion desde archivo " << endl;
        cout << "2. Imprimir Disco "<< endl;
        cout << "3. Consultar Bloque "<< endl;
        cout << "4. Salir del Menu ..."<< endl;
        cout << "Ingrese su opcion: ";
        
        // Capturar la opción del usuario
        std::cin >> opcion;

        // Manejar la opción seleccionada
        switch(opcion) {
            case 1:
                cout << " Nombre del Archivo > ";std::cin>>archivo;
                Disco1->adicRelacion(archivo);
                break;
            case 2:
                Disco1->printDisco();
                break;
            case 3: 
                std::cout<<" Que bloque quiere consultar? > ";std::cin>>n;
                consultarBloque(n);
                break;
            case 4: 
                std::cout<<" SALIENDO DEL MENU //";
                break;
            default:
                cout << "Opcion invalida. Por favor, seleccione una opción valida." << endl;
        }
    } while(opcion != 4);
}