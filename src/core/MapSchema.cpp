//
// Created by Jim Carter personal on 6/26/23.
//

#include "MapSchema.hpp"

namespace colyseus {
    namespace schema {
        template<typename T>
        MapSchema<T> MapSchema<T>::clone() {
            MapSchema<T> cloned;
            cloned.items = this->items;
            cloned.onAdd = this->onAdd;
            cloned.onRemove = this->onRemove;
            cloned.onChange = this->onChange;
            return cloned;
        }
    } // colyseus
} // schema