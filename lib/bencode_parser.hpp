#pragma once
#include <iostream>
#include <string>
// #include <cctype>
// #include <cstdlib>

#include "../lib/json.hpp"
using json = nlohmann::json;
json decode_bencoded_value(const std::string &encoded_value, size_t &pos);

inline json decode_bencoded_string(const std::string &encoded_string, size_t &pos) {
    size_t colon_index = encoded_string.find(':', pos);
    if (colon_index == std::string::npos) {
        std::cerr << "Malformed Bencode: String: Colon is not found.\n";
        exit(1);
    }
    std::string number_string = encoded_string.substr(pos, colon_index - pos);
    int64_t number = std::atoll(number_string.c_str());
    if (number < 0 || colon_index + 1 + number > encoded_string.size()) {
        std::cerr << "Malformed Bencode: String: Length exceeds input size.\n";
        exit(1);
    }
    const char* binary_data = encoded_string.data() + colon_index + 1;
    size_t binary_size = static_cast<size_t>(number);
    pos = colon_index + 1 + binary_size;
    std::vector<std::uint8_t> binary_vec(binary_data, binary_data + binary_size);
    return json::binary_t(binary_vec);
}

inline json decode_bencoded_integer(const std::string &encoded_integer, size_t &pos) {
    size_t e_index = encoded_integer.find('e', pos);
    if (e_index == std::string::npos) {
        std::cerr << "Malformed Bencode: Integer: End ('e') is not found.\n";
        exit(1);
    }
    std::string number_string = encoded_integer.substr(pos + 1, e_index - pos - 1);
    uint64_t number = std::atoll(number_string.c_str());
    pos = e_index + 1;
    return json(number);
}

inline json decode_bencoded_list(const std::string &encoded_list, size_t &pos) {
    pos++;
    auto json_list = json::array();
    while (pos < encoded_list.size() && encoded_list[pos] != 'e') {
        json_list.push_back(decode_bencoded_value(encoded_list, pos));
    }
    if (pos >= encoded_list.size()) {
        std::cerr << "Malformed Bencode: Unterminated list.\n";
        exit(1);
    }
    pos++;
    return json_list;
}

inline json decode_bencoded_dictionary(const std::string &encoded_dictionary, size_t &pos) {
    pos++;
    auto json_dictionary = json::object(); // Use nlohmann::json object type for dictionaries
    while (pos < encoded_dictionary.size() && encoded_dictionary[pos] != 'e') {
        json key = decode_bencoded_value(encoded_dictionary, pos);
        if (key.is_binary()) {
            auto binary_data = key.get_binary();
            key = std::string(binary_data.begin(), binary_data.end());
        }
        if (!key.is_string()) {
            std::cerr << "Malformed Bencode: Dictionary keys must be strings.\n";
            exit(1);
        }
        json val = decode_bencoded_value(encoded_dictionary, pos);
        json_dictionary[key.get<std::string>()] = val;
    }
    if (pos >= encoded_dictionary.size()) {
        std::cerr << "Malformed Bencode: Unexpected end of input in dictionary.\n";
    }
    pos++;
    return json_dictionary;
}

inline json decode_bencoded_value(const std::string &encoded_value, size_t &pos) {
    if (pos >= encoded_value.size()) {
        std::cerr << "Malformed Bencode: Unexpected end of input.\n";
        exit(1);
    }
    if (encoded_value[pos] == 'i')
        return decode_bencoded_integer(encoded_value, pos);
    else if (std::iswdigit(encoded_value[pos]))
        return decode_bencoded_string(encoded_value, pos);
    else if (encoded_value[pos] == 'l')
        return decode_bencoded_list(encoded_value, pos);
    else if (encoded_value[pos] == 'd')
        return decode_bencoded_dictionary(encoded_value, pos);
    else if (pos > encoded_value.size())
        return nullptr;
    else {
        for (int i = -5; i < 5; i++)
            std::cerr << encoded_value[pos + i];
        std::cerr << '\n';
    }
    return nullptr;
}

inline json decode_bencode(const std::string &encoded_string) {
    size_t pos = 0;
    return decode_bencoded_value(encoded_string, pos);
}
