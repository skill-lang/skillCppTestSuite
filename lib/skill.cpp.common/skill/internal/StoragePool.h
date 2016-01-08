//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STORAGEPOOL_H
#define SKILL_CPP_COMMON_STORAGEPOOL_H

#include "AbstractStoragePool.h"
#include "Book.h"
#include "../restrictions/TypeRestriction.h"
#include "../iterators/TypeOrderIterator.h"
#include "../iterators/DynamicDataIterator.h"

namespace skill {
    namespace iterators {
        template<class T, class B>
        class StaticDataIterator;

        template<class T, class B>
        class DynamicDataIterator;
    }

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
            friend class AbstractStoragePool;

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
                T *page = book->firstPage();
                int idx = 0;
                for (const auto &b : blocks) {
                    SKilLID i = b.bpo + 1;
                    const auto last = i + b.staticCount;
                    for (; i < last; i++)
                        data[i] = new(page + idx++) T(i);
                }
            }

            /**
             * All stored objects, which have exactly the type T. Objects are stored as arrays of field entries. The types of the
             *  respective fields can be retrieved using the fieldTypes map.
             */
            std::vector<T *> newObjects;

            //! static data iterator can traverse over new objects
            friend class iterators::StaticDataIterator<T, B>;

            //! dynamic data iterator can traverse over new objects
            friend class iterators::DynamicDataIterator<T, B>;

            virtual SKilLID newObjectsSize() const {
                return (SKilLID) newObjects.size();
            }

            StoragePool(TypeID typeID, AbstractStoragePool *superPool,
                        const api::string_t *name, std::set<TypeRestriction *> *restrictions)
                    : AbstractStoragePool(typeID, superPool, name, restrictions),
                      book(nullptr),
                      data(nullptr) { }

            virtual ~StoragePool() {
                if (book)
                    delete book;
            }

        public:

            inline T *get(SKilLID id) const {
                return id <= 0 ? nullptr : data[id];
            }

            virtual api::Object *getAsAnnotation(SKilLID id) const {
                return get(id);
            }

            iterators::StaticDataIterator<T, B> staticInstances() const {
                return iterators::StaticDataIterator<T, B>(this);
            };

            iterators::DynamicDataIterator<T, B> all() const {
                return iterators::DynamicDataIterator<T, B>(this);
            };

            iterators::TypeOrderIterator<T, B> allInTypeOrder() const {
                return iterators::TypeOrderIterator<T, B>(this);
            };

            iterators::DynamicDataIterator<T, B> begin() const {
                return iterators::DynamicDataIterator<T, B>(this);
            };

            const iterators::DynamicDataIterator<T, B> end() const {
                return iterators::DynamicDataIterator<T, B>();
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_STORAGEPOOL_H
