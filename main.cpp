#include <iostream>
#include <set>

#include "Buffer.cpp"

using namespace std;

void insert(Buffer* _myBuffer, string relacion, vector<pair<string, int>> clavesBusqueda) {
    int R = 0;  // POLITICA RLV 0 / RLF 1
    string n;
    string archivo;
    cout << "\n----- ADICIONANDO REGISTROS -----\n";
    std::cout << " Adicionando (n) registros del (archivo) a la (Relacion) >\n";
    std::cout << " n> (n/*)   \t";
    cin >> n;  /// 1
    std::cout << " Archivo >  \t ";
    cin >> archivo;
    archivo = "DATABASE/" + archivo;
    // std::cout<<" Registro Long FIJA (1) O Varible(0) > ";cin>>R;

    std::ifstream data(archivo);
    std::string registro;
    std::getline(data, registro);

    vector<int> IDPAGES;

    int loopLimit = (n == "*") ? INT_MAX : stoi(n);
    for (int i = 0; i < loopLimit; ++i) {
        if (!std::getline(data, registro)) {
            break;
        }  // Salir del bucle si no hay más registros

        vector<string> vector_registro = getVectorRegistro(registro);
        vector<int> keys;
        for(int j=0; j<clavesBusqueda.size();j++){
            keys.push_back(0);
            if (vector_registro[clavesBusqueda[j].second] == " ") keys[j] = 0;
            else keys[j] = stoi(vector_registro[clavesBusqueda[j].second]);
            keys[j] = getIndiceDisperso(keys[j]);
        }
        
        int _idPage = _myBuffer->getBloque(relacion, clavesBusqueda[0].first, keys[0]);
        _myBuffer->pinPage(_idPage, 'W', 0);

        int espacioLibre = adicionarRegistroPage(_idPage, registro, relacion, R);
        if (espacioLibre != -1) {  // si lo inserto
            cout << "\n REGISTRO \n"
                 << registro << " insertado en " << _idPage << endl;
            IDPAGES.push_back(_idPage);

            for(int j=0; j<clavesBusqueda.size(); j++){
                // AÑADIR A CAMBIOS
                _myBuffer->addChanges(_idPage, keys[j], 1, relacion, clavesBusqueda[j].first);
                // AÑADIR RUTA
                _myBuffer->addRuta(relacion, clavesBusqueda[j].first, keys[j], make_pair(_idPage, 0));
            }

            // EDITAR HEAP FILES
            _myBuffer->updateCapacBloqueHF(relacion, _idPage, espacioLibre);
        } else {
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

void delet(Buffer* _myBuffer, string relacion, vector<pair<string, int>> clavesBusqueda) {
    string condicion;
    std::cout << " Eliminando registro de la (relacion) donde (condicion) >\n";
    // std::cout << " Relacion > ";
    // cin >> relacion;
    std::cout << " Condicion > ";
    getline(std::cin >> std::ws, condicion);

    std::vector<std::string> condicionVector = getCondicionVector(condicion);
    int key = getIndiceDisperso(stoi(condicionVector[2]));

    //int key = stoi(condicionVector[2]);

    // for(int i=0; i<clavesBusqueda.size();i++){
    //     cout<<" Claves "<<clavesBusqueda[i].first<<" "<<condicionVector[0]<<endl;
    //     if(clavesBusqueda[i].first == condicionVector[0]){
            int _idPage = _myBuffer->getBloque(relacion, condicionVector[0], key);
            _myBuffer->pinPage(_idPage, 'W', 0);

            int espacioLibre = registroPage(_idPage, relacion, condicion, "", 0);
            // EDITAR HEAP FILES
            _myBuffer->updateCapacBloqueHF(relacion, _idPage, espacioLibre);
            // AÑADIR A CAMBIOS
            _myBuffer->addChanges(_idPage, key, 0, relacion, clavesBusqueda[0].first);
            // ELIMINAR RUTA
            //_myBuffer->deleteRuta(relacion, "", key);

            cout << "\n ELIMINANDO DE PAGINA> " << _idPage << endl;
            char guardar;
            cout << " Desea guardar los cambios? (S/N): ";
            cin >> guardar;
            _myBuffer->superUnpinPage(_idPage, guardar);
            cout << "\n ELIMINADO GUARDADOS EN EL DISCO EXITOSAMENTE \n";

            return;
    //     }
    // }
}

void consultas(Buffer* _myBuffer,string relacion, vector<pair<string, int>> clavesBusqueda) {
    string  condicion, atributo;
    std::cout << " Consultando en la (relacion) del registro donde (condicion) >\n";
    // std::cout << " Relacion > ";
    // cin >> relacion;
    std::cout << " Condicion > ";
    getline(std::cin >> std::ws, condicion);

    // pair<string, int> claveBusqueda = chooseClaveBusqueda(relacion);
    std::vector<std::string> condicionVector = getCondicionVector(condicion);
    int key = getIndiceDisperso(stoi(condicionVector[2]));

    vector<int> datas_;
    datas_.push_back(key);
    vector<int> datas;
    // for(int i=0; i<clavesBusqueda.size();i++){
    //     if(clavesBusqueda[i].first == condicionVector[0]){
            BPlusTree<int>* indice = _myBuffer->getIndice(relacion, clavesBusqueda[0].first);
            if(!indice) return;
            if (condicionVector[1] == ">" || condicionVector[1] == ">=") {
                datas = indice->getAllGreaterThan(key);
                datas_.insert(datas_.end(), datas.begin(), datas.end());
            } else if (condicionVector[1] == "<" || condicionVector[1] == "<=") {
                datas = indice->getAllLessThan(key);
                datas_.insert(datas_.end(), datas.begin(), datas.end());
            }
            for (const auto i : datas_) {
                int _idPage = _myBuffer->getBloque(relacion, clavesBusqueda[0].first, i);
                _myBuffer->pinPage(_idPage, 'R', 0);
                registroPage(_idPage, relacion, condicion, "", 1);
            }
            return;
    //     }
    // }


}

void requerimientos(Buffer* _myBuffer) {    

    int op;
    do {
        // Choose a relation and search keys
        string relacion;
        cout << "_______________________ MENU _______________________\n\n";
        cout << "1. RELACION \n";
        cout << "2. EXIT \n";
        cout << "Elija una opcion: ";
        cin >> op;        

        if(op == 1) {
            cout << "Ingrese la relación: "; cin >> relacion;
            vector<pair<string, int>> clavesBusqueda = chooseClavesBusquedas(relacion);
            for (int i = 0; i < clavesBusqueda.size(); i++) {
                _myBuffer->addIndice(relacion, clavesBusqueda[i].first);
            }

            int opcion;
            do {
                cout << "_______________________ MENU _______________________\n\n";
                cout << "1. INSERT \n";
                cout << "2. DELETE \n";
                cout << "3. SELECT \n";
                cout << "4. EXIT  \n";
                cout << "__________________________________________________\n\n";
                cout << "Elija una opcion: ";
                cin >> opcion;

                if (opcion == 1) {
                    insert(_myBuffer, relacion, clavesBusqueda);
                } else if (opcion == 2) {
                    delet(_myBuffer, relacion, clavesBusqueda);
                } else if (opcion == 3) {
                    consultas(_myBuffer, relacion, clavesBusqueda);
                } else if (opcion == 4) {
                    cout << " Saliendo \n ";
                } else {
                    cout << " Ingrese una opción valida! \n ";
                    return;
                }
            } while (opcion != 4);
        }else {cout << " Saliendo \n "; break;}
    }while(op != 2);
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