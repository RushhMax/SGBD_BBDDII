#ifndef __DISCO_H__
#define __DISCO_H__

#include <fstream>
#include <direct.h>
#include <sstream> // para mis stringsteam
#include "Bloque.h"

#include "Fauxiliares.cpp"

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

        void adicRelacion(std::string _archivo);

        //std::string crearRLF(std::string _registro, std::string _relacion, int _long);
        //std::string crearRLV(std::string _registro);

        void escribirReg(std::string _registro, std::string _archivo);
        //void editarDirectorio(int linea, int posicion, std::string _nRelacion, std::string Ndato, std::string _archivo);

        void adicionarReg(std::string _registro, std::string _tabla, int _longMax);
        void adicionarReg(int n, std::string _archivo, std::string _tabla, bool _tipoR);
        void adicionarCSV(std::string _archivo, std::string _tabla, bool _tipoR);
        
        void adicionarRegBLOQUE(std::string _registro, int _nroBloque);

        void guardarBloqueSector(int nBloque);

        void consulta(std::string n, std::string _relacion, std::string _criterio);
        void consultaS(std::string _relacion, std::string _dirS, int &_contR, int _n, std::string _criterio);
        void printDisco();
};

// CONSTRUCTOR DISCO DEFAULT // BIEN
Disco::Disco(){
    std::string comando = "rm -r DISCO";
    std::system(comando.c_str());  
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

// CONSTRUCTOR DISCO POR PARAMETROS // BIEN
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

// CONSTRUCTOR DE ESTRUCTURA DE DISCO // BIEN
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

// POLITICA 
void Disco::crearBloques(){
    std::ofstream _dirBloques(directorioBloques);
    std::cout<<" Creando bloques "<<std::endl;
    _mkdir("DISCO/BLOQUES");
    int nroSectoresBLOQUE = capacBloque / capacidadS;

    int nPlato = 1, nS = 1 , nPista = 1, nSector = 1; 
    for(int i=0; i<nroBloques; i++){
        // {espacioLibre}#BLOQUE#1#{capacidadBloque}#SECTORES
        _dirBloques<<capacBloque<<"#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_";
        std::string newDir = "DISCO/BLOQUES/Bloque" + std::to_string(i+1) + ".txt";
        std::ofstream nB(newDir);
        nB<<capacBloque<<"#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_"; // cabecera de bloque en bloque
        
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
    }//std::cout<<" Hay "<<nroBloques<<" con capacidad "<<capacBloque<<"\n";
    _dirBloques.close();
    
}

// IMPORTAR TABLA MANUAL o AUTOMATICO // BIEN
void Disco::adicRelacion(std::string _archivo){
    std::ofstream _diccionario(diccionario, std::ios::app); // abriendo archivo diccionario
    std::cout<<"\n > Adicionando tabla desde archivo "<<_archivo<<"\n";
    std::ifstream archivo(_archivo);
    std::string linea = "", nombre = ""; 

    char delimi = ';';
    //std::cout<<" DELIMITADOR ? ";std::cin>>delimi; 
    
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

// FUNCION PUNTUAL REGISTRO EN SECTOR.TXT
void Disco::escribirReg(std::string _registro, std::string _archivo){
    std::cout<<" ESCRIBIENDO >"<<_registro<<" en "<<_archivo<<endl;
    std::stringstream ss(_archivo);
    std::vector<std::string> ubi;
    std::string token = "";
    while(std::getline(ss, token, '/')){ ubi.push_back(token);}
    
    std::string dirSector = "DISCO/Plato" + ubi[0] + "/S" + ubi[1] + "/Pista" + ubi[2] + "/Sector" + ubi[3] + ".txt";

    std::ofstream sector(dirSector, std::ios::app);
    sector<<_registro<<"\n";
    sector.close();
}

// FUNCION PUNTUAL REGISTRO EN BLOQUE N
void Disco::adicionarRegBLOQUE(std::string _registro, int _nroBloque){
    std::string dir = "DISCO/BLOQUES/Bloque" + std::to_string(_nroBloque) + ".txt"; 
    std::ofstream bloque(dir, std::ios::app);
    bloque<<_registro<<"\n";
    bloque.close();
}   

// FUNCION PARA ADICIONAR UN REGISTRO EN UNA TABLA ESPECIFICA
void Disco::adicionarReg(std::string _registro, std::string _relacion, int _longMax){
    std::ifstream bloques(directorioBloques); // abrimos directorio
    // variables que nos serviran
    std::string lineaBloque = "", lineaSector ="", dato = ""; 
    char delimitador = '_'; 
    int NBloque = 0, NSector = 0;
    int espacLibreINTB = 0, espacLibreINTS = 0;
    int nroSectBLOQUE = capacBloque / capacidadS; // NRO DE SECTORES EN BLOQUE
    
    std::string _nroR = std::to_string(NroRelacion(_relacion));
    std::string R = ""; 
    if(_longMax != 0) {R +=  crearRLF(_registro, _relacion, _longMax);  }
    else{ R += crearRLV(_registro); } // REGISTRO LONGITUD VARIABLE
    
    this->capaclibre -= R.size();  

    while (std::getline(bloques, lineaBloque)){ // LEE PRIMERA LINEA DE BLOQUE
        NBloque++; 
        NSector = 0;

        //std::cout<<">"<<lineaBloque.size()<<">"<<lineaBloque<<">\n";
        std::stringstream _linea(lineaBloque);
        
        // OBTENEMOS ESPACIO LIBRE BLOQUE
        std::getline(_linea, dato, '#'); 
        espacLibreINTB = std::stoi(dato); 

        // SI HAY ESPACIO LIBRE
        //std::cout<<"LB>"<<espacLibreINTB<<">"<<RLF.size()<<"?\n";
        
        if(espacLibreINTB > R.size()){
            getline(_linea, lineaSector, '_'); // corremos el bloque
            while(getline(_linea, lineaSector, '_')){// BUSCANDO SECTOR LIBRE
                NSector++;
                //std::cout<<">"<<lineaSector.size()<<">"<<lineaSector<<">\n";
                std::stringstream _linea(lineaSector);

                // ESPACIO LIBRE DE SECTOR
                std::getline(_linea, dato, '#');
                espacLibreINTS = std::stoi(dato);

                // SI EN ESTE SECTOR ESTA LIBRE ENTONCES
                //std::cout<<"LS>"<<espacLibreINTS<<">"<<RLF.size()<<"?\n";
                if(espacLibreINTS > R.size()){
                    std::getline(_linea, dato, '#'); // Obtenemos dir de Sector
                    
                    
                    //CALCULAMOS CAPACIDAD BLOQUE Y SECTOR 
                    espacLibreINTB -= R.size();
                    espacLibreINTS -= R.size();

                    editarCabeceras(NBloque, 0, _nroR ,  std::to_string(espacLibreINTB), directorioBloques);
                    editarCabeceras(NBloque, NSector, _nroR ,std::to_string(espacLibreINTS), directorioBloques);

                    editarCabeceras(1, 0, _nroR ,  std::to_string(espacLibreINTB),  "DISCO/BLOQUES/Bloque" + std::to_string(NBloque) + ".txt");
                    //editarDirectorio(1, NSector, _nroR ,std::to_string(espacLibreINTS),  "DISCO/BLOQUES/Bloque" + std::to_string(NBloque) + ".txt");

                    std::cout<<"> Insertando >"<<R<<"> de tamano "<<R.size()<<" en "<<dato<<std::endl;
                    
                    // AGREGAMOS 
                    if(_longMax != 0) R = _nroR + "#1#" + R; // RLF
                    else R = _nroR + "#0#" + R; // RLV
                    adicionarRegBLOQUE(R, NBloque); // ESCRIBIRMOS EN BLOQUE
                    escribirReg(R, dato); // ESCRIBIMOS

                    bloques.close();
                    return;
                }//std::cout<<" SECTOR OCUPADO NEXT! \n";
            }//std::cout<<" SECTORES DE ESTE BLOQUE OCUPADOS NEXT! \n";
        }
    }
    //std::cout<<" TODOS LOS BLOQUES OCUPADOS! Lo siento! \n"; 
    bloques.close();
}

// FUNCION ADICIONAR N REGISTROS DE UN CSV
void Disco::adicionarReg(int n, std::string _archivo, std::string _tabla, bool _TipoR){
    std::ifstream data(_archivo);
    std::string registro;
    std::getline(data, registro); // saltandonos encabezado
    // CAPACIDAD MAXIMA DE REGISTRO calcular si TIPO DE DATO 1(Variable)
    int capMax =0;
    if(_TipoR) {capMax = capacMaxRegistro(_archivo);} //IF VARIABLE
    //std::cout<<" CAP MAX ES>"<<capMax<<"\n";
    for(int i=0; i<n; i++){
        std::getline(data, registro);
        // rLF = funcion RLV = 0
        if(_TipoR) adicionarReg(registro, _tabla, capMax);
        else{adicionarReg(registro, _tabla, 0);}
    }
    data.close();
}

// FUNCION PARA ADICIONAR TODOS LOS REGISTROS DE UN CSV.
void Disco::adicionarCSV(std::string _archivo, std::string _tabla, bool _TipoR){
    std::ifstream data(_archivo);
    std::string registro;
    std::getline(data, registro); // saltandonos encabezado
    int capMax =0;
    if(_TipoR) {capMax = capacMaxRegistro(_archivo);} //IF VARIABLE
    while(std::getline(data, registro)){
        if(_TipoR) adicionarReg(registro, _tabla, capMax);
        else{adicionarReg(registro, _tabla, 0);}
    }
    data.close(); 
}

void Disco::guardarBloqueSector(int nBloque){
    // ACTUALIZAR DIRECTORIO
    ifstream Bloque("DISCO/BLOQUES/Bloque" + to_string(nBloque) + ".txt");
    std::fstream directorio(directorioBloques, std::ios::binary | std::ios::in | std::ios::out);
    int nroSectoresBLOQUE = capacBloque / capacidadS;
    std::vector<string> sectores;// VECTOR DE SECTORES EN ESTE BLOQUE

    int tam = 0;
    string R = "";
    //std::ostringstream nuevo;

    getline(Bloque, R); // me paso la cabecera7
    stringstream cabecerabloque(R);
    getline(cabecerabloque, R, '#');
    int capacidadBloque = stoi(R);
    // RECOGER ESTE R Y ACTULIZAR EN DIRECTORIO

    for(int i=0; i<nBloque; i++) { getline(directorio,R);}
    stringstream cabeceraBloque(R);
    getline(cabeceraBloque, R, '_'); // CABECERA BLOQUE
    while (getline(cabeceraBloque, R, '_')){
        stringstream sector(R);
        getline(sector, R, '#');
        getline(sector, R, '#');
        sectores.push_back(R);
    }
    sectores.pop_back();
    
    for(int i=0; i<sectores.size(); i++){
        std::cout<<" SECTOR >"<<sectores[i]<<endl;
    }

    // RECORRER CADA REGISTRO y si puede entrar en sector ingresarlo
    // actulizar SECTOR CABECERA
    int nSector = 0, capacidadSectori = capacidadS;
    while(getline(Bloque, R)){
        std::string s(1, R[2]);
        editarCabeceras(nBloque, 0, s ,to_string(capacidadBloque), directorioBloques);
        tam = R.size() - 4;
        if(tam <= capacidadSectori){ // 1/1/1/1
            escribirReg(R, sectores[nSector]);
            capacidadSectori -= tam;
            std::string s(1, R[2]);
            editarCabeceras(nBloque, nSector+1, s ,to_string(capacidadSectori), directorioBloques);
        }else{
            capacidadSectori = capacidadS;
            nSector++; 
            escribirReg(R, sectores[nSector]);
            capacidadSectori -= tam;
            std::string s(1, R[2]);
            editarCabeceras(nBloque, nSector+1, s ,to_string(capacidadSectori), directorioBloques);
        }
    }

}

void Disco::consultaS(std::string _relacion, std::string _dirS, int &_contR, int _n, std::string _criterio){
    std::cout<<" EN CONSULTAS S >"<<_relacion<<">"<<_dirS<<">"<<_contR<<">"<<_n<<">"<<_criterio<<"\n";
    std::stringstream dirS(_dirS); std::stringstream cr(_criterio);
    std::vector<std::string> ubi, criterio;
    std::string token = "";
    while(std::getline(dirS, token, '/')){ ubi.push_back(token);}
    if(_criterio != "0") while(std::getline(cr, token, ' ' )){ criterio.push_back(token);}

    int nAtributo = 0;
    if(_criterio != "0"){
        std::ifstream dicc(diccionario);
        for(int i=0; i<stoi(_relacion); i++){getline(dicc, token);}
        std::stringstream relac(token);
        while(getline(relac, token, '#')){ 
            if(token == criterio[0]) break; 
            nAtributo++;
        }
    }
    
    std::string dirSector = "DISCO/Plato" + ubi[0] + "/S" + ubi[1] + "/Pista" + ubi[2] + "/Sector" + ubi[3] + ".txt";
    std::ifstream sector(dirSector);


    std::string _registro; int contAT = 1;
    while (getline(sector, _registro)){
        std::stringstream STR(_registro); std::string r;
        getline(STR, r, '#');
        if(_relacion == r){
            if(_criterio == "0"){
                _registro = _registro.substr(2);
                std::cout<<_registro<<"\n";
                _contR++;
                if(_n == _contR){sector.close(); return;} 
            }else{
                bool critCUM = false;
                while(getline(STR, r, ' ')){
                    //std::cout<<" nAtributo >"<<nAtributo<<" CONTAT "<<contAT;    
                    if(contAT == nAtributo){
                        std::cout<<" COMPARANDO >"<<r<<"> con >"<<criterio[2]<<">\n";

                        //if(is_IntFlo(criterio[2]) == 'T'){ criterio[2] = std::stoi(criterio[2]); r = std::stoi(r);}
                        //else if(is_IntFlo(criterio[2]) == 'F'){ criterio[2] = std::stof(criterio[2]); r = std::stof(r);} 
                        //std::cout<<_registro<<"\n";
                        if(criterio[1] == "="){ if(r == criterio[2] ) critCUM = true;}
                        else if(criterio[1] == ">="){ if(r >= criterio[2] ) critCUM = true;}
                        else if(criterio[1] == "<="){ if(r <= criterio[2] ) critCUM = true;}
                        else if(criterio[1] == "<"){ if(r < criterio[2] ) critCUM = true;}
                        else if(criterio[1] == ">"){ if(r >criterio[2] ) critCUM = true;}
                        break;
                    }
                    contAT++;
                }
                if(critCUM){
                    _registro = _registro.substr(2);
                    std::cout<<_registro<<"\n";
                    _contR++;
                    if(_n == _contR){sector.close(); return;} 
                }
            }
        } 
    }
    sector.close();
}

void Disco::consulta(std::string _n, std::string _relacion, std::string _criterio){
    std::ifstream dir(directorioBloques);
    std::vector<std::string> dirRegistros;

    int n;
    if(_n == "*") { n = 1000; }
    else{ n = stoi(_n); }

    int nroR =NroRelacion(_relacion);
    int contR = 0;

    std::string bloque, sector , lineaAUX;
    while (getline(dir, bloque)){
        std::stringstream b(bloque);
        for(int i=0; i<5; i++){getline(b, lineaAUX, '#');}
        if(lineaAUX == "") break;
        std::stringstream strR(lineaAUX);
        bool encontrado = false;
        while (getline(strR, lineaAUX, '/') && !encontrado){
            if(lineaAUX == std::to_string(nroR)){
                std::stringstream s(bloque);
                getline(s, lineaAUX, '_'); // Nos pasamos bloque
                while (getline(s, lineaAUX, '_')){
                    std::stringstream s(lineaAUX);
                    getline(s, lineaAUX, '#');
                    getline(s, lineaAUX, '#');
                    std::string dirS = lineaAUX;
                    getline(s, lineaAUX, '#');
                    std::stringstream strR(lineaAUX);
                    while (getline(strR, lineaAUX, '/')){
                        if(lineaAUX == std::to_string(nroR)){
                            dirRegistros.push_back(dirS);
                            //int antes = contR;
                            consultaS(std::to_string(nroR), dirS, contR, n, _criterio);
                            //for(int j=1; j<contR-antes; j++){dirRegistros.push_back(dirS);}
                            dirRegistros.push_back(dirS);
                            if(contR >= n){
                                std::cout<<" REGISTROS EN >\n";
                                for(int k=0; k<dirRegistros.size(); k++){
                                    std::cout<<dirRegistros[k]<<"\n";
                                }
                                return;
                            }
                        }
                    }   
                }
                encontrado = true;
            }
        }
    }
    std::cout<<" REGISTROS EN >\n";
    for(int k=0; k<dirRegistros.size(); k++){
        std::cout<<dirRegistros[k]<<"\n";
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

#endif 