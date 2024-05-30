#include <iostream>
#include <string>
#include <fstream>
// #include <vector>
// #include <cctype>
// #include <cstdlib>

#include "../lib/json.hpp"
#include "../lib/bencode_parser.hpp"

using json = nlohmann::json;


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> <encoded_value>" << std::endl;
        return 1;
    }
    
    std::string command = argv[1];

    if (command == "decode") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }

        std::string encoded_value = argv[2];
        json data = decode_bencode(encoded_value);
        std::cout << data.dump() << std::endl;
    } else if (command == "info") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " info <.torrent file>" << std::endl;
            return 1;
        }

        std::string file_name = argv[2];
        std::ifstream file(file_name);
        if (!file.good()) {
            std::cerr << "File is not valid" << std::endl;
            return 1;
        }

        std::string encoded_value;
        std::getline(file, encoded_value);
        file.close();

        json data = decode_bencode(encoded_value);
        std::cout << data.dump() << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
