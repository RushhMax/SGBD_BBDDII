#include <iostream>
#include <random>
#include <utility>
#include <fstream>
#include <vector>
#include <algorithm> // Necesario para std::shuffle
#include <sstream> // para mis stringsteam

using namespace std;

template <typename T>
class Node {
  public:
    vector<T> keys;
    Node *parent;
    vector<Node *> children;
    Node *next;
    Node *prev;
    bool isLeaf;
    vector<pair<int, int>> rutas; // Almacenar pares de ruta en las hojas

    Node(Node *parent = nullptr, bool isLeaf = false, Node *prev_ = nullptr, Node *next_ = nullptr) 
        : parent(parent), isLeaf(isLeaf), prev(prev_), next(next_) {
      if (next_) next_->prev = this;
      if (prev_) prev_->next = this;
    }

    int indexOfChild(T key) { 
      for (int i = 0; i < keys.size(); i++) {
        if (key < keys[i]) return i;
      }
      return keys.size();
    }

    int indexOfKey(int key){ // IGUALES A 
      for (int i = 0; i < keys.size(); i++){
        if (key == keys[i]) return i;
      }
      return -1;
    }

    Node *getChild(T key) { return children[indexOfChild(key)]; }

    void setChild(T key, vector<Node *> value) {
      int i = indexOfChild(key);
      keys.insert(keys.begin() + i, key);
      children.erase(children.begin() + i);
      children.insert(children.begin() + i, value.begin(), value.end());
    }

    tuple<T, Node *, Node *> splitInternal() {
      Node *left = new Node(parent, false, nullptr, nullptr);
      int mid = keys.size() / 2;

      copy(keys.begin(), keys.begin() + mid, back_inserter(left->keys));
      copy(children.begin(), children.begin() + mid + 1, back_inserter(left->children));

      for (Node *child : left->children) {
        child->parent = left;
      }

      T key = keys[mid];
      keys.erase(keys.begin(), keys.begin() + mid + 1);
      children.erase(children.begin(), children.begin() + mid + 1);

      return make_tuple(key, left, this);
    }

    void set(T key, pair<int, int> ruta) {
      int i = indexOfChild(key);
      if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.insert(keys.begin() + i, key);
        if (isLeaf) {
          rutas.insert(rutas.begin() + i, ruta); // Insertar ruta en la hoja
        }
      } 
    }

    tuple<T, Node *, Node *> splitLeaf() {
      Node *left = new Node(parent, true, prev, this);
      int mid = keys.size() / 2;

      left->keys = vector<T>(keys.begin(), keys.begin() + mid);
      left->rutas = vector<pair<int, int>>(rutas.begin(), rutas.begin() + mid); // Dividir rutas

      keys.erase(keys.begin(), keys.begin() + mid);
      rutas.erase(rutas.begin(), rutas.begin() + mid); // Borrar rutas divididas

      return make_tuple(keys[0], left, this);
    }
};

template <typename T>
class BPlusTree {
  public:
    Node<T> *root;
    string relacion;
    string claveBusqueda;
    string archivo;
    int maxCapacity;
    int minCapacity;
    int depth;

    BPlusTree(int _maxCapacity = 4, string _relacion = "", string _claveBusqueda = "") {
      root = new Node<T>(nullptr, true, nullptr, nullptr);
      maxCapacity = _maxCapacity;
      minCapacity = maxCapacity / 2;
      claveBusqueda = _claveBusqueda;
      relacion = _relacion;
      archivo = "INDICES/" + _relacion + "_" +_claveBusqueda + ".txt";
      depth = 0;
    }

    BPlusTree(string archivo) { // relacion clave de busqyeda, depth
        ifstream infile(archivo);
        string line;

        // Leer la primera línea del archivo para obtener la relación, clave de búsqueda y capacidad máxima
        getline(infile, line);
        istringstream headerStream(line);
        headerStream >> relacion >> claveBusqueda >> maxCapacity;
        minCapacity = maxCapacity / 2;

        root = nullptr;
        depth = 0;
        vector<Node<T>*> nodeStack;
        vector<int> levelStack;

        while (getline(infile, line)) {
            //if (line.empty()) continue;

            int level = 0;
            while (line[level] == ' ') level++;

            line = line.substr(level); // Remove leading spaces
            line.erase(0, 3); // Remove "> ["

            Node<T>* newNode = new Node<T>(nullptr, line.find('>') == string::npos);
            istringstream iss(line);
            T key;
            while (iss >> key) {
                newNode->keys.push_back(key);
                if (iss.peek() == ' ' && iss.ignore() && iss.peek() == '[') {
                    iss.ignore(7); // Skip "Ruta: ("
                    int first, second;
                    char comma;
                    iss >> first >> comma >> second;
                    iss.ignore(2); // Skip ") "
                    newNode->rutas.push_back(make_pair(first, second));
                }
            }

            if (nodeStack.empty()) {
                root = newNode;
            } else {
                while (!levelStack.empty() && levelStack.back() >= level) {
                    nodeStack.pop_back();
                    levelStack.pop_back();
                }
                Node<T>* parent = nodeStack.back();
                parent->children.push_back(newNode);
                newNode->parent = parent;
            }
            nodeStack.push_back(newNode);
            levelStack.push_back(level);
        }
        infile.close();
    }

    Node<T> *findLeaf(T key) {
      Node<T> *node = root;
      while (!node->isLeaf) node = node->getChild(key);
      return node;
    }

    int get(int key) { return findLeaf(key)->get(key); }

    void set(T key, pair<int, int> ruta) {
      Node<T> *leaf = findLeaf(key);
      leaf->set(key, ruta);
      if (leaf->keys.size() > maxCapacity) insert(leaf->splitLeaf());
    }

    void insert(tuple<T, Node<T> *, Node<T> *> result) {
      T key = std::get<0>(result);
      Node<T> *left = std::get<1>(result);
      Node<T> *right = std::get<2>(result);

      Node<T> *parent = right->parent;
      
      if (parent == nullptr) {
        left->parent = right->parent = root = new Node<T>(nullptr, false, nullptr, nullptr);
        depth += 1;
        root->keys = {key};
        root->children = {left, right};
        return;
      }
      parent->setChild(key, {left, right});
      if (parent->keys.size() > maxCapacity) insert(parent->splitInternal());
    }

    void removeFromLeaf(int key, Node<T> *node) {
        int index = node->indexOfKey(key);
        if (index == -1) {
            cout << "Key " << key << " not found! Exiting ..." << endl;
            exit(0);
        }
        node->keys.erase(node->keys.begin() + index);
        node->rutas.erase(node->rutas.begin() + index);
        if (node->parent) {
            int indexInParent = node->parent->indexOfChild(key);
            if (indexInParent) node->parent->keys[indexInParent - 1] = node->keys.front();
        }
    }

    void removeFromInternal(int key, Node<T> *node) {
        int index = node->indexOfKey(key);
        if (index != -1) {
            Node<T> *leftMostLeaf = node->children[index + 1];
            while (!leftMostLeaf->isLeaf) leftMostLeaf = leftMostLeaf->children.front();
            node->keys[index] = leftMostLeaf->keys.front();
        }
    }

    void borrowKeyFromRightInternal(int myPositionInParent, Node<T> *node, Node<T> *next){
      node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
      node->rutas.insert(node->rutas.end(), node->parent->rutas[myPositionInParent]);
      node->parent->keys[myPositionInParent] = next->keys.front();
      node->parent->rutas[myPositionInParent] = next->rutas.front();
      next->keys.erase(next->keys.begin());
      next->rutas.erase(next->rutas.begin());
      node->children.insert(node->children.end(), next->children.front());
      next->children.erase(next->children.begin());
      node->children.back()->parent = node;
    }

    void borrowKeyFromLeftInternal(int myPositionInParent, Node<T> *node, Node<T> *prev){
      node->keys.insert(node->keys.begin(),node->parent->keys[myPositionInParent - 1]);
      node->rutas.insert(node->rutas.begin(),node->parent->rutas[myPositionInParent - 1]);
      node->parent->keys[myPositionInParent - 1] = prev->keys.back();
      node->parent->rutas[myPositionInParent - 1] = prev->rutas.back();
      prev->keys.erase(prev->keys.end() - 1);
      prev->rutas.erase(prev->rutas.end() - 1);
      node->children.insert(node->children.begin(), prev->children.back());
      prev->children.erase(prev->children.end() - 1);
      node->children.front()->parent = node;
    }

    void borrowKeyFromRightLeaf(Node<T> *node, Node<T> *next) {
        node->keys.push_back(next->keys.front());
        next->keys.erase(next->keys.begin());
        node->rutas.push_back(next->rutas.front());
        next->rutas.erase(next->rutas.begin());
        for (int i = 0; i < node->parent->children.size(); i++) {
            if (node->parent->children[i] == next) {
                node->parent->keys[i - 1] = next->keys.front();
                break;
            }
        }
    }

    void borrowKeyFromLeftLeaf(Node<T> *node, Node<T> *prev) {
        node->keys.insert(node->keys.begin(), prev->keys.back());
        prev->keys.erase(prev->keys.end() - 1);
        node->rutas.insert(node->rutas.begin(), prev->rutas.back());
        prev->rutas.erase(prev->rutas.end() - 1);
        for (int i = 0; i < node->parent->children.size(); i++) {
            if (node->parent->children[i] == node) {
                node->parent->keys[i - 1] = node->keys.front();
                break;
            }
        }
    }

    void mergeNodeWithRightLeaf(Node<T> *node, Node<T> *next) {
        node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
        node->rutas.insert(node->rutas.end(), next->rutas.begin(), next->rutas.end());
        node->next = next->next;
        if (node->next) node->next->prev = node;
        for (int i = 0; i < next->parent->children.size(); i++) {
            if (node->parent->children[i] == next) {
                node->parent->keys.erase(node->parent->keys.begin() + i - 1);
                node->parent->children.erase(node->parent->children.begin() + i);
                break;
            }
        }
    }

    void mergeNodeWithLeftLeaf(Node<T> *node, Node<T> *prev) {
        prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
        prev->rutas.insert(prev->rutas.end(), node->rutas.begin(), node->rutas.end());
        prev->next = node->next;
        if (prev->next) prev->next->prev = prev;
        for (int i = 0; i < node->parent->children.size(); i++) {
            if (node->parent->children[i] == node) {
                node->parent->keys.erase(node->parent->keys.begin() + i - 1);
                node->parent->children.erase(node->parent->children.begin() + i);
                break;
            }
        }
    }

    void mergeNodeWithRightInternal(int myPositionInParent, Node<T> *node, Node<T> *next){
      node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
      node->rutas.insert(node->rutas.end(), node->parent->rutas[myPositionInParent]);
      node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent);
      node->parent->rutas.erase(node->parent->rutas.begin() + myPositionInParent);
      node->parent->children.erase(node->parent->children.begin() + myPositionInParent + 1);
      node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
      node->rutas.insert(node->rutas.end(), next->rutas.begin(), next->rutas.end());
      node->children.insert(node->children.end(), next->children.begin(), next->children.end());
      for (Node<T> *child : node->children){
        child->parent = node;
      }
    }

    void mergeNodeWithLeftInternal(int myPositionInParent, Node<T> *node,Node<T> *prev){
      prev->keys.insert(prev->keys.end(), node->parent->keys[myPositionInParent - 1]);
      prev->rutas.insert(prev->rutas.end(), node->parent->rutas[myPositionInParent - 1]);
      node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent - 1);
      node->parent->rutas.erase(node->parent->rutas.begin() + myPositionInParent - 1);
      node->parent->children.erase(node->parent->children.begin() + myPositionInParent);
      prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
      prev->rutas.insert(prev->rutas.end(), node->rutas.begin(), node->rutas.end());
      prev->children.insert(prev->children.end(), node->children.begin(), node->children.end());
      for (Node<T> *child : prev->children){
        child->parent = prev;
      }
    }

    void remove(int key, Node<T> *node = nullptr){
      if (node == nullptr){ node = findLeaf(key);}
      if (node->isLeaf){ removeFromLeaf(key, node);}
      else{ removeFromInternal(key, node);}

      if (node->keys.size() < minCapacity){
        if (node == root){
          if (root->keys.empty() && !root->children.empty()){
            root = root->children[0];
            root->parent = nullptr;
            depth -= 1;
          }
          return;
        }else if (node->isLeaf){
          Node<T> *next = node->next;
          Node<T> *prev = node->prev;

          if (next && next->parent == node->parent && next->keys.size() > minCapacity){
            borrowKeyFromRightLeaf(node, next);
          }
          else if (prev && prev->parent == node->parent && prev->keys.size() > minCapacity){
            borrowKeyFromLeftLeaf(node, prev);
          }
          else if (next && next->parent == node->parent && next->keys.size() <= minCapacity){
            mergeNodeWithRightLeaf(node, next);
          }
          else if (prev && prev->parent == node->parent && prev->keys.size() <= minCapacity){
            mergeNodeWithLeftLeaf(node, prev);
          }
        }else{
          int myPositionInParent = -1;

          for (int i = 0; i < node->parent->children.size(); i++){
            if (node->parent->children[i] == node){
              myPositionInParent = i;
              break;
            }
          }

          Node<T> *next;
          Node<T> *prev;

          if (node->parent->children.size() > myPositionInParent + 1){
            next = node->parent->children[myPositionInParent + 1];
          }else{
            next = nullptr;
          }

          if (myPositionInParent){
            prev = node->parent->children[myPositionInParent - 1];
          }
          else{
            prev = nullptr;
          }

          if (next && next->parent == node->parent &&
              next->keys.size() > minCapacity){
            borrowKeyFromRightInternal(myPositionInParent, node, next);
          }

          else if (prev && prev->parent == node->parent &&
                  prev->keys.size() > minCapacity){
            borrowKeyFromLeftInternal(myPositionInParent, node, prev);
          }

          else if (next && next->parent == node->parent &&
                  next->keys.size() <= minCapacity){
            mergeNodeWithRightInternal(myPositionInParent, node, next);
          }

          else if (prev && prev->parent == node->parent &&
                  prev->keys.size() <= minCapacity){
            mergeNodeWithLeftInternal(myPositionInParent, node, prev);
          }
        }
      }
      if (node->parent){
        remove(key, node->parent);
      }
    }

    void print(Node<T> *node = nullptr, string _prefix = "", bool _rutas = false, ofstream *outfile = nullptr) {
        if (!node) {
            node = root;
            if (outfile) {
                *outfile << maxCapacity << endl;
            } else {
                cout << maxCapacity << endl;
            }
        }

        string output = _prefix + "> [";
        for (int i = 0; i < node->keys.size(); i++) {
            output += " [ " + to_string(node->keys[i]) + " ] ";
            if (node->isLeaf && _rutas) 
                output += "[Ruta: (" + to_string(node->rutas[i].first) + ", " + to_string(node->rutas[i].second) + ")] ";
        }
        output += "]";


        if (outfile) {
            *outfile << output << endl;
        } else {
            cout << output << endl;
        }

        _prefix += "|  ";

        if (!node->isLeaf) {
            for (int i = 0; i < node->children.size(); i++) {
                //bool _last = (i == node->children.size() - 1);
                print(node->children[i], _prefix, _rutas, outfile);
            }
        }
    }
    void generateDot(Node<T>* node, ofstream *file) {
        if(!node) return;

        string output =  "{\n";
        output += "\"items\" : [";
        for (int i = 0; i < node->keys.size(); i++) {
            if(i == node->keys.size()-1)
                output +=  to_string(node->keys[i]) ;
            else output +=  to_string(node->keys[i]) + ",";
        }
        output += "]";

        
        *file << output ;
        if (!node->isLeaf && node->children.size()>0) {

            *file << ",\n\"children\" : [";
            for (int i = 0; i < node->children.size(); i++) {
                if(i > 0) *file << ",";
                generateDot(node->children[i], file);
            }
            *file << "]\n";
        }
        *file << "}";
    }
    void printToJson(ofstream *file) {
        *file << "{\n";
        *file << "\"q\": "<<maxCapacity<<",\n";
        *file << "\"root\":";
        generateDot(root, file);
        *file << "}\n";
    }
};

// int main() {
//     // BPlusTree<int> tree("INDICES/Post_userid.txt");
//     // tree.print();
//     // return 0;

//   BPlusTree<int> tree(8, "", ""); // d = 7
//   //vector<int> random_list = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; 278
//   vector<int> random_list = {200,150,8,35,230,1,120,50,30,2,300,25,12,5,10,43,3,258,60,129,270,7,38,80,180,280,17,45,110,4,13,6,9,11,31,51,81,91,101};

//   for (int i : random_list){
//     cout << endl
//           << "-------------" << endl;
//     cout << "Inserting " << i << endl
//           << endl;
//       cout << "-------------" << endl
//            << endl;

//       tree.set(i, make_pair (0,0));
//       //tree.print();
//     }


//     // cout<<endl;
//     // int x = 0;
//     // do{
//     //     tree.set(x, make_pair (0,0));
//     //     cout<<" INGRESAR >";cin>>x;
//     // }while(x != -1);
    

//     cout << endl
//          << "-------------------------" << endl;
//     cout << "All keys are inserted ..." << endl;
//     cout << "-------------------------" << endl
//          << endl;

//     tree.print(nullptr,"",false);
//     tree.remove(3);
//     cout << "\n REMOVE 3" << endl;
//     tree.print(nullptr,"",false);
//     tree.remove(1);
//     cout << "\n REMOVE 1" << endl;
//     tree.print(nullptr,"",false);
//     tree.remove(270);
//     cout << "\n REMOVE 270" << endl;
//     tree.print(nullptr,"",false);
//     tree.remove(51);
//     cout << "\n REMOVE 51" << endl;
//     tree.print(nullptr,"",false);
//     tree.remove(230);
//     tree.remove(258);
//     tree.remove(280);
//     tree.remove(300);

//     Node<int>* NodoHoja = tree.findLeaf(8);

//     for(int i=0; i<NodoHoja->keys.size(); i++){
//       cout<<NodoHoja->keys[i]<<" (";
//       cout<<NodoHoja->rutas[i].first<<"-"<<NodoHoja->rutas[i].second<<")\n";
//     }

// tree.print(nullptr,"",false);
//     ofstream archivo_json("INDICES/grafoPrueba.json");
//     tree.printToJson(&archivo_json);
//     archivo_json.close();

//     ofstream outfile("INDICES/grafoPrueba.txt");
//     tree.print(nullptr, "", false, &outfile);
//     outfile.close();
//     system("node INDICES/bplus-graphviz.js INDICES/grafoPrueba.json INDICES/output.dot");

// }
