#include <iostream>
#include <set>

#include "Buffer.cpp"

using namespace std;

void insert(Buffer* _myBuffer) {
    int R = 0;  // POLITICA RLV 0 / RLF 1
    string n;
    string archivo, relacion;
    cout << "\n----- ADICIONANDO REGISTROS -----\n";
    std::cout << " Adicionando (n) registros del (archivo) a la (Relacion) >\n";
    std::cout << " n> (n/*)   \t";
    cin >> n;  /// 1
    std::cout << " Archivo >  \t ";
    cin >> archivo;
    std::cout << " Relacion > \t";
    cin >> relacion;
    archivo = "DATABASE/" + archivo;
    // std::cout<<" Registro Long FIJA (1) O Varible(0) > ";cin>>R;

    std::ifstream data(archivo);
    std::string registro;
    std::getline(data, registro);

    pair<string, int> claveBusqueda = chooseClaveBusqueda(relacion);

    cout << " CLAVE DE BUSQUEDA SELECCIONADA! " << claveBusqueda.first << "-" << claveBusqueda.second << endl;

    vector<int> IDPAGES;

    int loopLimit = (n == "*") ? INT_MAX : stoi(n);
    for (int i = 0; i < loopLimit; ++i) {
        if (!std::getline(data, registro)) {
            break;
        }  // Salir del bucle si no hay más registros

        vector<string> vector_registro = getVectorRegistro(registro);
        int key;
        if (vector_registro[claveBusqueda.second] == " ") key = 0;
        else key = stoi(vector_registro[claveBusqueda.second]);
        key = getIndiceDisperso(key);
        
        int _idPage = _myBuffer->getBloque(relacion, claveBusqueda.first, key);
        _myBuffer->pinPage(_idPage, 'W', 0);

        int espacioLibre = adicionarRegistroPage(_idPage, registro, relacion, R);
        // if(espacioLibre == -1) {
        //     key += 10;
        //     _idPage = _myBuffer->getBloque(relacion, claveBusqueda.first, key);
        //     _myBuffer->pinPage(_idPage, 'W', 0);
        //     espacioLibre =  adicionarRegistroPage(_idPage, registro, relacion, R);
        // }
        if (espacioLibre != -1) {  // si lo inserto
            cout << "\n REGISTRO \n"
                 << registro << " insertado en " << _idPage << endl;
            IDPAGES.push_back(_idPage);
            // AÑADIR A CAMBIOS
            _myBuffer->addChanges(_idPage, key, 1, relacion, claveBusqueda.first);
            // AÑADIR RUTA
            _myBuffer->addRuta(relacion, claveBusqueda.first, key, make_pair(_idPage, 0));
            // EDITAR HEAP FILES
            _myBuffer->updateCapacBloqueHF(relacion, _idPage, espacioLibre);
        } 
        else {
            cout << "\n\n >> LIMITE DE ALMACENAMIENTO ALCANZADO! >> \n\n";
        }
    }
    // Usar std::set para eliminar duplicados
    std::set<int> uniqueElements(IDPAGES.begin(), IDPAGES.end());
    IDPAGES.assign(uniqueElements.begin(), uniqueElements.end());

    _myBuffer->printBuffer();
    _myBuffer->printIndice(relacion);
    cout << "\n SE INSERTARON LOS DATOS CORRECTAMENTE \n";
    char guardar;
    cout << " Desea guardar los cambios? (S/N): ";
    cin >> guardar;
    for (int i = 0; i < IDPAGES.size(); i++) {
        _myBuffer->superUnpinPage(IDPAGES[i], guardar);
    }
    cout << "\n CAMBIOS GUARDADOS EN EL DISCO EXITOSAMENTE \n";
    data.close();
}

void delet(Buffer* _myBuffer) {
    string relacion, condicion;
    std::cout << " Eliminando registro de la (relacion) donde (condicion) >\n";
    std::cout << " Relacion > ";
    cin >> relacion;
    std::cout << " Condicion > ";
    getline(std::cin >> std::ws, condicion);

    pair<string, int> claveBusqueda = chooseClaveBusqueda(relacion);

    std::vector<std::string> condicionVector = getCondicionVector(condicion);

    int key = getIndiceDisperso(stoi(condicionVector[2]));
    //int key = stoi(condicionVector[2]);

    int _idPage = _myBuffer->getBloque(relacion, claveBusqueda.first, key);
    _myBuffer->pinPage(_idPage, 'W', 0);
    int espacioLibre = registroPage(_idPage, relacion, condicion, "", 0);
    // EDITAR HEAP FILES
    _myBuffer->updateCapacBloqueHF(relacion, _idPage, espacioLibre);
    // AÑADIR A CAMBIOS
    _myBuffer->addChanges(_idPage, key, 0, relacion, claveBusqueda.first);
    // ELIMINAR RUTA
    //_myBuffer->deleteRuta(relacion, "", key);

    cout << "\n ELIMINANDO DE PAGINA> " << _idPage << endl;
    char guardar;
    cout << " Desea guardar los cambios? (S/N): ";
    cin >> guardar;
    _myBuffer->superUnpinPage(_idPage, guardar);
    cout << "\n ELIMINADO GUARDADOS EN EL DISCO EXITOSAMENTE \n";
}

void consultas(Buffer* _myBuffer) {
    string relacion, condicion, atributo;
    std::cout << " Consultando en la (relacion) del registro donde (condicion) >\n";
    std::cout << " Relacion > ";
    cin >> relacion;
    std::cout << " Condicion > ";
    getline(std::cin >> std::ws, condicion);

    pair<string, int> claveBusqueda = chooseClaveBusqueda(relacion);
    std::vector<std::string> condicionVector = getCondicionVector(condicion);

    int key = getIndiceDisperso(stoi(condicionVector[2]));
    //int key = stoi(condicionVector[2]);

    int _idPage = _myBuffer->getBloque(relacion, claveBusqueda.first, key);
    _myBuffer->pinPage(_idPage, 'R', 0);
    registroPage(_idPage, relacion, condicion, "", 1);
}

void requerimientos(Buffer* _myBuffer) {
    int op;
    do {
        cout << "_______________________ MENU _______________________\n\n";
        cout << "1. INSERT \n";
        cout << "2. DELETE \n";
        cout << "3. SELECT \n";
        cout << "4. EXIT  \n";
        cout << "__________________________________________________\n\n";
        cout << "Elija una opcion: ";
        cin >> op;

        if (op == 1) {
            insert(_myBuffer);
        } else if (op == 2) {
            delet(_myBuffer);
        } else if (op == 3) {
            consultas(_myBuffer);
        } else if (op == 4) {
            cout << " Saliendo \n ";
        } else {
            cout << " Ingrese una opción valida! \n ";
            return;
        }
    } while (op != 4);
}

int main() {
    Disco* Disco1;
    int opcion;
    int platos, pistas, sectores, bloques, tamSector, tamBloque;
    // cout << " > Crear DISCO " << endl;
    // cout << " DEFAULT (0) o MANUAL (1) > ";cin>>opcion;
    opcion = 0;
    if (opcion == 0) {
        Disco1 = new Disco();
    } else if (opcion == 1) {
        std::cout << " Cantidad de platos> ";
        cin >> platos;
        std::cout << " Cantidad de pistas> ";
        cin >> pistas;
        std::cout << " Cantidad de Sectores> ";
        cin >> sectores;
        std::cout << " Capacidad de sector> ";
        cin >> tamSector;
        std::cout << " Capacidad de bloque> ";
        cin >> tamBloque;
        Disco1 = new Disco(platos, pistas, sectores, tamSector, tamBloque);
    }
    // std::cout<<" DISCO  CREADO >>\n";
    Disco1->printDisco();

    int choice;
    // cout << " > Metodo de reemplazo " << endl;
    // cout << " LRU (0) o CLOCK (1) > ";cin>>choice;
    choice = 1;

    Buffer* myBuffer = new Buffer(Disco1->getCapacidadBloque() * 10, Disco1->getCapacidadBloque(), Disco1, choice);
    myBuffer->printBuffer();

    requerimientos(myBuffer);
    do {
        cout << "\n----- MENU SISTEMA -----\n";
        cout << "1. MENU DISCO \n";
        cout << "2. MENU BUFFER\n";
        cout << "3. MENU USUARIO !! \n";
        cout << "4. Salir\n";
        cout << "Elija una opcion: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                MenuDisco(Disco1);
                break;
            }
            case 2: {
                myBuffer->MenuBuffer();
                break;
            }
            case 3: {
                requerimientos(myBuffer);
                break;
            }
            case 4: {
                cout << "Saliendo...\n";
                break;
            }
            default: {
                cout << "Opción no válida. Intente de nuevo.\n";
            }
        }
    } while (choice != 4);
}