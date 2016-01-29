//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_UNKNOWNBASEPOOL_H
#define SKILL_CPP_COMMON_UNKNOWNBASEPOOL_H

#include "AbstractStoragePool.h"
#include "StoragePool.h"
#include "UnknownObject.h"
#include "BasePool.h"
#include "../restrictions/TypeRestriction.h"
#include "UnknownSubPool.h"
#include "UnknownSubPool.implementation.h"

namespace skill {
    using restrictions::TypeRestriction;
    namespace internal {
        /**
         * This class represents unknown base types
         *
         * @author Timm Felden
         */
        class UnknownBasePool : public BasePool<UnknownObject> {

        public:
            UnknownBasePool(TypeID typeID, String name, std::set<TypeRestriction *> *rest)
                    : BasePool<UnknownObject>(typeID, name, rest) { }

            virtual AbstractStoragePool *makeSubPool(skill::TypeID typeID,
                                                     skill::api::String name,
                                                     std::set<TypeRestriction *> *restrictions) {
                return new UnknownSubPool<UnknownObject, UnknownObject>(
                        typeID, this, name, restrictions);
            }

            virtual FieldDeclaration *addField(const AbstractStringKeeper *const keeper,
                                               TypeID id, const FieldType *type, api::String name);

            virtual void allocateInstances();
        };
    }
}


#endif //SKILL_CPP_COMMON_UNKNOWNBASEPOOL_H
