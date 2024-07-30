#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "BPlusTree.cpp"

int search_number(const std::string& str, size_t pos) {
    int number = 0;
    bool flag = false;
    while (pos > 0 && !flag) {
        --pos;
        if (std::isdigit(str[pos])) {
            size_t end_pos = pos;
            while (pos > 0 && std::isdigit(str[pos - 1])) {
                --pos;
            }
            number = std::stoi(str.substr(pos, end_pos - pos + 1));
            flag = true;
        }
    }
    return number;
}

void extract_number(const std::string leaf_data, int& num1, int& num2) {
    char ignore;
    std::stringstream ss(leaf_data);
    ss >> ignore >> num1 >> ignore >> num2 >> ignore;
}

std::vector<std::tuple<int, int, int>> get_leaf_to_bplustree(const std::string& str) {
    std::vector<std::tuple<int, int, int>> result;
    std::fstream file(str, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error: open file failed." << std::endl;
        return result;
    }
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = 0;
        while ((pos = line.find("Ruta:", pos)) != std::string::npos) {  // lo config a while
            size_t pos2 = line.find(')', pos);
            if (pos2 == std::string::npos) break;
            std::string leaf_data = line.substr(pos + 6, pos2 - (pos + 6) + 1);
            int num1, num2;
            extract_number(leaf_data, num1, num2);
            int number = search_number(line, pos);
            result.push_back(std::make_tuple(number, num1, num2));
            pos = pos2 + 1;
        }
    }
    return result;
}

int main(int argc, char const* argv[]) {
    BPlusTree<int> bpt("INDICES/Post_userid.txt");
    // bpt.print();
    bpt.exportToDotFromPrint("INDICES/Post_userid.dot");
    // BPlusTree<int> bpt(4, "Post", "userid");
    // std::vector<std::tuple<int, int, int>> leaf = get_leaf_to_bplustree("INDICES/Post_userid.txt");
    // for (const auto& dato : leaf)
    //     bpt.set(std::get<0>(dato), std::make_pair(std::get<1>(dato), std::get<2>(dato)));
    // bpt.print();
    // // Node<int>* new_node = bpt.findLeaf(15);
    // // if (!new_node) std::cout << "existence" << std::endl;
    // std::pair<int, int> result = bpt.getRuta(15);
    // std::cout << "Ruta: " << result.first << " " << result.second << std::endl;

    // std::string proof = "condicion = 1";
    // std::stringstream ss(proof);
    // std::string dato_condicion;
    // getline(ss, dato_condicion, ' ');
    // getline(ss, dato_condicion, ' ');
    // getline(ss, dato_condicion, ' ');
    // std::cout << dato_condicion << std::endl;
    return 0;
}
