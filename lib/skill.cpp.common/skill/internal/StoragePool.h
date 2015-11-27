//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STORAGEPOOL_H
#define SKILL_CPP_COMMON_STORAGEPOOL_H

#include "AbstractStoragePool.h"
#include "Book.h"
#include "../restrictions/TypeRestriction.h"

namespace skill {
    using restrictions::TypeRestriction;
    namespace internal {
/**
 * @author Timm Felden
 * @note maybe, we could omit B, but we will keep it, just for the sake of type level
 * verification and architectural compatibility to other implementations
 */
        template<class T, class B>
        class StoragePool : public AbstractStoragePool {

        protected:
            /**
             * allocated when all instances are allocated, because by then, we can now
             * how many instances are to be read from file, which is quite helpful
             */
            Book<T> *book;
            
        public:
            /**
             * @note internal use only!
             *
             * @note the truth would be B*[], but this is not important now
             * @note the pointer is shifted by 1, so that access by id will get the right
             * result
             */
            T **data;

        protected:

            virtual void allocateInstances() {
                book = new Book<T>(staticDataInstances);
                for (const auto &b : blocks) {
                    SKilLID i = b.bpo + 1;
                    const auto last = i + b.staticCount;
                    for (; i < last; i++)
                        data[i] = new(book->next()) T(i + 1);
                }
            }

            /**
             * All stored objects, which have exactly the type T. Objects are stored as arrays of field entries. The types of the
             *  respective fields can be retrieved using the fieldTypes map.
             */
            std::vector<T *> newObjects;

            virtual SKilLID newObjectsSize() const {
                return (SKilLID) newObjects.size();
            }

            StoragePool(TypeID typeID, AbstractStoragePool *superPool,
                        const api::string_t *name, std::set<TypeRestriction *> *restrictions)
                    : AbstractStoragePool(typeID, superPool, name, restrictions) { }


        public:

            T *get(SKilLID id) const {
                return id <= 0 ? nullptr : data[id];
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_STORAGEPOOL_H
