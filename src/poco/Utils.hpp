//
// Created by Jim Carter personal on 6/20/23.
//

#ifndef COLYSEUSCPP_UTILS_HPP
#define COLYSEUSCPP_UTILS_HPP

#pragma once

#include <string>
#include <algorithm>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <nlohmann/json.hpp>

inline std::string replace(std::string str, const std::string& from, const std::string& to, size_t start_pos = 0) {
    if(str.find(from, start_pos) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
    }
    return str;
}

inline std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

Poco::Dynamic::Var toPoco(const nlohmann::json& j);

Poco::JSON::Object::Ptr toPocoJsonObject(const nlohmann::json& j);


nlohmann::json toNlohmannJson(const Poco::Dynamic::Var& var);

nlohmann::json toNlohmannJson(const Poco::JSON::Object::Ptr& pObj);

#endif //COLYSEUSCPP_UTILS_HPP
