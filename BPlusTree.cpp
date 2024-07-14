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
    string indice;
    int maxCapacity;
    int minCapacity;
    int depth;

    BPlusTree(int _maxCapacity = 4) {
      root = new Node<T>(nullptr, true, nullptr, nullptr);
      maxCapacity = _maxCapacity;
      minCapacity = maxCapacity / 2;
      depth = 0;
    }

    Node<T> *findLeaf(T key) {
      Node<T> *node = root;
      while (!node->isLeaf) node = node->getChild(key);
      return node;
    }

    int get(int key) { return findLeaf(key)->get(key); }

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
        node->values.erase(node->values.begin() + index);
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
        node->values.push_back(next->values.front());
        next->values.erase(next->values.begin());
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
        node->values.insert(node->values.begin(), prev->values.back());
        prev->values.erase(prev->values.end() - 1);
        for (int i = 0; i < node->parent->children.size(); i++) {
            if (node->parent->children[i] == node) {
                node->parent->keys[i - 1] = node->keys.front();
                break;
            }
        }
    }

    void mergeNodeWithRightLeaf(Node<T> *node, Node<T> *next) {
        node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
        node->values.insert(node->values.end(), next->values.begin(), next->values.end());
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
        prev->values.insert(prev->values.end(), node->values.begin(), node->values.end());
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

    void borrowKeyFromRightInternal(int myPositionInParent, Node<T> *node, Node<T> *next) {
        node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
        node->parent->keys[myPositionInParent] = next->keys.front();
        next->keys.erase(next->keys.begin());
        node->children.insert(node->children.end(), next->children.front());
        next->children.erase(next->children.begin());
        node->children.back()->parent = node;
    }

    void borrowKeyFromLeftInternal(int myPositionInParent, Node<T> *node, Node<T> *prev) {
        node->keys.insert(node->keys.begin(), node->parent->keys[myPositionInParent - 1]);
        node->parent->keys[myPositionInParent - 1] = prev->keys.back();
        prev->keys.erase(prev->keys.end() - 1);
        node->children.insert(node->children.begin(), prev->children.back());
        prev->children.erase(prev->children.end() - 1);
        node->children.front()->parent = node;
    }

    void mergeNodeWithRightInternal(int myPositionInParent, Node<T> *node, Node<T> *next) {
        node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
        node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent);
        node->parent->children.erase(node->parent->children.begin() + myPositionInParent + 1);
        node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
        node->children.insert(node->children.end(), next->children.begin(), next->children.end());
        for (Node<T> *child : node->children) {
            child->parent = node;
        }
    }
    void mergeNodeWithLeftInternal(int myPositionInParent, Node<T> *node, Node<T> *prev) {
        prev->keys.insert(prev->keys.end(), node->parent->keys[myPositionInParent - 1]);
        node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent - 1);
        node->parent->children.erase(node->parent->children.begin() + myPositionInParent);
        prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
        prev->children.insert(prev->children.end(), node->children.begin(), node->children.end());
        for (Node<T> *child : prev->children) {
            child->parent = prev;
        }
    }

    void remove(int key, Node<T> *node = nullptr) {
        if (node == nullptr) {
            node = findLeaf(key);
        }
        if (node->isLeaf) {
            removeFromLeaf(key, node);
        } else {
            removeFromInternal(key, node);
        }

        if (node->keys.size() < minCapacity) {
            if (node == root) {
                if (root->keys.empty() && !root->children.empty()) {
                    root = root->children[0];
                    root->parent = nullptr;
                    depth -= 1;
                }
                return;
            } else if (node->isLeaf) {
                Node<T> *next = node->next;
                Node<T> *prev = node->prev;

                if (next && next->parent == node->parent && next->keys.size() > minCapacity) {
                    borrowKeyFromRightLeaf(node, next);
                } else if (prev && prev->parent == node->parent && prev->keys.size() > minCapacity) {
                    borrowKeyFromLeftLeaf(node, prev);
                } else if (next && next->parent == node->parent && next->keys.size() <= minCapacity) {
                    mergeNodeWithRightLeaf(node, next);
                } else if (prev && prev->parent == node->parent && prev->keys.size() <= minCapacity) {
                    mergeNodeWithLeftLeaf(node, prev);
                }
            } else {
                int myPositionInParent = -1;
                for (int i = 0; i < node->parent->children.size(); i++) {
                    if (node->parent->children[i] == node) {
                        myPositionInParent = i;
                        break;
                    }
                }

                Node<T> *next;
                Node<T> *prev;

                if (node->parent->children.size() > myPositionInParent + 1) {
                    next = node->parent->children[myPositionInParent + 1];
                } else {
                    next = nullptr;
                }

                if (myPositionInParent) {
                    prev = node->parent->children[myPositionInParent - 1];
                } else {
                    prev = nullptr;
                }

                if (next && next->parent == node->parent && next->keys.size() > minCapacity) {
                    borrowKeyFromRightInternal(myPositionInParent, node, next);
                } else if (prev && prev->parent == node->parent && prev->keys.size() > minCapacity) {
                    borrowKeyFromLeftInternal(myPositionInParent, node, prev);
                } else if (next && next->parent == node->parent && next->keys.size() <= minCapacity) {
                    mergeNodeWithRightInternal(myPositionInParent, node, next);
                } else if (prev && prev->parent == node->parent && prev->keys.size() <= minCapacity) {
                    mergeNodeWithLeftInternal(myPositionInParent, node, prev);
                }
            }
        }
        if (node->parent) {
            remove(key, node->parent);
        }
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

// int main() {
//   BPlusTree<string> tree(4);
//   vector<int> random_list = {10, 27, 29, 17, 25, 21, 15, 31, 13, 51, 20, 24, 48, 19, 60, 35, 66};
//   vector<string> random_list_strings = {"Hola", "Pepito", "Ayuda", "Suena", "One Piece", "Ciencia de la Computación"};  

//   for (string i : random_list_strings) {
//     cout << endl
//          << "-------------" << endl;
//     cout << "Inserting " << i << endl
//          << endl;
//     cout << "-------------" << endl
//          << endl;

//     tree.set(i);
//     tree.print();
//   }

//   cout << endl
//        << "-------------------------" << endl;
//   cout << "All keys are inserted ..." << endl;
//   cout << "-------------------------" << endl
//        << endl;

//   tree.print(nullptr, "", false);

//   /*
//   > [25]
//      > [15, 20]
//      |  > [10, 13]
//      |  > [15, 17, 19]
//      |  > [20, 21, 24]
//      > [29, 48]
//         > [25, 27]
//         > [29, 31, 35]
//         > [48, 51, 60, 66]
//   */
//   return 0;
// }
