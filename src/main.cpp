#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

#include "../lib/json.hpp"
using json = nlohmann::json;


json decode_bencoded_string(const std::string &encoded_string, size_t &pos) {
    size_t colon_index = encoded_string.find(':', pos);
    if (colon_index == std::string::npos)
        return NULL;
    std::string number_string = encoded_string.substr(pos, colon_index);
    int64_t number = std::atoll(number_string.c_str());
    pos = colon_index + number + 1;
    std::string str = encoded_string.substr(colon_index + 1, number);
    return json(str);
}

json decode_bencoded_integer(const std::string &encoded_integer, size_t &pos) {
    size_t e_index = encoded_integer.find('e', pos);
    if (e_index == std::string::npos)
        return NULL;
    std::string number_string = encoded_integer.substr(pos + 1, e_index);
    int64_t number = std::atoll(number_string.c_str());
    pos = e_index + 1;
    return json(number);
}

json decode_bencoded_value(const std::string &encoded_value, size_t &pos) {
    if (encoded_value[pos] == 'i')
        return decode_bencoded_integer(encoded_value, pos);
    else if (std::isdigit(encoded_value[pos]))
        return decode_bencoded_string(encoded_value, pos);
    std::cout << "ERROR!";
    return NULL;
}

json decode_bencoded_list(const std::string &encoded_list, size_t &pos) {
    pos++;
    auto json_list = json::array();
    while (encoded_list[pos] != 'e' || pos > encoded_list.size())
        json_list.push_back(decode_bencoded_value(encoded_list, pos));
    pos++;
    return json_list;
}

json decode_bencoded_dictionary(const std::string &encoded_dictionary, size_t &pos) {
    pos++;
    auto json_dictionary = nlohmann::ordered_map<json, json>();
    while (encoded_dictionary[pos] != 'e') {
      auto key = decode_bencoded_value(encoded_dictionary, pos);
      auto val = decode_bencoded_value(encoded_dictionary, pos);
      json_dictionary.push_back({key, val});
    }
    pos++;
    return json(json_dictionary);
    // return std::make_tuple(json_dictionary, rest.substr(pos + 1));
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }
    
    std::string command = argv[1];

    if (command == "decode") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
            return 1;
        }

        std::string encoded_value = argv[2];
        size_t i = 0;
        json decoded_value = decode_bencoded_list(encoded_value, i);
        std::cout << decoded_value.dump() << std::endl;
    } else {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}
