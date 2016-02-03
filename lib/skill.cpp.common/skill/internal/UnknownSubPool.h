//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_UNKNOWNSUBPOOL_H
#define SKILL_CPP_COMMON_UNKNOWNSUBPOOL_H

#include "SubPool.h"
#include "LazyField.h"

namespace skill {
    namespace internal {
        /**
         * represents sub types not known at generation time
         *
         * @author Timm Felden
         */
        template<class T, class B>
        class UnknownSubPool : public SubPool<T, B> {

            void allocateDeferred();

        public:
            UnknownSubPool(TypeID typeID, AbstractStoragePool *superPool,
                           const api::string_t *name,
                           std::set<TypeRestriction *> *restrictions)
                    : SubPool<T, B>(typeID, superPool, name,
                                    restrictions) { }

            virtual AbstractStoragePool *makeSubPool(TypeID typeID,
                                                     api::String name,
                                                     std::set<TypeRestriction *> *restrictions) {
                return new UnknownSubPool<T, B>(typeID, this, name, restrictions);
            }

            virtual FieldDeclaration *addField(const AbstractStringKeeper *const keeper,
                                               TypeID id, const FieldType *type, api::String name) {
                auto rval = new LazyField(type, name, (SKilLID)this->dataFields.size() + 1, this);
                this->dataFields.push_back(rval);
                return rval;
            }

            virtual void allocateInstances() {
                allocateDeferred();
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_UNKNOWNSUBPOOL_H
