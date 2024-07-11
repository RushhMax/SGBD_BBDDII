#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <algorithm> // Necesario para std::shuffle

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

    void set(T key) {
      int i = indexOfChild(key);
      if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.insert(keys.begin() + i, key);
      } 
    }

    tuple<T, Node *, Node *> splitLeaf() {
      Node *left = new Node(parent, true, prev, this);
      int mid = keys.size() / 2;

      left->keys = vector<T>(keys.begin(), keys.begin() + mid);

      keys.erase(keys.begin(), keys.begin() + mid);

      return make_tuple(keys[0], left, this);
    }
};

template <typename T>
class BPlusTree {
  public:
    Node<T> *root;
    int maxCapacity;

    BPlusTree(int _maxCapacity = 4) {
      root = new Node<T>(nullptr, true, nullptr, nullptr);
      maxCapacity = _maxCapacity;
    }

    Node<T> *findLeaf(T key) {
      Node<T> *node = root;
      while (!node->isLeaf) node = node->getChild(key);
      return node;
    }

    void set(T key) {
      Node<T> *leaf = findLeaf(key);
      leaf->set(key);
      if (leaf->keys.size() > maxCapacity) insert(leaf->splitLeaf());
    }

    void insert(tuple<T, Node<T> *, Node<T> *> result) {
      T key = std::get<0>(result);
      Node<T> *left = std::get<1>(result);
      Node<T> *right = std::get<2>(result);

      Node<T> *parent = right->parent;
      
      if (parent == nullptr) {
        left->parent = right->parent = root = new Node<T>(nullptr, false, nullptr, nullptr);
        root->keys = {key};
        root->children = {left, right};
        return;
      }
      parent->setChild(key, {left, right});
      if (parent->keys.size() > maxCapacity) insert(parent->splitInternal());
    }

    void print(Node<T> *node = nullptr, string _prefix = "", bool _last = true) {
      if (!node) node = root;
      cout << _prefix << "> [";
      for (int i = 0; i < node->keys.size(); i++) {
        cout << " " << node->keys[i] << " - ";
      }
      cout << "]" << endl;

      _prefix += _last ? "   " : "|  ";

      if (!node->isLeaf) {
        for (int i = 0; i < node->children.size(); i++) {
          bool _last = (i == node->children.size() - 1);
          print(node->children[i], _prefix, _last);
        }
      }
    }
};

int main() {
  BPlusTree<string> tree(4);
  vector<int> random_list = {10, 27, 29, 17, 25, 21, 15, 31, 13, 51, 20, 24, 48, 19, 60, 35, 66};
  vector<string> random_list_strings = {"Hola", "Pepito", "Ayuda", "Suena", "One Piece", "Ciencia de la Computación"};  

  for (string i : random_list_strings) {
    cout << endl
         << "-------------" << endl;
    cout << "Inserting " << i << endl
         << endl;
    cout << "-------------" << endl
         << endl;

    tree.set(i);
    tree.print();
  }

  cout << endl
       << "-------------------------" << endl;
  cout << "All keys are inserted ..." << endl;
  cout << "-------------------------" << endl
       << endl;

  tree.print(nullptr, "", false);

  /*
  > [25]
     > [15, 20]
     |  > [10, 13]
     |  > [15, 17, 19]
     |  > [20, 21, 24]
     > [29, 48]
        > [25, 27]
        > [29, 31, 35]
        > [48, 51, 60, 66]
  */
  return 0;
}
