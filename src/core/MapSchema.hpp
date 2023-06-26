//
// Created by Jim Carter personal on 6/26/23.
//

#ifndef COLYSEUSCPP_MAPSCHEMA_HPP
#define COLYSEUSCPP_MAPSCHEMA_HPP

#include <map>
#include "SchemaUtils.hpp"

namespace colyseus {
    namespace schema {

        template <typename T>
        class MapSchema
        {
        public:
            MapSchema() {}
            ~MapSchema() {}

            std::map<string, T> items;

            std::function<void(MapSchema<T> *, T, string)> onAdd;
            std::function<void(MapSchema<T> *, T, string)> onChange;
            std::function<void(MapSchema<T> *, T, string)> onRemove;

            inline T &operator[](const char index[])
            {
                return items[index];
            }

            MapSchema<T> clone();

            inline T at(const string& key)
            {
                return items.at(key);
            }

            inline bool has(const string& field)
            {
                return items.find(field) != items.end();
            }


            inline int size()
            {
                return items.size();
            }
        };



    } // colyseus
} // schema

#endif //COLYSEUSCPP_MAPSCHEMA_HPP
