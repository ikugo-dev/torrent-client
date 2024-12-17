#include <fstream>
#include <iostream>
#include <string>
// #include <vector>
// #include <cctype>
// #include <cstdlib>

#include "../lib/bencode_parser.hpp"
#include "../lib/json.hpp"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> <encoded_value>"
                  << '\n';
        return 1;
    }

    std::string command = argv[1];
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0];
        if (command == "decode")
            std::cerr << " decode <encoded_value>\n";
        else if (command == "info")
            std::cerr << " info <.torrent_file>\n";
        return 1;
    }

    if (command == "decode") {
        std::string encoded_value = argv[2];
        json data = decode_bencode(encoded_value);
        std::cout << data.dump() << '\n';
    } else if (command == "info") {
        std::string file_name = argv[2];
        std::ifstream torrent_file(file_name);
        if (!torrent_file.good()) {
            std::cerr << "File is not valid" << '\n';
            return 1;
        }

        std::string encoded_value;
        std::getline(torrent_file, encoded_value);
        torrent_file.close();

        json data = decode_bencode(encoded_value);

        std::cout << "Tracker URL: " << data.at("announce");
        std::cout << "\nLength: " << data.at("info").at("length");
        std::cout << "\nLength: " << data.at("info").at("length");
        std::cout << "\n\n\n" << data.dump() << std::endl;
    } else {
        std::cerr << "unknown command: " << command << '\n';
        return 1;
    }

    return 0;
}
