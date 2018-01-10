//
// Created by Timm Felden on 28.12.15.
//

#ifndef SKILL_CPP_COMMON_TYPE_ORDER_ITERATOR_H
#define SKILL_CPP_COMMON_TYPE_ORDER_ITERATOR_H

#include <iterator>
#include "StaticDataIterator.h"
#include "TypeHierarchyIterator.h"


namespace skill {
    namespace internal {
        template<class T, class B>
        class StoragePool;
    }
    using internal::StoragePool;

    namespace iterators {

        /**
         * Iterates efficiently over dynamic instances of a pool in type order.
         *
         * @author Timm Felden
         */
        template<class T, class B>
        class TypeOrderIterator :
                public std::iterator<std::input_iterator_tag, T> {

            TypeHierarchyIterator ts;
            StaticDataIterator<T, B> is;

        public:
            //! creates an empty iterator
            TypeOrderIterator() : ts(), is() { }

            TypeOrderIterator(const StoragePool<T, B> *p)
                    : ts(p), is(p) {
                while (ts.hasNext()) {
                    auto t = (StoragePool<T, B> *) ts.next();
                    if (t->staticSize()) {
                        new(&is) StaticDataIterator<T, B>(t);
                        break;
                    }
                }
            }

            TypeOrderIterator(const TypeOrderIterator &iter)
                    : ts(iter.ts), is(iter.is) { }

            TypeOrderIterator &operator++() {
                is.next();
                if (!is.hasNext()) {
                    while (ts.hasNext()) {
                        auto t = (StoragePool<T, B> *) ts.next();
                        if (t->staticSize()) {
                            new(&is) StaticDataIterator<T, B>(t);
                            break;
                        }
                    }
                }
                return *this;
            }

            TypeOrderIterator operator++(int) {
                TypeHierarchyIterator tmp(*this);
                operator++();
                return tmp;
            }

            //! move to next position and return current element
            T *next() {
                T *result = is.next();
                if (!is.hasNext()) {
                    while (ts.hasNext()) {
                        auto t = (StoragePool<T, B> *) ts.next();
                        if (t->staticSize()) {
                            new(&is) StaticDataIterator<T, B>(t);
                            break;
                        }
                    }
                }
                return result;
            }

            //! @return true, iff another element can be returned
            bool hasNext() const {
                return is.hasNext();
            }

            bool operator==(const TypeOrderIterator &iter) const {
                return !hasNext() && !iter.hasNext();
            }

            bool operator!=(const TypeOrderIterator &iter) const {
                return hasNext() || iter.hasNext();
            }

            T &operator*() const { return *is; }

            T* operator->() const { return is.operator->(); }

            //!iterators themselves can be used in generalized for loops
            //!@note this will not consume the iterator
            TypeOrderIterator<T, B> begin() const {
                return *this;
            }

            TypeOrderIterator<T, B> end() const {
                return TypeOrderIterator<T, B>();
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_TYPE_ORDER_ITERATOR_H
