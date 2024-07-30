#include "HeapFile.cpp"

using namespace std;

//_______________________________ RELACION _____________________________
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
string getNombreRelacion(int nroRelacion){
    string aux;
    std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
    for(int i=0; i<nroRelacion; i++){
        getline(diccionario, aux);
    }
    stringstream relacion(aux);
    getline(relacion, aux, '#');
    diccionario.close();
    return aux;
}
string getEsquema(string _relacion){
    ifstream relaciones("diccionario.txt");
    string _esquema;
    for(int i=0; i<NroRelacion(_relacion); i++){
        getline(relaciones, _esquema);
    }
    relaciones.close();
    stringstream esquema(_esquema);
    getline(esquema, _esquema, '_');
    return _esquema;
}

//_______________________________ MAIN _____________________________
// RETORNA ATRIBUTOS
std::vector<string> getAtributos(string _relacion){
    std::vector<string> atributos; 
    std::string linea = getEsquema(_relacion);
    string Atributo;
    std::stringstream lin(linea); // de nuestro diccionario buscamos la tabla y el #4 y #7  #10 etc (vector) 
    std::getline(lin, linea, '#');
    int n=1; //Tabla1#ID#int#4#Name#String#10#Edad#int#2
    while(std::getline(lin, linea, '#')){
        if(n == 1){ Atributo = linea; }
        if(n%3 == 0){ 
            atributos.push_back(Atributo); 
            n = 0;
        }
        n++;
    }
    return atributos;
}
// CONSIEGUE LLAVE PRIMARIO DE LA RELACION
string getPrimaryKey(string _relacion){
    string aux;
    std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
    while (getline(diccionario, aux)){
        std::stringstream strRelacion(aux);
        getline(strRelacion,aux, '#');
        if(_relacion == aux){
            getline(strRelacion,aux, '_');
            getline(strRelacion,aux, '_');
            getline(strRelacion,aux, '#');
            diccionario.close();
            return aux;
        }    
    }
    diccionario.close();
    cout<<"\n! RELACION NO ENCONTRADA !\n";
    return "";
}
// Retorna clave de busqueda (nombre de la clave de busqueda, nro de atributo (clave de busqueda))
pair<string,int> chooseClaveBusqueda(string _relacion){ 
    vector<string> atributos = getAtributos(_relacion);
    cout<<"\n ----- ELIGIENDO CLAVE DE BUSQUEDA ------- \n";
    cout<<" > OPCIONES:\n";
    cout<<" (-1) LLAVE PRIMARIA \n"; 
    for(int i=0; i<atributos.size(); i++){
        cout<<"  ("<<i<<") "<<atributos[i]<<"\n";
    }
    int opcion;
    cout<<" Ingresar opcion: ";cin>>opcion;
    if(opcion == -1) return make_pair( getPrimaryKey(_relacion),0);
    if(opcion < atributos.size()){
        return make_pair(atributos[opcion], opcion);
    }
    cout<<" OPCION NO VALIDA \n";
    return chooseClaveBusqueda(_relacion);
}
int getIndiceDisperso(int key){
    return key/20;
}


//_______________________________ PATH _____________________________
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


//_______________________________ BLOQUE _____________________________
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
int getCapacLibreBloque(int idBloque){
    std::ifstream bloque(getdirBloque(idBloque));
    std::string cap = "";
    getline(bloque, cap, '#');
    bloque.close();
    return stoi(cap);
}

//_______________________________ PAGINA _____________________________
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
string getRelacionPagina(int idPage){
    std::ifstream pagina(getdirPage(idPage));
    std::string tipo = "";
    getline(pagina, tipo, '#'); // capacidad 
    getline(pagina, tipo, '#'); // tipo de 
    getline(pagina, tipo, '#'); // NOMVRE
    getline(pagina, tipo, '#'); // NRO
    getline(pagina, tipo, '#'); // CAPACIDAD
    getline(pagina, tipo, '#'); // Nro RELACION
    pagina.close();
    return getNombreRelacion(stoi(tipo.substr(1)));
}

//_______________________________ EDICION _____________________________
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

//_______________________________ ADICION _____________________________

////////////////////////// RLF //////////////////////////////7
// RETORNA TIPO DE VALOR Y LONGITUD
std::vector<pair<string,int>> longMaxEsquema(string _relacion) {
    std::vector<pair<string,int>> longMax; 
    std::string linea = getEsquema(_relacion);
    string tipoAtributo;
    std::stringstream lin(linea); // de nuestro diccionario buscamos la tabla y el #4 y #7  #10 etc (vector) 
    std::getline(lin, linea, '#');
    int n=1; //Tabla1#ID#int#4#Name#String#10#Edad#int#2
    while(std::getline(lin, linea, '#')){
        if(n%2 == 0){ tipoAtributo = linea; }
        if(n%3 == 0){ 
            longMax.push_back(make_pair(tipoAtributo,std::stoi(linea))); 
            n = 0;
        }
        n++;
    }
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
void agregarSLOTS(int idPage, int slot){
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

////////////////////////// ADICIONAR //////////////////////////////7
int adicionarRegistroPage(int _idPage, std::string _registro, std::string _relacion, bool tipoR){
    // CREAMOS RLV O RLF
    std::string R = ""; 
    if(tipoR) {R +=  crearRLF(_registro, _relacion);  }
    else{ R += crearRLV(_registro, _relacion); } // REGISTRO LONGITUD VARIABLE

    int espacLibrePage = getcapacLibrePagina(_idPage); 
    int tipoPage = getTipoPagina(_idPage); // Tener en consideracion el page
    
    if(R.size() <= espacLibrePage && (tipoR == tipoPage || tipoPage == 2)){  // y si es del mismo tipo de R
        espacLibrePage -= R.size();
        std::string _nroR = std::to_string(NroRelacion(_relacion));
        if(tipoPage == 2 && tipoR == 0){
            adicionarRLFArchivo("_|\n", getdirPage(_idPage));
        }
        agregarSLOTS(_idPage, R.size());
        adicionarRLFArchivo(R, getdirPage(_idPage));
        editarCabeceras(1, 0, _nroR ,  std::to_string(espacLibrePage), tipoR ,getdirPage(_idPage));

        return espacLibrePage;
    }
    return -1;
}

//_______________________________ CONSULTAR _____________________________
int getCapacMaxRegistro(vector<pair<string,int>> _longMaxEsquema){
    int capac = 0;
    for(int i=0; i<_longMaxEsquema.size();i++){
        capac += _longMaxEsquema[i].second;
    }
    return capac;
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
void imprimirRegistro(vector<string> _registro, vector<pair<string,int>> _longitudes){
    for (int i = 0; i < _registro.size(); i++) {
        int longitud = (i < _longitudes.size()) ? _longitudes[i].second : 0;
        cout << setw(longitud) << left << _registro[i] << " ";
    }
    cout << endl;
}
/// REGISTRO /////////////
vector<string> getVectorRegistro(string _registro){
    vector<string> arrayRegistro;
    string aux;
    stringstream registro(_registro);
    while(getline(registro, aux, ';')){
        if(aux == "") aux = " ";
        arrayRegistro.push_back(aux);
    }
    return arrayRegistro;
}
vector<string> getVectorRegistro(string _registro, string _relacion, bool _tipoR){
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
    return arrayRegistro;
}

//_______________________________ CONSULTAR _____________________________
/// CONDICION ////////////
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
vector<std::string> getCondicionVector(string _condicion){
    std::vector<std::string> condicion;
    std::stringstream cond(_condicion); 
    string token;
    getline(cond, token, ' ' ); condicion.push_back(token);
    getline(cond, token, ' ' ); condicion.push_back(token);
    getline(cond, token); condicion.push_back(token);
    return condicion;
}
bool cumpleCondicion(string _registro, string _condicion, string _relacion, bool _tipoR){
    std::vector<std::string> condicion = getCondicionVector(_condicion);
    
    int nroAtributo = GetnroAtributo(condicion[0], _tipoR, _relacion);
    vector<string> arrayRegistro = getVectorRegistro(_registro, _relacion, _tipoR); // Nuestro registro en un vector de atributos

    //cout<<"->"<<condicion[0]<<" "<<nroAtributo<<"-"<<arrayRegistro[nroAtributo-1];
    if(condicion[1] == "="){ if(arrayRegistro[nroAtributo-1] == condicion[2] ) return true;}
    //else if(condicion[1] == ">="){ if(r >= condicion[2] ) critCUM = true;}
    //else if(condicion[1] == "<="){ if(r <= condicion[2] ) critCUM = true;}
    //else if(condicion[1] == "<"){ if(r < condicion[2] ) critCUM = true;}
    //else if(condicion[1] == ">"){ if(r >condicion[2] ) critCUM = true;}

    return false;
}
// FUNCION PARA ELIMINAR UN REGISTRO DE LA PAGINA
void eliminarRegistroPage(int _idPage,vector<pair<int,int>> posiciones,bool _tipoR){
    //cout<<"\n! ELIMINADNO !\n";
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
// FUNCION PARA ELIMINAR/MODFICAR UN REGISTRO DE LA PAGINA //ACCION = 0 ELIMINAR  || ACCION = 1 MODIFICAR
void eliminarSLOTS(int _idPage, vector<int> _slots){
    std::fstream archiv(getdirPage(_idPage), std::ios::in);
    string lineaAux = "";
    std::ostringstream nuevo_contenido;
    getline(archiv, lineaAux); // HEADER
    nuevo_contenido<<lineaAux<<endl;

    getline(archiv, lineaAux); // SLOTS
    stringstream slots(lineaAux);
    string nuevaLinea = "";
    int i = 0; // 8|7|5|6|_|
    while (getline(slots, lineaAux, '|')){
        if(lineaAux == "_") break;
        //cout<<" stoi(lineaAux) == _slots[i] "<<stoi(lineaAux)<<" == "<<_slots[i] <<endl;
        if(stoi(lineaAux) == _slots[i] ) { 
            i++;
            if(i == _slots.size()) {break;} 
        } 
        else { nuevaLinea += lineaAux + "|"; }
    }
    getline(slots, lineaAux);
    nuevaLinea += lineaAux;
    nuevo_contenido<<nuevaLinea<<endl;

    getline(archiv, lineaAux); // REGISTROS
    nuevo_contenido<<lineaAux;
    archiv.close();
    //cout<<"\n ------------------\n" ;
    //cout<<nuevo_contenido.str();
    //cout<<"\n ------------------\n" ;
    std::ofstream output(getdirPage(_idPage), std::ios::trunc);
    output<<nuevo_contenido.str();
    output.close();
}
int registroPage(int _idPage, string _relacion, string _condicion, string _atributo, bool _accion){
    int tipoPage = getTipoPagina(_idPage); // RLV Y RLF
    int espacioLibre = getcapacLibrePagina(_idPage); 

    fstream pagina(getdirPage(_idPage), std::ios::in | std::ios::out);
    if (!pagina) { std::cerr << "\n\n Page no se encuentra en el buffer \n"<< std::endl; return -1;}
    
    // LEYENDO PAGINA
    string registros = "";
    getline(pagina, registros);// CABECERA
    getline(pagina, registros);// SLOTS
    
    string registro;
    vector<pair<int,int>> posiciones;
    vector<int> vectorSlots;

    vector<pair<string,int>> longitudes = longMaxEsquema(_relacion);
    
    if(tipoPage == 1) {// FIJA
        int longitudFija = getCapacMaxRegistro(longitudes);
        getline(pagina, registros); // REGISTROS 
        pagina.close();
        int posfinal = registros.size();
        int pos = 0;
        while (pos < posfinal){
            registro = registros.substr(pos,longitudFija);
            //cout<<" REGISTRO >"<<registro<<endl;
            if(cumpleCondicion(registro, _condicion, _relacion, tipoPage)){
                if(_accion == 0) espacioLibre += registro.size();
                if(_accion == 1)  {
                    vector<string> arrayRegistro = getVectorRegistro(registro, _relacion, tipoPage);
                    imprimirRegistro(arrayRegistro, longitudes);
                }
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
                vectorSlots.push_back(post);
                if(_accion == 0) espacioLibre += registro.size();
                if(_accion == 1)  {
                    vector<string> arrayRegistro = getVectorRegistro(registro, _relacion, tipoPage);
                    imprimirRegistro(arrayRegistro, longitudes);
                }
                posiciones.push_back(make_pair(pos, post));
            }
            pos += post;
            
        }
    }else { return -1; }

    // IF _ACCION ES 0 ELIMINAR
    if(_accion == 0) { 
        eliminarRegistroPage(_idPage,posiciones, tipoPage);
        editarCabeceras(1, 0,"|", to_string(espacioLibre) , tipoPage, getdirPage(_idPage));
        if(tipoPage == 0) {// VARIABLE
            eliminarSLOTS(_idPage, vectorSlots);// EDITAR SLOTS ELIMINAR
        }
        return espacioLibre;
    }
    return -1;
}