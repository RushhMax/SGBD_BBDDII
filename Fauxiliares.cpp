#include <iostream>
#include <string>
#include <vector>
#include <sstream> // para mis stringsteam
#include <fstream>
#include <algorithm> // Para std::remove_if
#include <cctype>    // Para std::isspace

using namespace std;

////////////////////////// BLOQUE //////////////////////////////7
// Funcion que indicar como esta compuesto bloque
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
void imprimirArchivo(string _string){
    cout<<"\nImprimiendo archivo . . . \n\n";
    std::ifstream archivo(_string);
    archivo.seekg(0);
    std::string cap = "";
    while(getline(archivo, cap)){
        cout<<cap;
    }
    cout<<endl;
    archivo.close();
}
// retorna PATH SECTOR desde  1/1/1/1
string getdirSector(string dir){
    std::stringstream ss(dir);
    std::vector<std::string> ubi;
    std::string token = "";
    while(std::getline(ss, token, '/')){ ubi.push_back(token);}
    
    std::string dirSector = "DISCO/Plato" + ubi[0] + "/S" + ubi[1] + "/Pista" + ubi[2] + "/Sector" + ubi[3] + ".txt";
    return dirSector;
}

string getdirBloque(int _nBloque){
    std::string dir = "DISCO/BLOQUES/Bloque" + std::to_string(_nBloque) + ".txt"; 
    return dir;
}
string getdirPage(int _nPage){
    std::string dir = "BUFFERPOOL/Page" + to_string(_nPage) + ".txt";
    return dir;
}

// DEVUELVE LA CAPACIDAD LIBRE DE LA PAGINA
int getcapacLibrePagina(int idPage){
    std::ifstream pagina(getdirPage(idPage));
    std::string cap = "";
    getline(pagina, cap, '#');
    pagina.close();
    return stoi(cap);
}
int getCapacPagina(int idPage){
    std::ifstream pagina(getdirPage(idPage));
    std::string cap = "";
    getline(pagina, cap, '#');
    getline(pagina, cap, '#');
    getline(pagina, cap, '#');
    getline(pagina, cap, '#');
    getline(pagina, cap, '#');
    pagina.close();
    return stoi(cap);
}
int getTipoPagina(int idPage){
    std::ifstream pagina(getdirPage(idPage));
    std::string tipo = "";
    getline(pagina, tipo, '#');
    getline(pagina, tipo, '#');
    pagina.close();
    return stoi(tipo);
}
// FUNCION PUNTUAL Adiciona registro en un archvo 
void adicionarRLFArchivo(std::string _registro, string _archivo){
    std::ofstream archiv(_archivo, std::ios::app);
    archiv<<_registro;
    archiv.close();
}   
void editarSLOTS(int idPage, int slot){
    std::fstream archiv(getdirPage(idPage), std::ios::in | std::ios::out);
    string lineaAux = "";
    string nuevaLinea = "";
    getline(archiv, lineaAux); // HEADER
    getline(archiv, lineaAux); // SLOTS
    stringstream slots(lineaAux);
    while (getline(slots, lineaAux, '|')){
        if(lineaAux == "_") { nuevaLinea += to_string(slot) + "|_|"; break; } 
        nuevaLinea += lineaAux + "|"; 
    }
    archiv.seekg(0);
    std::ostringstream nuevo_contenido;
    getline(archiv, lineaAux);
    nuevo_contenido<<lineaAux;
    getline(archiv, lineaAux);;
    nuevo_contenido<<endl<<nuevaLinea<<endl;
    getline(archiv, lineaAux);
    nuevo_contenido<<lineaAux;
    archiv.close(); 
    archiv.open(getdirPage(idPage), std::ios::out | std::ios::trunc);
    archiv<<nuevo_contenido.str();
    archiv.close();
}
bool esNumero(const std::string& str) {
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}
void adicionarRLVArchivo(std::string _registro, int idPage){
    imprimirArchivo(getdirPage(idPage));
    std::fstream archiv(getdirPage(idPage), std::ios::in | std::ios::out );
    archiv.seekg(0);
    archiv.seekp(0);

    std::string lineaAux;

    // Leer el HEADER
    std::getline(archiv, lineaAux);
    std::cout << "HEADER: " << lineaAux << std::endl;

    // Leer SLOTS
    std::getline(archiv, lineaAux);
    std::cout << "SLOTS: " << lineaAux << std::endl;

    std::stringstream slots(lineaAux);
    int contSlots = 0;
    int slot = getCapacPagina(idPage);

    while (std::getline(slots, lineaAux, '|')) {
        if (lineaAux == "_") {
            break;
        }
        if (esNumero(lineaAux)) {
            contSlots++;
            slot = std::stoi(lineaAux);
        } else {
            std::cerr << "Valor inválido en los slots: " << lineaAux << std::endl;
        }
    }
    slot -= _registro.size();


    archiv.seekg(0, std::ios::end);
    std::streampos length = archiv.tellg();
    archiv.seekg(0);

    if (length < slot) {
        archiv.seekp(0, std::ios::end);
        for (std::streamoff i = length; i < slot; ++i) {
            archiv.put('_');
        }
    }
    archiv.seekp(slot, std::ios::beg);
    archiv << _registro;
    archiv.close();
    //editarSLOTS(idPage, slot);
}
// FUNCION EXTRA  DE ADIC_RELACION(1/2) 
char is_IntFlo(std::string _dato){ //T ENTERO F FLOAT S STRING;
    for(int i=0; i<_dato.size();i++){
        if(_dato[i] == '.') return 'F';
        if(!isdigit(_dato[i])) return 'S';
    }
    return 'T';
}

////////////////////////// BLOQUE //////////////////////////////7

// FUNCION DEVUELVE NRO DE RELACION (2/2)
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
string getEsquema(string _relacion){
    ifstream relaciones("diccionario.txt");
    string _esquema;
    for(int i=0; i<NroRelacion(_relacion); i++){
        getline(relaciones, _esquema);
    }
    relaciones.close();
    return _esquema;
}
// Funcion devuelve cap max del registro
int capacMaxRegistro(std::string _archivo){
    std::ifstream archivo(_archivo);
    std::string regis;
    int max = 0;
    getline(archivo, regis);
    while(getline(archivo, regis)){
        if(regis.size()> max){ /*std::cout<<">"<<regis<<">"<<regis.size()<<"-"<<max<<"\n"; */max = regis.size();}
    }
    //std::cout<<" EL MAX ES "<<max<<std::endl;
    return max;
}

////////////////////////// RLV //////////////////////////////7

// Funcion para crear un registro de longitud variable
std::string crearRLV(std::string _registro, std::string _relacion){
    stringstream esquema(getEsquema(_relacion));
    string atributo;
    getline(esquema, atributo, '#'); // nombre de la relacion
    std::string RN = "", aux, mapa=""; // 2 strings RN y mapa
    int desplazamiento = 0;
    std::stringstream R(_registro); // return _registro
    while(getline(R, aux, ';')){ // Leemos registros separados por ;
        for(int i=0; i<2; i++)getline(esquema, atributo, '#'); // nombre atributo
        if(atributo == "varchar") { 
            mapa += std::to_string(desplazamiento) + "|";
            getline(esquema, atributo, '#'); 
        }
        else { 
            getline(esquema, atributo, '#');  // tamaño
            if (stoi(atributo) > aux.size()) {
                aux.append(stoi(atributo) - aux.size(), ' ');
            }
        }
        desplazamiento += aux.size();  
        RN += aux; 
    }
    mapa += std::to_string(desplazamiento) + "|";

    mapa.push_back('#');
    RN = mapa + RN; // UNIMOS nuestro mapa con Registro
    return RN;
}

////////////////////////// RLF //////////////////////////////7

std::vector<int> longMaxEsquema(string _relacion) {
    std::vector<int> longMax; 
    std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
    std::string linea = "";
    bool relacionEncontrada = false;
    while (std::getline(diccionario, linea) && !relacionEncontrada ){
        std::stringstream lin(linea); // de nuestro diccionario buscamos la tabla y el #4 y #7  #10 etc (vector) 
        std::getline(lin, linea, '#');
        if (linea == _relacion){ // if linea es igual a la relacion que estamos buscando
            int n=1; //Tabla1#ID#int#4#Name#String#10#Edad#int#2
            while(std::getline(lin, linea, '#')){
                std::stringstream longAtr(linea);
                //getline(longAtr, linea, '_');
                if(n%3 == 0){ longMax.push_back(std::stoi(linea));}
                n++;
            }
            relacionEncontrada = true;
        }
    }
    diccionario.close();
    if(!relacionEncontrada){std::cout<<" RELACION NO ENCONTRADA! "; return longMax;}
    return longMax;
} 
// Funcion para crear un registro de longitud fija de acuerdo a su relcion
std::string crearRLF(std::string _registro, std::string _relacion){
    std::vector<int> longMax = longMaxEsquema(_relacion);
    if(longMax.size() == 0)  {std::cout<<" RELACION NO ENCONTRADA! "; return "";}

    std::string _atributo = "";
    // LONGITUD FIJA 
    std::string registroN = ""; // registro nuevo de LONG FIJA
    int it = 0, _longRA=0;
    std::stringstream str(_registro);
    while (std::getline(str, _atributo, ';')){ // leer registro por atributos ";" 
        registroN += _atributo;
        if(_atributo.size() < longMax[it]) {// contamos la long del registro 
            // funcion para agregar espacios
            if (longMax[it] > _atributo.size()) {
                registroN.append(longMax[it] - _atributo.size(), ' ');
            }
            //for(int i=0; i< longMax[it]-_atributo.size(); i++){ registroN += " "; }
        } // else{  for(int j=0; j<_atributo.size() - longMax[it] ;j++){ registroN.pop_back(); } }
        it++;
    }
    //std::cout<<"Registro Nuevo de LONG FIJA >"<<registroN<<">";
    return registroN;
}

// EDITA EL DIRECTORIO O CABEZALES DE ARCHIVOS
void editarCabeceras(int Linea, int Posicion, std::string _nRelacion,std::string _capacLibre, int _tipoB, std::string _archivo){
    //(1, 0, _nroR ,  std::to_string(espacLibrePage), tipoR ,getdirPage(idPage))
    std::fstream archivo(_archivo, std::ios::binary | std::ios::in | std::ios::out);
    std::string nuevaLinea = "", lineaAUX = "";

    // OBTENEMOS la linea a editar
    for(int i=0; i<Linea; i++){ getline(archivo, lineaAUX);}  
    std::stringstream str(lineaAUX);
    for(int i=0; i<Posicion; i++){ getline(str, lineaAUX, '_'); nuevaLinea += lineaAUX + '_';}

    getline(str, lineaAUX, '#'); // nos pasamos el dato que quiero borrar
    nuevaLinea += _capacLibre + "#"; // agregarmos el dato nuevo

    if(Posicion == 0) {
        getline(str, lineaAUX, '#');
        nuevaLinea += to_string(_tipoB) + "#";
        for(int i=0; i<3;i++){
            getline(str, lineaAUX, '#');
            nuevaLinea += lineaAUX + "#";
        }
    }else{
        getline(str, lineaAUX, '#');
        nuevaLinea += lineaAUX + "#"; 
    }
    
    getline(str, lineaAUX, '#'); // ADQUIERO RELACIONES
    std::stringstream relaciones(lineaAUX);

    std::string r; bool encontrado = false;
    if ( _nRelacion == "|" ) {
        nuevaLinea += lineaAUX + "#";
        encontrado = true;
    }
    while (getline(relaciones, r, '/')  && !encontrado){
        if(r == _nRelacion ){
            nuevaLinea += lineaAUX + "#";
            encontrado = true;
            break;
        }
    }
    if(!encontrado) nuevaLinea += lineaAUX + '/' + _nRelacion + "#";
    
    // AGREGAMOS EL RESTO
    getline(str, lineaAUX);
    nuevaLinea += lineaAUX;
    //nuevaLinea.pop_back(); // eliminamos salto de linea

    // 2 PARTE / REESCRIBIMOS EL ARCHIVO
    archivo.seekg(0);
    std::ostringstream nuevo_contenido;
    int BloqueActual = 0; lineaAUX = "";
    // Leer el archivo y escribir en un stringstream todas las líneas excepto la línea a eliminar
    while (getline(archivo, lineaAUX)) {
        BloqueActual++;
        // Si no es la línea que queremos eliminar, la añadimos al nuevo contenido
        if (BloqueActual != Linea) { nuevo_contenido << lineaAUX;}
        else{ nuevo_contenido << nuevaLinea;}
    }
    //nuevo_contenido.str().pop_back();
    archivo.close(); 
    archivo.open(_archivo, std::ios::out | std::ios::trunc);
    archivo<<nuevo_contenido.str();
    archivo.close();
}

// FUNCION PARA ADICIONAR UN REGISTRO EN UNA TABLA ESPECIFICA
bool adicionarRegistroP(int idPage, std::string _registro, std::string _relacion, bool tipoR){
    // CREAMOS RLV O RLF
    std::string R = ""; 
    if(tipoR) {R +=  crearRLF(_registro, _relacion);  }
    else{ R += crearRLV(_registro, _relacion); } // REGISTRO LONGITUD VARIABLE

    int espacLibrePage = getcapacLibrePagina(idPage); 
    
    // este codigo no respeta el espac sectores quiere decir un registro puede partirse
    if(R.size() <= espacLibrePage && (tipoR == getTipoPagina(idPage) || getTipoPagina(idPage) == 2)){  // y si es del mismo tipo de R
        espacLibrePage -= R.size();
        std::string _nroR = std::to_string(NroRelacion(_relacion));
        if(tipoR) {
            adicionarRLFArchivo
            (R, getdirPage(idPage));
            editarCabeceras(1, 0, _nroR ,  std::to_string(espacLibrePage), tipoR ,getdirPage(idPage));
        }else{
            if(getTipoPagina(idPage) == 2){
                cout<<" REGISTRO EN PAGINA TIPO 2 ACTUALIZANDO!";
                adicionarRLFArchivo("_|\n", getdirPage(idPage));
            }
            adicionarRLFArchivo(R, getdirPage(idPage));
            editarCabeceras(1, 0, _nroR ,  std::to_string(espacLibrePage), tipoR ,getdirPage(idPage)); 
            //editarSLOTS(idPage, R.size());
        }
        return true;
    }else 
        return false;
}

bool cumpleCondicion(string _registro, string _condicion, string _relacion, bool _tipoR){
    std::cout<<" EN FUUNCION CUMPLE ! con registro <<"<<_registro<<"> RL>"<<_tipoR<<"\n";
    // if tipoR = 0 RLV
    // if tipoR = 1 RLF
    
    // DESGLOSAMOS nuestra condicion
    std::stringstream cond(_condicion); 
    std::vector<std::string> condicion; string token;
    getline(cond, token, ' ' ); condicion.push_back(token);
    getline(cond, token, ' ' ); condicion.push_back(token);
    getline(cond, token); condicion.push_back(token);

    string atributoCondicion;
    
    stringstream esquema(getEsquema(_relacion));
    //std::cout<<" Esquema de la relacion "<<esquema<<endl;
    string atributo; 
    int n_atributo = 1, n = 0;
    int posRLF = 0;  int longitudACTUAL;
    getline(esquema, atributo, '#'); // SE PASA NOMBRE DE LA RELACION
    while (getline(esquema, atributo, '#')){
        n++;
        //Tabla1# ID#int#4#Name#String#10#Edad#int#2
        //std::cout<<" A> "<<atributo<<" = "<<condicion[0]<<"? "<<n_atributo<<"-"<<posRLF<<" \n";
        if(atributo == condicion[0]){
            if(_tipoR == 1){ // RLF
                getline(esquema, atributo, '#');
                getline(esquema, atributo, '#');
                stringstream longA(atributo);
                string longAtributo;
                getline(longA, longAtributo, '_');
                longitudACTUAL = stoi(longAtributo);
            }
            break;
        }
        if(n % 3 == 0 ) { 
            //std::cout<<" en A> "<<atributo<<endl;
            n_atributo++;
            if(_tipoR == 1){// RLF
                stringstream longA(atributo);
                string longAtributo;
                getline(longA, longAtributo, '_');
                int longitud = stoi(longAtributo);
                posRLF += longitud + 1;
            }
        }
    }

    //std::cout<<" Posicion del tributo de RLF >"<<posRLF<<"> y nro Atributo "<<n_atributo<<endl;
    //std::cout<<" REGISTRO "<<_registro<<">"<<_registro.substr(posRLF, longitudACTUAL)<<"\n";

    if(_tipoR == 1){// RLF
        atributoCondicion = _registro.substr(posRLF, longitudACTUAL);
        size_t endpos = atributoCondicion.find_last_not_of(" \t\n\r\f\v");
        if (std::string::npos != endpos) { atributoCondicion.erase(endpos + 1); }// Borra desde el final hasta el primer carácter que no es un espacio
    }else{
        // 0|1|2|3|26|30|32|33|34|43|47|47#103Braund, Mr. Owen Harrismale2210A/5 211717.25S
        string mapaBits, registroLV;
        std::stringstream registro(_registro); 
        getline(registro, mapaBits, '#');
        stringstream mapa(mapaBits); string posRLV, posRLV2;
        getline(registro, registroLV);
        for(int i=0;i<n_atributo; i++){
            getline(mapa, posRLV, '|');
        }
        getline(mapa, posRLV2, '|');

        longitudACTUAL = stoi(posRLV2) - stoi(posRLV);
        atributoCondicion = registroLV.substr(stoi(posRLV), longitudACTUAL);
    }

    std::cout<<" ATRIBUTO >"<<atributoCondicion<<">< condicion >"<<condicion[2]<<">\n";

    if(condicion[1] == "="){ if(atributoCondicion == condicion[2] ) return true;}
    //else if(condicion[1] == ">="){ if(r >= condicion[2] ) critCUM = true;}
    //else if(condicion[1] == "<="){ if(r <= condicion[2] ) critCUM = true;}
    //else if(condicion[1] == "<"){ if(r < condicion[2] ) critCUM = true;}
    //else if(condicion[1] == ">"){ if(r >condicion[2] ) critCUM = true;}
   
    return false;
}

void eliminarNLineaPage(int _idPage, int _nRegistro){
    fstream _Page("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::binary | std::ios::in | std::ios::out);
    std::ostringstream nuevo_contenido;
    string lineaAUX;
    int contLinea = 0;
    while (getline(_Page, lineaAUX)) {
        contLinea++;
        lineaAUX.pop_back();
        if (contLinea != _nRegistro) {
            nuevo_contenido << lineaAUX <<endl;
        }
    }
    _Page.close(); 
    _Page.open("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::out | std::ios::trunc);
    _Page<<nuevo_contenido.str();
    _Page.close();
}

// FUNCION PARA ELIMINAR UN REGISTRO DE LA PAGINA
void eliminarRegistroPage(int _idPage, string _relacion, string _condicion){
    std::cout<<" EN FUNCION ELIMINAR !\n";
    fstream pagina("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::in | std::ios::out);
    pagina.seekg(0);
    if (!pagina) { std::cerr << "Error al abrir el archivo! "<< std::endl; return;}
    
    int nroRelac = NroRelacion(_relacion);
    string registro = "";
    int nroregistro = 1;
    // CABECERA
    getline(pagina, registro);
    stringstream cabecera(registro);
    string espacioPage; 
    getline(cabecera, espacioPage, '#'); // CAPACIDAD
    int espacioPageINT = stoi(espacioPage);

    while(getline(pagina, registro)){ // ITERAR CADA REGISTRO 
        std::cout<<" REGISTRO >"<< registro<<">\n" ;
        nroregistro++;
        stringstream reg(registro);
        getline(reg, registro, '#');
        if(stoi(registro) == nroRelac){ //SI ES DE LA RELACION
            
            getline(reg, registro, '#'); // NOS SALTAMOS SI ES DE RLF O RLV
            int _tipoR = stoi(registro);

            getline(reg, registro);
            // SI CUMPLE CONDICION
            if(cumpleCondicion(registro, _condicion, _relacion, _tipoR)){
                std::cout<<"\n CUMPLE CONDICION! registro <<"<<registro.size()<<">  \n"; 
                eliminarNLineaPage(_idPage, nroregistro);
                nroregistro--;
                espacioPageINT += registro.size();
                //std::cout<<" ESPACIO "<<espacioPageINT<<endl;
                //editarCabeceras(1, 0, std::to_string(nroRelac) , std::to_string(espacioPageINT),  "BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt");
            }

        }
    }
    pagina.close();
}

int GetnroAtributo(string _atributo, int _tipoR, string _relacion){
    stringstream esquema(getEsquema(_relacion));
    
    string atributo; 
    int n_atributo = 1, n = 0;
    getline(esquema, atributo, '#'); // SE PASA NOMBRE DE LA RELACION
    while (getline(esquema, atributo, '#')){
        n++;
        //Tabla1# ID#int#4#Name#String#10#Edad#int#2
        //std::cout<<" A> "<<atributo<<" = "<<condicion[0]<<"? "<<n_atributo<<"-"<<posRLF<<" \n";
        if(atributo == _atributo){
            break;
        }
        if(n % 3 == 0 ) { 
            //std::cout<<" en A> "<<atributo<<endl;
            n_atributo++;
        }
    }
    return n_atributo;
    
}

void modificarNRegistroPage(int _idPage, int _nroRegistro, string _relacion, string _atributo, int _tipoR){
    fstream _Page("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::binary | std::ios::in | std::ios::out);
    
    // DESGLOSAMOS nuestra condicion
    std::stringstream atributostr(_atributo); 
    std::vector<std::string> atributoV; string token;
    getline(atributostr, token, ' ' ); atributoV.push_back(token);
    getline(atributostr, token, ' ' ); atributoV.push_back(token);
    getline(atributostr, token); atributoV.push_back(token);

    //stringstream esquema(getEsquema(_relacion));
    int n_atributo = GetnroAtributo(atributoV[0], _tipoR, _relacion);
    std::cout<<" Atributo a cambiar > "<<n_atributo<<endl;
    

    string nuevoRegistro = "";
    std::ostringstream nuevo_contenido;
    string lineaAUX;
    int contLinea = 0;
    while (getline(_Page, lineaAUX)) {
        contLinea++;
        lineaAUX.pop_back();
        if (contLinea != _nroRegistro) {
            nuevo_contenido << lineaAUX <<endl;
        }else{
            // LEER REGISTRO
            
            if(_tipoR == 1){ // FIJA
                //stringstream registro(lineaAUX);

                vector<int> longitudes = longMaxEsquema(_relacion);
                for(int i=0; i<longitudes.size(); i++) std::cout<<" LONGITUDES "<<longitudes[i]<<endl;
                int desplazamiento  = 4;
                std::cout<<lineaAUX<<endl;
                for(int i=0; i< longitudes.size(); i++){
                    std::cout<<" i >"<<i+1<<"> Atributo a cambiar >"<<n_atributo<<endl;
                    if (n_atributo == i+1) { 
                        nuevoRegistro += atributoV[2] + ";";
                        desplazamiento +=  longitudes[i]+1;

                    } else {  //1#1#2  1 1 Cumings, Mrs. John Bradley (Florence Briggs Thayer)       female 38 1 0 PC 17599             71.2833 C85  C
                        // REGISTROS VACIOS ATRIBUTOS
                        string atributoSinEspacios = lineaAUX.substr(desplazamiento, longitudes[i]+1);
                        std::cout<<"ATRIBUTO >"<<atributoSinEspacios<<endl;
                        // eliminar espacios des = 
                        size_t endpos = atributoSinEspacios.find_last_not_of(" \t\n\r\f\v");
                        desplazamiento +=  longitudes[i]+1;
                        if (std::string::npos != endpos) { atributoSinEspacios.erase(endpos + 1); }// Borra desde el final hasta el primer carácter que no es un espacio
                        std::cout<<"ATRIBUTO SE >"<<atributoSinEspacios<<endl;
                        nuevoRegistro += atributoSinEspacios + ";";
                    }
                    std::cout<<nuevoRegistro<<endl;
                }
                nuevoRegistro = crearRLF(nuevoRegistro, _relacion);
                nuevoRegistro = lineaAUX.substr(0,4) + nuevoRegistro;
            } 
            else{ // VARIABLE
                nuevoRegistro = crearRLV(nuevoRegistro, _relacion);
            }

            //std::cout<<" Nueva linea es "<<nuevalinea<<">\n";
            
            nuevo_contenido << nuevoRegistro << endl;
            // MODIFICAR ESTA LINEA;
        }
    }
    _Page.close(); 
    _Page.open("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::out | std::ios::trunc);
    _Page<<nuevo_contenido.str();
    _Page.close();
}

void modificarRegistroPage(int _idPage, string _relacion, string _atributo, string _condicion){
    std::cout<<" EN FUUNCION MODIFICAR !\n";
    fstream pagina("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::in | std::ios::out);
    pagina.seekg(0);
    if (!pagina) { std::cerr << "Error al abrir el archivo! "<< std::endl; return;}
    
    int nroRelac = NroRelacion(_relacion);
    string registro = "";
    int nroregistro = 1;
    // CABECERA
    getline(pagina, registro);
    stringstream cabecera(registro);
    string espacioPage; 
    getline(cabecera, espacioPage, '#'); // CAPACIDAD
    int espacioPageINT = stoi(espacioPage);

    while(getline(pagina, registro)){ // ITERAR CADA REGISTRO 
        std::cout<<" REGISTRO >"<< registro<<">\n" ;
        nroregistro++;
        stringstream reg(registro);
        getline(reg, registro, '#');
        if(stoi(registro) == nroRelac){ //SI ES DE LA RELACION
            
            getline(reg, registro, '#'); // NOS SALTAMOS SI ES DE RLF O RLV
            int _tipoR = stoi(registro);

            getline(reg, registro);
            // SI CUMPLE CONDICION
            if(cumpleCondicion(registro, _condicion, _relacion, _tipoR)){
                std::cout<<"\n CUMPLE CONDICION! registro <<"<<registro.size()<<">  \n"; 
                // EDITAR LINEA CON ATRIBUTO
                
                modificarNRegistroPage(_idPage, nroregistro,_relacion ,  _atributo, _tipoR);
                //espacioPageINT += registro.size();
                //std::cout<<" ESPACIO "<<espacioPageINT<<endl;
                //editarCabeceras(1, 0, std::to_string(nroRelac) , std::to_string(espacioPageINT),  "BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt");
            }

        }
    }
    pagina.close();
}


