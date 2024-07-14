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
// FUNCION EXTRA  DE ADIC_RELACION(1/2) 
char is_IntFlo(std::string _dato){ //T ENTERO F FLOAT S STRING;
    for(int i=0; i<_dato.size();i++){
        if(_dato[i] == '.') return 'F';
        if(!isdigit(_dato[i])) return 'S';
    }
    return 'T';
}
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
    //archivo.seekg(0);
    std::string cap = "";
    while(getline(archivo, cap)){
        cout<<cap<<endl;
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
// EDITA EL DIRECTORIO O CABEZALES DE ARCHIVOS
void editarCabeceras(int Linea, int Posicion, std::string _nRelacion,std::string _capacLibre, int _tipoB, std::string _archivo){
    //(1, 0, _nroR ,  std::to_string(espacLibrePage), tipoR ,getdirPage(idPage))
    std::fstream archivo(_archivo, std::ios::in | std::ios::out);
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
        if (BloqueActual != Linea) { nuevo_contenido << lineaAUX<<endl; }
        else{ nuevo_contenido << nuevaLinea<<endl;}
    }
    //nuevo_contenido.str().pop_back();
    archivo.close(); 
    archivo.open(_archivo, std::ios::out | std::ios::trunc);
    archivo<<nuevo_contenido.str();
    archivo.close();
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
        if(regis.size()> max){ /*std::cout<<">"<<regis<<">"<<regis.size()<<"-"<<max<<"\n"; */
            max = regis.size();
        }
    }
    //std::cout<<" EL MAX ES "<<max<<std::endl;
    return max;
}
int getCapacMaxRegistro(vector<pair<string,int>> _longMaxEsquema){
    int capac = 0;
    for(int i=0; i<_longMaxEsquema.size();i++){
        capac += _longMaxEsquema[i].second;
    }
    return capac;
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
////////////////////////// RLF //////////////////////////////7
std::vector<pair<string,int>> longMaxEsquema(string _relacion) {
    std::vector<pair<string,int>> longMax; 
    std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
    std::string linea = "";
    bool relacionEncontrada = false;
    string tipoAtributo;
    while (std::getline(diccionario, linea) && !relacionEncontrada ){
        std::stringstream lin(linea); // de nuestro diccionario buscamos la tabla y el #4 y #7  #10 etc (vector) 
        std::getline(lin, linea, '#');
        if (linea == _relacion){ // if linea es igual a la relacion que estamos buscando
            int n=1; //Tabla1#ID#int#4#Name#String#10#Edad#int#2
            while(std::getline(lin, linea, '#')){
                if(n%2 == 0){ tipoAtributo = linea; }
                if(n%3 == 0){ 
                    longMax.push_back(make_pair(tipoAtributo,std::stoi(linea))); 
                    n = 0;
                }
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
    std::vector<pair<string,int>> longMax = longMaxEsquema(_relacion);
    if(longMax.size() == 0)  {std::cout<<" RELACION NO ENCONTRADA! "; return "";}

    std::string _atributo = "";
    // LONGITUD FIJA 
    std::string registroN = ""; // registro nuevo de LONG FIJA
    int it = 0, _longRA=0;
    std::stringstream str(_registro);
    while (std::getline(str, _atributo, ';')){ // leer registro por atributos ";" 
        registroN += _atributo;
        if(_atributo.size() < longMax[it].second) {// contamos la long del registro 
            // funcion para agregar espacios
            if (longMax[it].second > _atributo.size()) {
                registroN.append(longMax[it].second - _atributo.size(), ' ');
            }
            //for(int i=0; i< longMax[it]-_atributo.size(); i++){ registroN += " "; }
        } // else{  for(int j=0; j<_atributo.size() - longMax[it] ;j++){ registroN.pop_back(); } }
        it++;
    }
    //std::cout<<"Registro Nuevo de LONG FIJA >"<<registroN<<">";
    return registroN;
}
// FUNCION PUNTUAL Adiciona registro en un archvo 
void adicionarRLFArchivo(std::string _registro, string _archivo){
    std::ofstream archiv(_archivo, std::ios::app);
    archiv<<_registro;
    archiv.close();
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
        for(int i=0; i<2; i++) getline(esquema, atributo, '#'); // nombre atributo
        if(atributo != "varchar") { 
            getline(esquema, atributo, '#');  // tamaño
            if (stoi(atributo) > aux.size()) {
                aux.append(stoi(atributo) - aux.size(), ' ');
            }
            desplazamiento += aux.size();  
        }else{
            desplazamiento += aux.size();  
            mapa += std::to_string(desplazamiento) + "|";
            getline(esquema, atributo, '#'); 
        }
        RN += aux;
    }

    mapa.push_back('#');
    RN = mapa + RN; // UNIMOS nuestro mapa con Registro
    return RN;
}
void editarSLOTS(int idPage, int slot){
    std::fstream archiv(getdirPage(idPage), std::ios::in);
    string lineaAux = "";
    std::ostringstream nuevo_contenido;
    getline(archiv, lineaAux); // HEADER
    nuevo_contenido<<lineaAux<<endl;
    getline(archiv, lineaAux); // SLOTS
    stringstream slots(lineaAux);
    string nuevaLinea = "";
    while (getline(slots, lineaAux, '|')){
        if(lineaAux == "_") { nuevaLinea += to_string(slot) + "|_|"; break; } 
        nuevaLinea += lineaAux + "|"; 
    }
    nuevo_contenido<<nuevaLinea<<endl;
    getline(archiv, lineaAux); // REGISTROS
    nuevo_contenido<<lineaAux;
    archiv.close();
    //cout<<"\n ------------------\n" ;
    //cout<<nuevo_contenido.str();
    //cout<<"\n ------------------\n" ;
    std::ofstream output(getdirPage(idPage), std::ios::trunc);
    output<<nuevo_contenido.str();
    output.close();
}
////////////////////////// PAGINA //////////////////////////////7
// FUNCION PARA ADICIONAR UN REGISTRO EN UNA TABLA ESPECIFICA
bool adicionarRegistroPage(int idPage, std::string _registro, std::string _relacion, bool tipoR){
    // CREAMOS RLV O RLF
    std::string R = ""; 
    if(tipoR) {R +=  crearRLF(_registro, _relacion);  }
    else{ R += crearRLV(_registro, _relacion); } // REGISTRO LONGITUD VARIABLE

    // este codigo no respeta el espac sectores quiere decir un registro puede partirse
    int espacLibrePage = getcapacLibrePagina(idPage); 
    int tipoPage = getTipoPagina(idPage); // Tener en consideracion el page
    
    if(R.size() <= espacLibrePage && (tipoR == tipoPage || tipoPage == 2)){  // y si es del mismo tipo de R
        espacLibrePage -= R.size();
        std::string _nroR = std::to_string(NroRelacion(_relacion));
        if(tipoPage == 2 && tipoR == 0){
            cout<<"\n REGISTRO EN PAGINA TIPO 2 ACTUALIZANDO!";
            adicionarRLFArchivo("_|\n", getdirPage(idPage));
        }
        //if(tipoPage == 0) 
        editarSLOTS(idPage, R.size());
        adicionarRLFArchivo(R, getdirPage(idPage));
        editarCabeceras(1, 0, _nroR ,  std::to_string(espacLibrePage), tipoR ,getdirPage(idPage));
        
        // editar HEAP FILE 
        //editarCabeceras(idPage,0, "|" ,std::to_string(espacLibrePage), tipoR, "dirPaginas.txt"); 
        return true;
    }
    return false;
}
bool cumpleCondicion(string _registro, string _condicion, string _relacion, bool _tipoR){
    // DESGLOSAMOS nuestra condicion
    std::stringstream cond(_condicion); 
    std::vector<std::string> condicion; string token;
    getline(cond, token, ' ' ); condicion.push_back(token);
    getline(cond, token, ' ' ); condicion.push_back(token);
    getline(cond, token); condicion.push_back(token);

    int nroAtributo = GetnroAtributo(condicion[0], _tipoR, _relacion);
    vector<string> arrayRegistro; // Nuestro registro en un vector de atributos

    vector<pair<string,int>> longitudes = longMaxEsquema(_relacion);
    if(_tipoR){ // FIJA
        for(int i=0; i<longitudes.size(); i++){
            string atributo = _registro.substr(0, longitudes[i].second);
            _registro = _registro.substr(longitudes[i].second);
            atributo.erase(atributo.find_last_not_of(" \t\n\r\f\v") + 1);
            //cout<<atributo<<"|"<<endl;
            arrayRegistro.push_back(atributo);
        }
    }else{
        // 30|34|50|58|59|59|#1  0 3 Braund, Mr. Owen Harrismale22 1 0 A/5 211717.25    S
        stringstream RLV(_registro);
        string valores, aux;
        getline(RLV, valores, '#');
        stringstream slots(valores);
        getline(RLV, valores);
        int conteoLong = 0;
        for(int i=0; i<longitudes.size();i++){
            int limite;
            if(longitudes[i].first == "varchar"){
                getline(slots, aux,'|');
                limite = stoi(aux) - conteoLong;
            }else{ limite = longitudes[i].second; }
            conteoLong += limite;
            string atributo = valores.substr(0, limite);
            valores = valores.substr(limite);
            atributo.erase(atributo.find_last_not_of(" \t\n\r\f\v") + 1);
            // cout<<atributo<<"|"<<endl;
            arrayRegistro.push_back(atributo);
        }
    }
    cout<<"->"<<condicion[0]<<" "<<nroAtributo<<"-"<<arrayRegistro[nroAtributo-1];
    if(condicion[1] == "="){ if(arrayRegistro[nroAtributo-1] == condicion[2] ) return true;}
    //else if(condicion[1] == ">="){ if(r >= condicion[2] ) critCUM = true;}
    //else if(condicion[1] == "<="){ if(r <= condicion[2] ) critCUM = true;}
    //else if(condicion[1] == "<"){ if(r < condicion[2] ) critCUM = true;}
    //else if(condicion[1] == ">"){ if(r >condicion[2] ) critCUM = true;}

    return false;
}
// FUNCION PARA ELIMINAR UN REGISTRO DE LA PAGINA
void eliminarRegistroPage(int _idPage,vector<pair<int,int>> posiciones,bool _tipoR){
    cout<<"\n! ELIMINADNO !\n";
    fstream pagina(getdirPage(_idPage), std::ios::in);
    if (!pagina) { std::cerr << "\n\n Page no se encuentra en el buffer \n"<< std::endl; return;} 
    string lineaAux = "";
    std::ostringstream nuevo_contenido;
    getline(pagina, lineaAux); // HEADER
    nuevo_contenido<<lineaAux<<endl;
    getline(pagina, lineaAux); // SLOTS
    nuevo_contenido<<lineaAux<<endl;
    getline(pagina, lineaAux); // REGISTROS
    string nuevalinea = lineaAux.substr(0, posiciones[0].first);
    for(int i=0; i<posiciones.size()-1; i++){
        int startPos = posiciones[i].first + posiciones[i].second;
        int endPos = posiciones[i + 1].first;
        nuevalinea += lineaAux.substr(startPos, endPos - startPos);
    }
    nuevalinea += lineaAux.substr(posiciones[posiciones.size()-1].first + posiciones[posiciones.size()-1].second);

    nuevo_contenido<<nuevalinea;
    pagina.close();
    // cout<<"\n ------------------\n" ;
    // cout<<nuevo_contenido.str();
    // cout<<"\n ------------------\n" ;
    std::ofstream output(getdirPage(_idPage), std::ios::trunc);
    output<<nuevo_contenido.str();
    output.close();
}

void modificarRegistroPage(int _idPage, string _relacion,int _posI, int _long, string _atributo, bool _tipoR){}

void registroPage(int _idPage, string _relacion, string _condicion, string _atributo, bool _accion){
    std::cout<<" EN FUNCION ELIMINAR !\n";
    int tipoPage = getTipoPagina(_idPage);
    int espacioLibre = getcapacLibrePagina(_idPage);

    fstream pagina(getdirPage(_idPage), std::ios::in | std::ios::out);
    if (!pagina) { std::cerr << "\n\n Page no se encuentra en el buffer \n"<< std::endl; return;}
    string registros = "";
    getline(pagina, registros);// CABECERA
    getline(pagina, registros);// SLOTS
    string registro;
    vector<pair<int,int>> posiciones;
    if(tipoPage == 1) {// FIJA
        vector<pair<string,int>> longitudes = longMaxEsquema(_relacion);
        int longitudFija = getCapacMaxRegistro(longitudes);
        getline(pagina, registros); // REGISTROS 
        pagina.close();
        int posfinal = registros.size();
        int pos = 0;
        while (pos < posfinal){
            registro = registros.substr(pos,longitudFija);
            //cout<<" REGISTRO >"<<registro<<endl;
            if(cumpleCondicion(registro, _condicion, _relacion, tipoPage)){
                //cout<<"\n Cumple con condicion "<<registro<<endl;
                if(_accion == 0) espacioLibre += registro.size();
                posiciones.push_back(make_pair(pos, longitudFija));
            }
            pos+=longitudFija;
        }
    }
    else if(tipoPage == 0) {// VARIABLE
        stringstream slots(registros);
        getline(pagina, registros); // REGISTROS
        pagina.close();
        string aux;
        int post;
        int pos = 0;
        while(getline(slots, aux, '|')){    
            if(aux == "_") break;
            post = stoi(aux);
            if (pos + post > registros.size()) break;
            registro = registros.substr(pos,post);
            //cout<<" REGISTRO >"<<registro<<endl;
            if(cumpleCondicion(registro, _condicion, _relacion, tipoPage)){
                //cout<<"\n Cumple con condicion "<<registro<<endl;
                if(_accion == 0) espacioLibre += registro.size();
                posiciones.push_back(make_pair(pos, post));
            }
            pos += post;
        }
    }else { return; }

    // IF _ACCION ES 0 ELIMINAR
    if(_accion == 0) { 
        eliminarRegistroPage(_idPage,posiciones, tipoPage);
        editarCabeceras(1, 0,"|", to_string(espacioLibre) , tipoPage, getdirPage(_idPage));
        if(tipoPage == 0) ;// VARIABLE
            // EDITAR SLOTS ELIMINAR
    }
    // IF _ACCION ES 1 MODIFICAR
    //else if(_accion == 1)
    
}