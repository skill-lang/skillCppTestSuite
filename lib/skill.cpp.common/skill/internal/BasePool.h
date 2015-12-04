//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_BASEPOOL_H
#define SKILL_CPP_COMMON_BASEPOOL_H

#include "StoragePool.h"
#include "../restrictions/TypeRestriction.h"

namespace skill {
    using restrictions::TypeRestriction;
    namespace internal {
        template<typename T>
        class BasePool : public StoragePool<T, T> {

        protected:
            virtual void allocateData() {
                this->data = new T *[this->cachedSize] - 1;
            }

            virtual ~BasePool() {
                if (this->data)
                    delete[] (1 + this->data);
            }

            BasePool(TypeID typeID, const api::string_t *name, std::set<TypeRestriction *> *restrictions)
                    : StoragePool<T, T>(typeID, nullptr, name, restrictions) { }
        };
    }
}


#endif //SKILL_CPP_COMMON_BASEPOOL_H
