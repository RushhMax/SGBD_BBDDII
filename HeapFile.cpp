#include <iostream>
#include <string>
#include <vector>
#include <sstream> // para mis stringsteam
#include <fstream>
#include <algorithm> // Para std::remove_if
#include <cctype>    // Para std::isspace
#include <iomanip> // Para std::setw

using namespace std;

/// HEAP FILE ////////////////
pair<int,int> getBloques(string _relacion){
    // User#userid#int#3#nickname#varchar#8_11#13
    string aux;
    std::ifstream diccionario("diccionario.txt"); // Abrimos diccionario
    while(getline(diccionario, aux)){
        stringstream relacion(aux);
        getline(relacion, aux, '#');
        if(aux == _relacion){
            getline(relacion, aux, '_');
            getline(relacion, aux, '#');
            int primerBloqueAsignado = stoi(aux); 
            getline(relacion, aux, '#');
            int ultimoBloqueAsignado = stoi(aux); 
            diccionario.close();
            return make_pair(primerBloqueAsignado, ultimoBloqueAsignado);
        }
    }
    diccionario.close();
    return make_pair(0,0); // No encontro
}

vector<pair<int,int>> getCapacidadLibreBloques(int inicio, int fin){
    ifstream directorio( "dirBloques.txt");
    vector<pair<int,int>> capacidadesLibre;
    int cont=0;
    string aux;
    for(int i=1; i<inicio; i++){getline(directorio, aux);}
    for(int i=inicio; i<=fin; i++){
        getline(directorio,aux);
        stringstream bloque(aux);
        getline(bloque, aux, '#');
        capacidadesLibre.push_back(make_pair(i,stoi(aux)));
    }
    return capacidadesLibre;
}

struct HeapFile{
    string nombreRelacion;
    pair<int,int> bloquesAsignados; // inicio - fin
    vector<pair<int,int>> Bloque_espacioLibre;  // Tamaño (fin-inicio) // BLOQUE - ESPACIO

    HeapFile(string _nombreRelacion):nombreRelacion(_nombreRelacion) {
        bloquesAsignados = getBloques(_nombreRelacion);
        Bloque_espacioLibre = getCapacidadLibreBloques(bloquesAsignados.first, bloquesAsignados.second);
    }

    void editCapacidad(pair<int,int> _Bloque_espacioLibre){
        for(int i=0; i<Bloque_espacioLibre.size(); i++){
            cout<<"BLOQUE > "<<Bloque_espacioLibre[i].first<<" - "<<Bloque_espacioLibre[i].second<<endl;
            if(Bloque_espacioLibre[i].first == _Bloque_espacioLibre.first ){
                Bloque_espacioLibre[i].second = _Bloque_espacioLibre.second;
                return;
            }
        }
    }

    int getNewFree(){
        int max = 0;
        for(int i=1; i<Bloque_espacioLibre.size(); i++){
            if(Bloque_espacioLibre[i].second > Bloque_espacioLibre[max].second) max = i;
        }  
        cout<<" ENCONTRE AL MAXIMO "<<max<<" - "<<Bloque_espacioLibre[max].first<<endl;
        return Bloque_espacioLibre[max].first;
    }

    void print(){
        cout<<" NOMBRE DE HEAP FILE "<<nombreRelacion<<endl;
        for(int i=0; i<Bloque_espacioLibre.size(); i++){
            cout<<"BLOQUE > "<<Bloque_espacioLibre[i].first<<" - "<<Bloque_espacioLibre[i].second<<endl;
        }
    }
};