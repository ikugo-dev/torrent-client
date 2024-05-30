#pragma once
#include <iostream>
#include <string>
// #include <cctype>
// #include <cstdlib>

#include "../lib/json.hpp"
using json = nlohmann::json;
json decode_bencoded_value(const std::string &encoded_value, size_t &pos);

json decode_bencoded_string(const std::string &encoded_string, size_t &pos) {
    size_t colon_index = encoded_string.find(':', pos);
    if (colon_index == std::string::npos)
        return NULL;
    std::string number_string = encoded_string.substr(pos, colon_index-pos);
    int64_t number = std::atoll(number_string.c_str());
    pos = colon_index + number + 1;
    std::string str = encoded_string.substr(colon_index + 1, number);
    return json(str);
}

json decode_bencoded_integer(const std::string &encoded_integer, size_t &pos) {
    size_t e_index = encoded_integer.find('e', pos);
    if (e_index == std::string::npos)
        return NULL;
    std::string number_string = encoded_integer.substr(pos + 1, e_index - pos - 1);
    int64_t number = std::atoll(number_string.c_str());
    pos = e_index + 1;
    return json(number);
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

json decode_bencoded_value(const std::string &encoded_value, size_t &pos) {
    if (encoded_value[pos] == 'i')
        return decode_bencoded_integer(encoded_value, pos);
    else if (std::iswdigit(encoded_value[pos]))
        return decode_bencoded_string(encoded_value, pos);
    else if (encoded_value[pos] == 'l')
        return decode_bencoded_list(encoded_value, pos);
    else if (encoded_value[pos] == 'd')
        return decode_bencoded_dictionary(encoded_value, pos);
    else
        std::cerr << "ERROR:\n" << pos << "\n";
    return NULL;
}

json decode_bencode(const std::string &encoded_dictionary) {
    json json_final = json();
    size_t pos = 0;
    while (pos < encoded_dictionary.size()) {
        json current_json = decode_bencoded_value(encoded_dictionary, pos);
        if (current_json == NULL)
            break;
        json_final.push_back(current_json);
    }
    return json_final;
}