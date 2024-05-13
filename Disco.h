#ifndef __DISCO_H__
#define __DISCO_H__

#include <fstream>
#include <direct.h>
#include <filesystem>
#include <sstream> // para mis stringsteam
#include "Bloque.h"

int capacMaxRegistro(std::string _archivo);
int capacRegistro(std::string _registro);
char is_IntFlo(std::string _dato);
int NroRelacion(std::string _relacion);

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

        std::string crearRLF(std::string _registro, std::string _tabla, int _long);

        void escribirReg(std::string _registro, std::string _archivo);
        void editarDirectorio(int linea, int posicion, std::string _nRelacion, std::string Ndato, std::string _archivo);

        void adicionarReg(std::string _registro, std::string _tabla, bool _TipoR);
        void adicionarReg(int n, std::string _archivo, std::string _tabla);
        void adicionarCSV(std::string _archivo, std::string _tabla);


        void consulta(std::string n, std::string _relacion, std::string _criterio);
        void consultaS(std::string _relacion, std::string _dirS, int &_contR, int _n, std::string _criterio);
        void printDisco();
};

// CONSTRUCTOR DISCO DEFAULT // BIEN
Disco::Disco(){
    this->nombre = "DISCO";
    this->nroPlatos = 8;
    this->nroPistas = 50;  
    this->nroSectores = 10;
    this->capacidadS = 200;
    this->capacidadD = capacidadS * nroSectores * nroPistas * nroPlatos * 2;
    this->capaclibre = capacidadD; 
    this->capacBloque = 1600;
    this->nroBloques = this->capacidadD / this->capacBloque;

    crearEstructura();
    crearBloques();

    std::ofstream dicc(diccionario); // inicializando;
    dicc.close();
}

// CONSTRUCTOR DISCO POR PARAMETROS // BIEN
Disco::Disco(int _platos, int _pistas, int _sectores, long long int _capacSector, int _capacidadBloque){
    this->nombre = "DISCO";
    this->nroPlatos = _platos;
    this->nroPistas = _pistas;  
    this->nroSectores = _sectores;
    this->capacidadS = _capacSector;
    this->capacidadD = _capacSector * _sectores * _pistas * _platos * 2;
    this->capacBloque = _capacidadBloque;

    crearBloques();
    crearEstructura();

    std::ofstream direct(diccionario); // inicializando;
    direct.close();
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

// !!! POLITICA DE !!!
void Disco::crearBloques(){
    std::ofstream _dirBloques(directorioBloques);

    int nroSectoresMAX = capacidadD / capacidadS;
    int nroSectoresBLOQUE = capacBloque / capacidadS;

    int nPlato = 1, nS = 1 , nPista = 1, nSector = 1; 
    for(int i=0; i<nroBloques; i++){
        // {espacioLibre}#BLOQUE#1#{capacidadBloque}#SECTORES
        _dirBloques<<capacBloque<<"#BLOQUE#"<<i+1<<"#"<<capacBloque<<"##_";
        // SECTORES 
        //deben copiarse solo nroSECTORESBLOQUE 
        for(int j=0; j<nroSectoresBLOQUE; j++){
            // {espacioLibre}#DIRSECTOR
            _dirBloques<<capacidadS<<"#"<<nPlato<<"/"<<nS<<"/"<<nPista<<"/"<<nSector<<"##_";
            // FUNCION QUE CONTROLE CRECIEMIENTO DE VARIABLES 
            nSector++;
            if(nSector % this->nroSectores == 1) { nSector = 1; nPista++;}   
            if(nPista % this->nroPistas == 1 && nPista != 1) { nPista = 1; nS++; }
            if(nS % 2 == 1 && nS != 1) {nS = 1; nPlato++;}
            if(nPlato % nroPlatos == 1 && nPlato != 1) {nPlato = 1; }
        }
        _dirBloques<<std::endl;
    }
    //std::cout<<" Hay "<<nroBloques<<" con capacidad "<<capacBloque<<"\n";
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
            if(isalpha(tip_dato[0])) { _diccionario<<"#string#20"; } // si dato comienza con char -> string
            else if(is_IntFlo(tip_dato) == 'T'){ _diccionario<<"#int#4";} // si dato es entero
            else if(is_IntFlo(tip_dato) == 'F'){ _diccionario<<"#float#4";} // si dato es float
            else if(is_IntFlo(tip_dato) == 'S'){ _diccionario<<"#string#20"; } // si dato es string
        }
    }
    _diccionario<<std::endl;
    archivo.close();
}

// FUncion para adicionar un registro de long fija de acuerdo a su tabla
std::string Disco::crearRLF(std::string _registro, std::string _tabla, int _long){
    std::ifstream diccionario(this->diccionario);
    std::string _atributo = "", linea = "";
    
    //std::cout<<" TRANSFORMANDO >"<<_registro<<"> LONG FIJA de la RELACION >"<<_tabla<<">\n";

    std::vector<int> longMax; bool relacionEncontrada = false;
    //std::cout<<" LONGITUDES DE LA TABLA >\n";
    while (std::getline(diccionario, linea) && !relacionEncontrada ){
        std::stringstream lin(linea); // de nuestro diccionario buscamos la tabla y el #4 y #7  #10 etc (vector) 
        std::getline(lin, linea, '#');
            
        if (linea == _tabla){ // buscando su tabla
            int n=1; //Tabla1#ID#int#4#Name#String#10#Edad#int#2
            while(std::getline(lin, linea, '#')){
                if(n%3 == 0){ longMax.push_back(std::stoi(linea));std::cout<<std::stoi(linea)<<">";}
                n++;
            }
            relacionEncontrada = true;
        }
    }
    if(!relacionEncontrada){std::cout<<" RELACION NO ENCONTRADA! "; return "";}

    // LONGITUD FIJA 
    std::string registroN = ""; // registro de long fija
    std::stringstream str(_registro);
    int it = 0, _longRA=0;
    while (std::getline(str, _atributo, ';')){ // leer registro por atributos ";" 
        registroN += _atributo;
        if(_atributo.size() <= longMax[it]) {// contamos la long del registro 
            // contamos la long de cada atributo y asignamos " " necesarios para alcanzar el max nro de bytes
            for(int i=1; i< longMax[it] - _atributo.size(); i++){
                registroN += " ";
            }
            registroN += ";";
        }else{
            for(int j=0; j<_atributo.size() - longMax[it] ;j++){ registroN.pop_back(); }
            registroN.pop_back();
            registroN += ";";
            std::cout<<" DATO DEMASIADO GRANDE PARA LONG ATRIBUTO !! "<<std::endl;
        }
        it++;
    }


    //std::cout<<"Registro >"<<registroN.size()<<" MAX "<<_long<<"\n";
    if(_long != 0){
        if(registroN.size() < _long){
            for(int i=0; i<_long-registroN.size(); i++){
                registroN += " ";
            }
        }
        else if(registroN.size() > _long){
            int itera = registroN.size()-_long;
        for(int j=0; j<itera; j++){
                std::cout<<">"<<j<<">";
                registroN.pop_back();
            }    
            registroN.pop_back();
            registroN += ";";
        }
    }

    //std::cout<<"Registro Nuevo de LONG FIJA >"<<registroN<<">";
    diccionario.close();
    return registroN;
}

// FUNCION PUNTUAL REGISTRO EN SECTOR.TXT
void Disco::escribirReg(std::string _registro, std::string _archivo){
    std::stringstream ss(_archivo);
    std::vector<std::string> ubi;
    std::string token = "";
    while(std::getline(ss, token, '/')){ ubi.push_back(token);}
    
    std::string dirSector = "DISCO/Plato" + ubi[0] + "/S" + ubi[1] + "/Pista" + ubi[2] + "/Sector" + ubi[3] + ".txt";

    std::ofstream sector(dirSector, std::ios::app);
    sector<<_registro<<"\n";
    sector.close();
}

void Disco::editarDirectorio(int _Bloque, int _Sector, std::string _nRelacion,std::string Ndato, std::string _archivo){
    std::fstream archivoL(_archivo, std::ios::binary | std::ios::in | std::ios::out);
    std::string Contlinea = "", nuevaLinea = "", lineaAUX = "";

    // OBTENEMOS BLOQUE
    for(int i=0; i<_Bloque; i++){ getline(archivoL, Contlinea);}  
    //1 nuevaLinea = Contlinea.substr(0, posicion); // copiando antes de la pos
    std::stringstream str(Contlinea);
    for(int i=0; i<_Sector; i++){getline(str, lineaAUX, '_'); nuevaLinea += lineaAUX + '_';}
    
    //std::cout<<">"<<nuevaLinea<<">\n";

    getline(str, lineaAUX, '#'); // nos pasamos el registro que quiero borrar
    //std::cout<<" Borrando registro "<<lineaAUX+"#\n";

    //std::cout<<">"<<nuevaLinea<<" + "<<Ndato<<"# + "<<lineaAUX<<"\n";

    nuevaLinea += Ndato + "#";
    if(_Sector == 0) {
        for(int i=0; i<3;i++){
            getline(str, lineaAUX, '#');
            nuevaLinea += lineaAUX + "#";
        }
    }else{
        getline(str, lineaAUX, '#');
        nuevaLinea += lineaAUX + "#"; //
    }
     // ME paso la direccion
    //nuevaLinea += lineaAUX + "#"; // agrego la direccion
    
    
    getline(str, lineaAUX, '#'); // ADQUIERO RELACIONES
    std::stringstream relaciones(lineaAUX);
    std::string r; bool encontrado = false;
    while (getline(relaciones, r, '/')  && !encontrado){
        if(r == _nRelacion){
            nuevaLinea += lineaAUX + "#";
            encontrado = true;
            break;
        }
    }
    if(!encontrado) nuevaLinea += lineaAUX + '/' + _nRelacion + "#";
    
    // AGREGAMOS EL RESTO
    getline(str, lineaAUX);
    nuevaLinea += lineaAUX;

    nuevaLinea.pop_back();
    //std::cout<<">"<<nuevaLinea<<">\n";


    archivoL.seekg(0);
    std::ostringstream nuevo_contenido;
    int BloqueActual = 0;
    // Leer el archivo y escribir en un stringstream todas las líneas excepto la línea a eliminar
    while (getline(archivoL, lineaAUX)) {
        BloqueActual++;
        lineaAUX.pop_back();

        // Si no es la línea que queremos eliminar, la añadimos al nuevo contenido
        if (BloqueActual != _Bloque) {
            nuevo_contenido << lineaAUX << std::endl;
        }else{
            nuevo_contenido << nuevaLinea << std::endl;
        }
    }

    archivoL.close(); 
    archivoL.open(_archivo, std::ios::out | std::ios::trunc);
    archivoL<<nuevo_contenido.str();
    archivoL.close();
}

// FUNCION PARA ADICIONAR UN REGISTRO EN UNA TABLA ESPECIFICA
void Disco::adicionarReg(std::string _registro, std::string _relacion, bool _TipoR){
    std::ifstream bloques(directorioBloques); // abrimos directorio
    
    // variables que nos serviran
    std::string lineaBloque = "", lineaSector ="", dato = ""; char delimitador = '_'; 
    int NBloque = 0, NSector = 0;
    int espacLibreINTB = 0, espacLibreINTS = 0;
    int nroSectBLOQUE = capacBloque / capacidadS; // NRO DE SECTORES EN BLOQUE
    
    std::string _nroR = std::to_string(NroRelacion(_relacion));
    std::string R = ""; 
    if(_TipoR){ R +=  crearRLF(_registro, _relacion, 0);  } // 1  REGISTRO DE LONGITUD FIJA
    else{ R += _registro; } // REGISTRO LONGITUD VARIABLE
    
    capaclibre -= R.size(); 

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

                    //std::cout<<"Bloque>"<<NBloque<<">BLOQUE>"<<espacLibreINTB<<">\n";
                    //std::cout<<"Sector>"<<NSector<<">SECTOR>"<<espacLibreINTS<<">\n"; 
                    editarDirectorio(NBloque, 0, _nroR ,  std::to_string(espacLibreINTB), directorioBloques);
                    editarDirectorio(NBloque, NSector, _nroR ,std::to_string(espacLibreINTS), directorioBloques);
                    
                    // AGREGAMOS 
                    R = _nroR + "#" + R;
                    escribirReg(R, dato); // ESCRIBIMOS

                    //std::cout<<"> Insertando >"<<R<<"> de tamano "<<R.size()<<" en "<<dato<<std::endl;
                    std::cout<<" INSERTADO CON EXITO EN >"<<dato<<" \n";
                    bloques.close();
                    return;
                }
            }
            //std::cout<<" SECTORES DE ESTE BLOQUE OCUPADOS NEXT! \n";
        }
    }
    std::cout<<" TODOS LOS BLOQUES OCUPADOS! Lo siento! \n"; 
    bloques.close();
}

// FUNCION ADICIONAR N REGISTROS DE UN CSV
void Disco::adicionarReg(int n, std::string _archivo, std::string _tabla){
    std::ifstream data(_archivo);
    std::string registro;
    std::getline(data, registro); // saltandonos encabezado
    for(int i=0; i<n; i++){
        std::getline(data, registro);
        adicionarReg(registro, _tabla, 0);
    }
    data.close();
}

// FUNCION PARA ADICIONAR TODOS LOS REGISTROS DE UN CSV.
void Disco::adicionarCSV(std::string _archivo, std::string _tabla){
    std::ifstream data(_archivo);
    std::string registro;
    std::getline(data, registro); // saltandonos encabezado
    while(std::getline(data, registro)){
        adicionarReg(registro, _tabla, 0);
    }
    data.close(); 
}

void Disco::consultaS(std::string _relacion, std::string _dirS, int &_contR, int _n, std::string _criterio){
    std::stringstream ss(_dirS); std::stringstream cr(_criterio);
    std::vector<std::string> ubi, criterio;
    std::string token = "";
    while(std::getline(ss, token, '/')){ ubi.push_back(token);}
    if(_criterio != " ") while(std::getline(cr, token, ' ' )){ criterio.push_back(token);  }

    //if(_criterio != " ") std::cout<<"Desarmando criterio "<<criterio[0]<<" "<<criterio[1]<<" "<<criterio[2]<<"\n";

    int nAtributo = 0;
    if(_criterio != " "){
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
        std::stringstream STR(_registro);
        std::string r;
        getline(STR, r, '#');
        if(_relacion == r){
            if(_criterio == " "){
                _registro = _registro.substr(2);
                std::cout<<_registro<<"\n";
                _contR++;
                if(_n == _contR){sector.close(); return;} 
            }else{
                bool critCUM = false;
                while(getline(STR, r, ';')){
                    //std::cout<<" nAtributo >"<<nAtributo<<" CONTAT "<<contAT;    
                    if(contAT == nAtributo){
                        //std::cout<<" COMPARANDO >"<<r<<"> con >"<<criterio[2]<<">\n";

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

// FUNCION EXTRA  DE ADIC_RELACION(1/1) 
char is_IntFlo(std::string _dato){ //T ENTERO F FLOAT S STRING;
    for(int i=0; i<_dato.size();i++){
        if(_dato[i] == '.') return 'F';
        if(!isdigit(_dato[i])) return 'S';
    }
    return 'T';
}

int NroRelacion(std::string _relacion){
    std::ifstream _diccionario("diccionario.txt");
    std::string _r; int i=0;
    while (getline(_diccionario, _r)){
        i++;
        std::stringstream strR(_r);
        getline(strR, _r, '#');
        if(_relacion == _r){
            return i;
        }
    }
    return 0; 
}

#endif 