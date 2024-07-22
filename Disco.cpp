#include <iostream>
#include <vector>
#include <fstream>
#include <direct.h>
#include <sstream> // para mis stringsteam
#include <filesystem>

#include "Fauxiliares.cpp"
#include "BPlusTree.cpp"

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
        int nroBloques; 
        long long int capacBloque; 
        std::string diccionario = "diccionario.txt";
        std::string directorioBloques = "dirBloques.txt";
        // INDICES

    public:
        // CONSTRUCTORES
        //Disco(); // deafult
        Disco(int _platos = 4, int _pistas = 8, int _sectores = 20, long long int _capacSector = 500, int _capacidadBloque = 2000);
        void crearEstructura();
        void crearBloques(); // FUERA DE CLASE
        ~Disco(){ };

        int getCapacidadBloque(){ return capacBloque; };
        void adicRelacion(std::string _archivo);
        vector<string> getSectores(int _nroBloque);
        void indices(int _nBloque);
        void guardarBloqueSector(int nBloque);
        void printDisco();
};

// CONSTRUCTOR DISCO POR PARAMETROS 
Disco::Disco(int _platos, int _pistas, int _sectores, long long int _capacSector, int _capacidadBloque){
    _mkdir("DISCO");
    this->nombre = "DISCO";
    this->nroPlatos = _platos;
    this->nroPistas = _pistas;  
    this->nroSectores = _sectores;
    this->capacidadS = _capacSector;
    this->capacidadD = _capacSector * _sectores * _pistas * _platos * 2;
    this->capaclibre = capacidadD; 
    this->capacBloque = _capacidadBloque;
    this->nroBloques = this->capacidadD / this->capacBloque;

    //crearBloques();
    //crearEstructura();
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
        // {espacioLibre}#tipoDeBloque#BLOQUE#1#{capacidadBloque}#RELACION
        _dirBloques<<capacBloque<<"#2#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_";
        std::string newDir = "DISCO/BLOQUES/Bloque" + std::to_string(i+1) + ".txt";
        std::ofstream nuevoBloque(newDir);
        nuevoBloque<<capacBloque<<"#2#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_"; // cabecera de bloque en bloque
        
        // SECTORES 
        for(int j=0; j<nroSectoresBLOQUE; j++){ 
            _dirBloques<<capacidadS<<"#"<<nPlato<<"/"<<nS<<"/"<<nPista<<"/"<<nSector<<"##_";
            //nB<<capacidadS<<"#"<<nPlato<<"/"<<nS<<"/"<<nPista<<"/"<<nSector<<"##_";
            nS++;
            if(nS % 2 == 1 && nS != 1) {nS = 1; nPlato++;}
            if(nPlato % nroPlatos == 1 && nPlato != 1) {nPlato = 1; nSector++; }
            if(nSector % this->nroSectores == 1 && nSector != 1) { nSector = 1; nPista++;}   
            if(nPista % this->nroPistas == 1 && nPista != 1) { nPista = 1; }
        }
        _dirBloques<<std::endl;
        nuevoBloque<<std::endl;
        nuevoBloque.close();
    }_dirBloques.close(); 
}

// IMPORTAR TABLA MANUAL o AUTOMATICO // asignar numeros de bloque
void Disco::adicRelacion(std::string _archivo){
    std::ofstream _diccionario(diccionario, std::ios::app); // abriendo archivo diccionario
    std::cout<<"\n > Adicionando tabla desde archivo "<<_archivo<<"\n";
    std::ifstream archivo(_archivo);
    std::string linea = "", nombre = ""; 

    char delimi = ';';
    
    // NRO DE BLOQUES A RELACIO
    //int nBloques;
    //cout<<" > Cuantos bloques desea asignar a esta relacion";cin>>nBloques;
    
    _diccionario << _archivo.substr(0, _archivo.length() - 4);
    int opc;
    std::cout<<" > MANUAL(0) o AUTOMATICO(1)> "; std::cin>>opc;
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
vector<string> Disco::getSectores(int _nroBloque){
    std::ifstream directorio("dirBloques.txt");

    string R = "";
    // RECOPILAR SECTORES
    std::vector<string> sectores;
    for(int i=0; i<_nroBloque; i++) { 
        std::getline(directorio,R);
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
    return sectores;
}
void Disco::indices(int _nBloque){
    ifstream Bloque(getdirBloque(_nBloque));
    string registros;
    int nSector = 0, capacidadSectori = capacidadS;
    if(!Bloque.is_open()) std::cout<<" ERROR ARCHIVO ! ";

    std::getline(Bloque, registros); // HEADER
    vector<string> vectorHeader = getHeader(registros);
    for(int i=0; i<vectorHeader.size(); i++){
        cout<<vectorHeader[i]<<endl;
    }
    int tipoR = stoi(vectorHeader[1]);
    int nroRelacion = stoi(vectorHeader[5].substr(1));
    string nombreRelacion = getNombreRelacion(nroRelacion);

    int nodos;
    cout<<"> NODOS ";cin>>nodos;
    BPlusTree<int> BPlusTree_indices(nodos, nombreRelacion, "userid" );


    std::getline(Bloque, registros);  // SLOTS
    //std::getline(Bloque, registros);  // REGISTROS
    string registro;
    vector<pair<int,int>> posiciones;
    vector<int> vectorSlots;
    if(tipoR == 1) {// FIJA
        vector<pair<string,int>> longitudes = longMaxEsquema(nombreRelacion);
        int longitudFija = getCapacMaxRegistro(longitudes);
        getline(Bloque, registros); // REGISTROS 
        Bloque.close();
        int posfinal = registros.size();
        int pos = 0;
        while (pos < posfinal){
            registro = registros.substr(pos,longitudFija);
            cout<<" REGISTRO >"<<registro<<endl;
            vector<string> vectorRegistro = getVectorRegistro(registro, nombreRelacion, tipoR);
            BPlusTree_indices.set(stoi(vectorRegistro[1]), make_pair(_nBloque, pos));
            pos+=longitudFija;
        }
    }
    else if(tipoR == 0) {// VARIABLE
        stringstream slots(registros);
        getline(Bloque, registros); // REGISTROS
        Bloque.close();
        string aux;
        int post;
        int pos = 0;
        while(getline(slots, aux, '|')){    
            if(aux == "_") break;
            post = stoi(aux);
            if (pos + post > registros.size()) break;
            registro = registros.substr(pos,post);
            cout<<" REGISTRO >"<<registro<<endl;
            vector<string> vectorRegistro = getVectorRegistro(registro, nombreRelacion, tipoR);
            BPlusTree_indices.set(stoi(vectorRegistro[1]), make_pair(_nBloque, pos));
            pos += post;
        }
    }else { return; }

    BPlusTree_indices.print(nullptr, "", true, nullptr);
    ofstream outfile(BPlusTree_indices.archivo);
    BPlusTree_indices.print(nullptr, "", true, &outfile);
    Bloque.close();
}
// FUNCION GUARDA BLOQUE EN SECTORES
void Disco::guardarBloqueSector(int nBloque){
    // RECOPILAR SECTORES
    std::vector<string> sectores = getSectores(nBloque);

    cout<<"\n\n> GUARDANDO registros del bloque "<<nBloque<<" en las siguientes direcciones>>\n";
    consultarBloque(nBloque);

    for(int i=0; i<sectores.size(); i++){
        std::ofstream sector( getdirSector(sectores[i])); // Abrir para reescribir
        sector.close();
    }
    string R = "";

    ifstream Bloque(getdirBloque(nBloque));
    int nSector = 0, capacidadSectori = capacidadS;
    if(!Bloque.is_open()) std::cout<<" ERROR ARCHIVO ! ";

    std::getline(Bloque, R); // HEADER
    std::getline(Bloque, R);  // SLOTS
    std::getline(Bloque, R);  // REGISTROS

    int capacB = capacBloque - R.size();
    while(R.size() > 0){
        //std::cout<<"\nR.SIZE "<<R.size()<<" capacB <<"<<capacB<<"\n";

        if(R.size() <= capacidadSectori){ // 1/1/1/1
            adicionarRLFArchivo(R, getdirSector(sectores[nSector]));
            capacidadSectori = capacidadSectori - R.size();
            //std::cout<<"\nCAPCA. SECTOr>"<<capacidadSectori<<"\n";
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
        editarCabeceras(nBloque, nSector+1, "|" , to_string(capacidadS), 2, directorioBloques);
    }

    indices(nBloque);
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

    do {
        // Presentación del menú
        cout << "__________________________________________________\n";
        cout << "\n----- MENU -----\n";
        cout << "1. Adicionar relacion desde archivo " << endl;
        cout << "2. Imprimir Disco "<< endl;
        cout << "3. Consultar Bloque "<< endl;
        cout << "4. Guardar Bloque a sectores "<< endl;
        cout << "5. Salir del Menu ..."<< endl;
        cout << "__________________________________________________\n\n";
        cout << "Elija una opcion: ";
        
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
                std::cout<<" Que bloque quiere consultar? > ";std::cin>>n;
                Disco1->guardarBloqueSector(n);
                break;            
            case 5: 
                std::cout<<" SALIENDO DEL MENU //";
                break;
            default:
                cout << "Opcion invalida. Por favor, seleccione una opción valida." << endl;
        }
    } while(opcion != 5);
}