//
// Created by Jim Carter personal on 6/26/23.
//

#include "ArraySchema.hpp"

namespace colyseus {
    namespace schema {
        template<typename T>
        ArraySchema<T> ArraySchema<T>::clone() {
            ArraySchema<T> cloned;
            cloned.items = this->items;
            cloned.onAdd = this->onAdd;
            cloned.onRemove = this->onRemove;
            cloned.onChange = this->onChange;
            return cloned;
        }
    } // colysues
} // schema