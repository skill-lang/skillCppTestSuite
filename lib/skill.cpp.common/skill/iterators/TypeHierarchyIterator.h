//
// Created by Timm Felden on 28.12.15.
//

#ifndef SKILL_CPP_COMMON_TYPEHIERARCHYITERATOR_H
#define SKILL_CPP_COMMON_TYPEHIERARCHYITERATOR_H

#include <iterator>

namespace skill {
    using internal::AbstractStoragePool;

    namespace iterators {
        /**
         * iterates efficiently over the type hierarchy
         */
        class TypeHierarchyIterator :
                public std::iterator<std::input_iterator_tag, AbstractStoragePool> {

            AbstractStoragePool *current;
            AbstractStoragePool *const endParent;

        public:
            TypeHierarchyIterator(AbstractStoragePool *first)
                    : current(first), endParent(first->superPool) { }

            TypeHierarchyIterator(const TypeHierarchyIterator &iter)
                    : current(iter.current), endParent(iter.endParent) { }

            TypeHierarchyIterator &operator++() {
                const auto n = current->nextPool;
                if (n && endParent != n->superPool)
                    current = n;
                else
                    current = nullptr;
                return *this;
            }

            TypeHierarchyIterator operator++(int) {
                TypeHierarchyIterator tmp(*this);
                operator++();
                return tmp;
            }

            //! move to next position and return current element
            AbstractStoragePool &next() {
                auto p = current;
                const auto n = current->nextPool;
                if (n && endParent != n->superPool)
                    current = n;
                else
                    current = nullptr;
                return *p;
            }

            //! @return true, iff another element can be returned
            bool hasNext() {
                return current;
            }

            bool operator==(const TypeHierarchyIterator &rhs) {
                return current == rhs.current && endParent == rhs.endParent;
            }

            bool operator!=(const TypeHierarchyIterator &rhs) {
                return current != rhs.current || endParent != rhs.endParent;
            }

            AbstractStoragePool &operator*() { return *current; }

            AbstractStoragePool &operator->() { return *current; }
        };
    }
}

#endif //SKILL_CPP_COMMON_TYPEHIERARCHYITERATOR_H
