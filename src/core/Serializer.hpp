//
// Created by Jim Carter personal on 5/31/23.
//

#ifndef COLYSEUSTEST_SERIALIZER_HPP
#define COLYSEUSTEST_SERIALIZER_HPP

#include <map>
#include <string>
#include <memory>

#include "schema.h"
using colyseus::schema::Schema;
using colyseus::schema::Iterator;

template <typename S>
class Serializer
{
    static_assert(std::is_base_of<Schema, S>::value, "S must be a subclass of Schema");
public:
//    Serializer();
//    virtual ~Serializer();

    virtual S* getState() = 0;
    virtual void setState(unsigned const char* bytes, int offset, int length) = 0;
    virtual void patch(unsigned const char* bytes, int offset, int length) = 0;
    virtual void teardown() = 0;
    virtual void handshake(unsigned const char* bytes, Iterator* it, int length) = 0;

protected:
};

// Implementations of standalone functions
template <typename S>
using SerializerFactory = std::function<std::shared_ptr<Serializer<S>>()>;

std::map<std::string, std::shared_ptr<void>>& getSerializers() {
    static std::map<std::string, std::shared_ptr<void>> serializers;
    return serializers;
}

void registerSerializer(const std::string& id, std::shared_ptr<void> serializer) {
    getSerializers()[id] = serializer;
}

std::shared_ptr<void> getSerializer(const std::string& id) {
    auto& serializers = getSerializers();
    auto it = serializers.find(id);
    if (it != serializers.end()) {
        return it->second;
    }
    else {
        return nullptr;
    }
}






#endif //COLYSEUSTEST_SERIALIZER_HPP
