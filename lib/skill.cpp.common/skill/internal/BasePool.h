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

            virtual void compress(SKilLID *lbpoMap) {
                // create our part of the lbpo map
                SKilLID next = 0;
                for (const auto &p : iterators::TypeHierarchyIterator(this)) {
                    lbpoMap[p.poolOffset()] = next;
                    next += p.staticSize() - p.deletedCount;

                    // reset data chunks
                    for (auto f : p.dataFields)
                        f->resetChunks(lbpoMap[p.poolOffset()], p.cachedSize);
                }

                auto tmp = this->data;
                allocateData();
                auto d = this->data;
                this->data = tmp;
                SKilLID id = 1;
                for (T &i : this->allInTypeOrder()) {
                    if (0 != i.id) {
                        d[id] = &i;
                        i.id = id;
                        id++;
                    }
                }
                delete[] (1 + this->data);
                this->data = d;

                for (auto& p : iterators::TypeHierarchyIterator(this)) {
                    const_cast<AbstractStoragePool&>(p).updateAfterCompress(lbpoMap);
                }
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_BASEPOOL_H
