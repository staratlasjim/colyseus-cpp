#ifndef COLYSEUSTEST_SCHEMASERIALIZER_HPP
#define COLYSEUSTEST_SCHEMASERIALIZER_HPP

#include "Serializer.hpp"

using colyseus::schema::Iterator;

template <typename S>
class SchemaSerializer: public Serializer<S> {
    static_assert(std::is_base_of<Schema, S>::value, "S must be a subclass of Schema");

public:
    inline colyseus::schema::Schema* getStateSchema() {
        return static_cast<colyseus::schema::Schema*>(this->state);
    }

    SchemaSerializer() {
        state = new S();
        it = new colyseus::schema::Iterator();
    }

    ~SchemaSerializer() {
        delete state;
        delete it;
    }

    S* getState() { return state; };

    void setState(unsigned const char* bytes, int offset, int length) {
        it->offset = offset;
        ((colyseus::schema::Schema*)state)->decode(bytes, length, it);
    }

    void patch(unsigned const char* bytes, int offset, int length) {
        it->offset = offset;
        ((colyseus::schema::Schema*)state)->decode(bytes, length, it);
    }

    void handshake(unsigned const char* bytes, Iterator* it, int length) {
        Schema* s = this->getStateSchema();
        s->decode(bytes, length, it);
    }

    void teardown() {}

protected:
    Iterator *it;
    S* state;
};

#endif //COLYSEUSTEST_SCHEMASERIALIZER_HPP
