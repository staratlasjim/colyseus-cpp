//
// Created by Jim Carter personal on 6/26/23.
//

#ifndef COLYSEUSCPP_ARRAYSCHEMA_HPP
#define COLYSEUSCPP_ARRAYSCHEMA_HPP

#include <iostream>

namespace colyseus {
    namespace schema {

        template <typename T>
        class ArraySchema
        {
        public:
            ArraySchema() {}
            ~ArraySchema() {
                std::cout << "ArraySchema destructor!" << std::endl;
            }

            std::vector<T> items;

            std::function<void(ArraySchema<T>*, T, int)> onAdd;
            std::function<void(ArraySchema<T>*, T, int)> onChange;
            std::function<void(ArraySchema<T>*, T, int)> onRemove;

            inline T &operator[](const int &index) { return items[index]; }
            inline T at(const int &index) { return items[index]; }

            ArraySchema<T> clone();

            inline void setAt(int index, const T& value) {
                if (items.size() == index) {
                    items.push_back(value);
                }
                else {
                    items[index] = value;
                }
            }

            inline bool has(int index)
            {
                return items.size() > index;
            }

            inline int size()
            {
                return items.size();
            }
        };

    } // colysues
} // schema

#endif //COLYSEUSCPP_ARRAYSCHEMA_HPP
