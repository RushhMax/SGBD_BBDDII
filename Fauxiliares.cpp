#include <iostream>
#include <string>
#include <vector>
#include <sstream> // para mis stringsteam
#include <fstream>
#include <algorithm> // Para std::remove_if
#include <cctype>    // Para std::isspace

using namespace std;

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


// FUNCION EXTRA  DE ADIC_RELACION(1/2) 
char is_IntFlo(std::string _dato){ //T ENTERO F FLOAT S STRING;
    for(int i=0; i<_dato.size();i++){
        if(_dato[i] == '.') return 'F';
        if(!isdigit(_dato[i])) return 'S';
    }
    return 'T';
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

// Funcion para crear un registro de longitud variable
std::string crearRLV(std::string _registro){
    std::string RN = "", aux, mapa="";
    int desplazamiento = 0;
    std::stringstream R(_registro); // return _registro
    while(getline(R, aux, ';')){
        mapa += std::to_string(desplazamiento) + "|";
        desplazamiento += aux.size(); 
        RN += aux;
    }
    mapa += std::to_string(desplazamiento) + "|";
    //mapa.pop_back();
    mapa.push_back('#');
    RN = mapa + RN;
    return RN;
}

// Funcion para crear un registro de longitud fija de acuerdo a su relcion
std::string crearRLF(std::string _registro, std::string _relacion, int _long){
    std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
    std::string _atributo = "", linea = "";
    //std::cout<<" TRANSFORMANDO >"<<_registro<<"> LONG FIJA de la RELACION >"<<_relacion<<">"<<_long<<">\n";

    //std::cout<<" LONGITUDES DE LA TABLA >\n"; 
    std::vector<int> longMax; 
    bool relacionEncontrada = false;
    while (std::getline(diccionario, linea) && !relacionEncontrada ){
        std::stringstream lin(linea); // de nuestro diccionario buscamos la tabla y el #4 y #7  #10 etc (vector) 
        std::getline(lin, linea, '#');
        if (linea == _relacion){ // if linea es igual a la relacion que estamos buscando
            int n=1; //Tabla1#ID#int#4#Name#String#10#Edad#int#2
            while(std::getline(lin, linea, '#')){
                std::stringstream longAtr(linea);
                getline(longAtr, linea, '_');
                if(n%3 == 0){ longMax.push_back(std::stoi(linea));}
                n++;
            }
            relacionEncontrada = true;
        }
    }
    if(!relacionEncontrada){std::cout<<" RELACION NO ENCONTRADA! "; return "";}

    // LONGITUD FIJA 
    std::string registroN = ""; // registro nuevo de LONG FIJA
    int it = 0, _longRA=0;
    std::stringstream str(_registro);
    while (std::getline(str, _atributo, ';')){ // leer registro por atributos ";" 
        registroN += _atributo;
        if(_atributo.size() < longMax[it]) {// contamos la long del registro 
            for(int i=0; i< longMax[it]-_atributo.size(); i++){ registroN += " "; }
        }else{ 
            for(int j=0; j<_atributo.size() - longMax[it] ;j++){ registroN.pop_back(); } }
        registroN += " ";
        it++;
    }

    //if(_long != 0){
      //  if(registroN.size() < _long){ 
        //    for(int i=0; i<_long-registroN.size(); i++){ registroN += " ";}
        //}
        //else if(registroN.size() > _long){
            //for(int j=0; j<registroN.size()-_long; j++){ registroN.pop_back(); }    
            //registroN.pop_back();
            //registroN += ";";
        //}
    //}

    //std::cout<<"Registro Nuevo de LONG FIJA >"<<registroN<<">";
    diccionario.close();
    return registroN;
}

// DEVUELVE LA CAPACIDAD LIBRE DE LA PAGINA
int capacLibrePagina(int idPage){
    std::ifstream pagina("BUFFERPOOL/Page" + to_string(idPage) + ".txt");
    std::string cap = "";
    getline(pagina, cap, '#');
    pagina.close();
    return stoi(cap);
}

// EDITA EL DIRECTORIO O CABEZALES DE ARCHIVOS
void editarCabeceras(int Linea, int Posicion, std::string _nRelacion,std::string Ndato, std::string _archivo){
    std::fstream archivoL(_archivo, std::ios::binary | std::ios::in | std::ios::out);
    std::string Contlinea = "", nuevaLinea = "", lineaAUX = "";

    // OBTENEMOS BLOQUE contlinea = bloque
    for(int i=0; i<Linea; i++){ getline(archivoL, Contlinea);}  
    //1 nuevaLinea = Contlinea.substr(0, posicion); // copiando antes de la pos
    std::stringstream str(Contlinea);
    for(int i=0; i<Posicion; i++){getline(str, lineaAUX, '_'); nuevaLinea += lineaAUX + '_';}
    
    //std::cout<<">"<<nuevaLinea<<">\n";

    getline(str, lineaAUX, '#'); // nos pasamos el registro que quiero borrar
    //std::cout<<" Borrando registro "<<lineaAUX+"#\n";

    //std::cout<<">"<<nuevaLinea<<" + "<<Ndato<<"# + "<<lineaAUX<<"\n";

    nuevaLinea += Ndato + "#";
    if(Posicion == 0) {
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
        if (BloqueActual != Linea) {
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

// FUNCION PUNTUAL Adiciona registro en un archvo 
void adicionarRegistroArchivo(std::string _registro, string _archivo){
    std::ofstream archiv(_archivo, std::ios::app);
    archiv<<_registro<<"\n";
    archiv.close();
}   

string dirSector(string dir){
    std::stringstream ss(dir);
    std::vector<std::string> ubi;
    std::string token = "";
    while(std::getline(ss, token, '/')){ ubi.push_back(token);}
    
    std::string dirSector = "DISCO/Plato" + ubi[0] + "/S" + ubi[1] + "/Pista" + ubi[2] + "/Sector" + ubi[3] + ".txt";
    return dirSector;
}

// FUNCION PARA ADICIONAR UN REGISTRO EN UNA TABLA ESPECIFICA
bool adicionarRegistroP(int idPage, std::string _registro, std::string _relacion, int _longMax){
    // CREAMOS RLV O RLF
    std::string R = ""; 
    if(_longMax != 0) {R +=  crearRLF(_registro, _relacion, _longMax);  }
    else{ R += crearRLV(_registro); } // REGISTRO LONGITUD VARIABLE

    int espacLibrePage = capacLibrePagina(idPage); 
    // este codigo no respeta el espac sectores quiere decir un registro puede partirse
    if(R.size() <= espacLibrePage){
        espacLibrePage -= R.size();
        std::string _nroR = std::to_string(NroRelacion(_relacion));
        if(_longMax != 0) R = _nroR + "#1#" + R; // REGISTRO DE LONG F
        else R = _nroR + "#0#" + R; // REGSITRP LONG V
        adicionarRegistroArchivo(R, "BUFFERPOOL/Page" + std::to_string(idPage) + ".txt");
        editarCabeceras(1, 0, _nroR ,  std::to_string(espacLibrePage),  "BUFFERPOOL/Page" + std::to_string(idPage) + ".txt");
        return true;
    }else { cout << " No hay espacio suficiente! \n"; return false;}
}

// FUNCION ADICIONAR N REGISTROS DE UN CSV
void adicionarRegistrosPage(int idPage, string n, std::string _archivo, std::string _relacion, bool _TipoR){
    std::ifstream data(_archivo);
    std::string registro;
    std::getline(data, registro); // saltandonos encabezado
    // CAPACIDAD MAXIMA DE REGISTRO calcular si TIPO DE DATO 1(Variable)
    int capMax =0;
    if(_TipoR == 1) {capMax = capacMaxRegistro(_archivo);} //IF VARIABLE
    //std::cout<<" CAP MAX ES>"<<capMax<<"\n";
    //std::cout<<"n>"<<n<<"\n";
    if(n == "*"){
        while(std::getline(data, registro)){
            if(_TipoR){ adicionarRegistroP(idPage, registro, _relacion, capMax);}
            else{ adicionarRegistroP(idPage, registro, _relacion, 0);}
        }
        data.close(); 
    }
    else{
        for(int i=0; i< stoi(n) ; i++){
            std::getline(data, registro);
            // rLF = funcion RLV = 0
            if(_TipoR){ adicionarRegistroP(idPage,registro, _relacion, capMax);}
            else{ adicionarRegistroP( idPage, registro, _relacion, 0);}
        }
        data.close();
    }
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
    
    // OBTENEMOS ESQUEMA
    ifstream relaciones("diccionario.txt");
    string _esquema;
    for(int i=0; i<NroRelacion(_relacion); i++){
        getline(relaciones, _esquema);
    }
    relaciones.close();
    stringstream esquema(_esquema);
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
    std::cout<<" EN FUUNCION ELIMINAR !\n";
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
                editarCabeceras(1, 0, std::to_string(nroRelac) , std::to_string(espacioPageINT),  "BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt");
            }

        }
    }
    pagina.close();
}

void modificarRegistroPage(int _idPage, int _nroRegistro, string _atributo){
    fstream _Page("BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt", std::ios::binary | std::ios::in | std::ios::out);
    
    std::ostringstream nuevo_contenido;
    string lineaAUX;
    int contLinea = 0;
    while (getline(_Page, lineaAUX)) {
        contLinea++;
        lineaAUX.pop_back();
        if (contLinea != _nroRegistro) {
            nuevo_contenido << lineaAUX <<endl;
        }else{
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
                modificarRegistroPage(_idPage, nroregistro, _atributo);
                //espacioPageINT += registro.size();
                //std::cout<<" ESPACIO "<<espacioPageINT<<endl;
                //editarCabeceras(1, 0, std::to_string(nroRelac) , std::to_string(espacioPageINT),  "BUFFERPOOL/Page" + std::to_string(_idPage) + ".txt");
            }

        }
    }
    pagina.close();
}
