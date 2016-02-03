//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_SUBPOOL_H
#define SKILL_CPP_COMMON_SUBPOOL_H

#include "StoragePool.h"
#include "BasePool.h"

namespace skill {
    namespace internal {
        template<class T, class B>
        class SubPool : public StoragePool<T, B> {

        protected:
            virtual void allocateData() {
                this->data = (T **) ((BasePool<B> *) this->basePool)->data;
            }

        public:
            SubPool(TypeID typeID, AbstractStoragePool *superPool, const api::string_t *name,
                    std::set<TypeRestriction *> *restrictions)
                    : StoragePool<T, B>(typeID, superPool, name, restrictions) { }

            //! must not be called
            virtual void compress(SKilLID *lbpoMap) { assert(false); }
        };
    }
}


#endif //SKILL_CPP_COMMON_SUBPOOL_H
