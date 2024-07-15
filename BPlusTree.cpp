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

        // Leer la capacidad máxima del archivo
        getline(infile, line);
        maxCapacity = stoi(line);
        minCapacity = maxCapacity / 2;

        root = nullptr;
        depth = 0;
        vector<Node<T>*> nodeStack;
        vector<int> levelStack;

        while (getline(infile, line)) {
            if (line.empty()) continue;

            int level = 0;
            while (line[level] == ' ') level++;

            line = line.substr(level); // Remove leading spaces
            line.erase(0, 3); // Remove "> ["

            Node<T>* newNode = new Node<T>(nullptr, line.find('>') == string::npos);
            istringstream iss(line);
            T key;
            while (iss >> key) {
                newNode->keys.push_back(key);
                iss.ignore(3, ' '); // Skip " - "
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
        for (int i = 0; i < node->parent->children.size(); i++) {
            if (node->parent->children[i] == node) {
                node->parent->keys[i - 1] = node->keys.front();
                break;
            }
        }
    }

    void mergeNodeWithRightLeaf(Node<T> *node, Node<T> *next) {
        node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
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

    void remove(int key, Node<T> *node = nullptr) {
        if (root == nullptr) return;
        if (node == nullptr) node = root;

        if (node->isLeaf) {
            removeFromLeaf(key, node);
            if (node->keys.size() < minCapacity && node->prev && node->prev->parent == node->parent) {
                if (node->prev->keys.size() > minCapacity) {
                    borrowKeyFromLeftLeaf(node, node->prev);
                } else {
                    mergeNodeWithLeftLeaf(node, node->prev);
                }
            }
            if (node->keys.size() < minCapacity && node->next && node->next->parent == node->parent) {
                if (node->next->keys.size() > minCapacity) {
                    borrowKeyFromRightLeaf(node, node->next);
                } else {
                    mergeNodeWithRightLeaf(node, node->next);
                }
            }
        } else {
            removeFromInternal(key, node);
            for (Node<T> *child : node->children) remove(key, child);
        }

        if (root->keys.empty()) {
            root = root->children[0];
            root->parent = nullptr;
            depth--;
        }
    }

    void print(Node<T> *node = nullptr, string _prefix = "", bool _last = true, ofstream *outfile = nullptr) {
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
            if (node->isLeaf) {}
                // output += "[Ruta: (" + to_string(node->rutas[i].first) + ", " + to_string(node->rutas[i].second) + ")] ";
        }
        output += "]";


        if (outfile) {
            *outfile << output << endl;
        } else {
            cout << output << endl;
        }

        _prefix += _last ? "   " : "|  ";

        if (!node->isLeaf) {
            for (int i = 0; i < node->children.size(); i++) {
                bool _last = (i == node->children.size() - 1);
                print(node->children[i], _prefix, _last, outfile);
            }
        }
    }
};

// int main() {
//   BPlusTree<int> tree(7, "", "");
//   vector<int> random_list = {100,75,1,99,30,80,3};

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

    
//     cout<<endl;
//     int x;
//     cout<<" INGRESAR >";cin>>x;
//     do{
//         tree.set(x, make_pair (0,0));
//         cout<<" INGRESAR >";cin>>x;
//     }while(x != -1);
    

//     cout << endl
//          << "-------------------------" << endl;
//     cout << "All keys are inserted ..." << endl;
//     cout << "-------------------------" << endl
//          << endl;

//     tree.print(nullptr,"",false);


  

// //   // Insertar claves con sus rutas
// //   tree.set(1, {1, 2});
// //   tree.set(2, {2, 3});
// //   tree.set(3, {3, 4});
// //   tree.set(4, {4, 5});
// //   tree.set(5, {5, 6});

// //   // Imprimir el árbol
// //   tree.print();
// //   cout << endl;

//   return 0;
// }
