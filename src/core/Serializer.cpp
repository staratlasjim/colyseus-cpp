//
// Created by Jim Carter personal on 6/22/23.
//
#include "Serializer.hpp"

std::map<std::string, std::shared_ptr<void>> &getSerializers() {
    static std::map<std::string, std::shared_ptr<void>> serializers;
    return serializers;
}

void registerSerializer(const std::string &id, std::shared_ptr<void> serializer) {
    getSerializers()[id] = serializer;
}

std::shared_ptr<void> getSerializer(const std::string &id) {
    auto& serializers = getSerializers();
    auto it = serializers.find(id);
    if (it != serializers.end()) {
        return it->second;
    }
    else {
        return nullptr;
    }
}
