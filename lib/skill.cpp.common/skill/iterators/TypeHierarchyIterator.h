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
                public std::iterator<std::input_iterator_tag, const AbstractStoragePool> {

            const AbstractStoragePool *current;
            const int endHeight;


        public:
            //! creates an empty iterator
            TypeHierarchyIterator()
                    : current(nullptr), endHeight(0) { }

            TypeHierarchyIterator(const AbstractStoragePool *first)
                    : current(first), endHeight(first->typeHierarchyHeight) { }

            TypeHierarchyIterator(const TypeHierarchyIterator &iter)
                    : current(iter.current), endHeight(iter.endHeight) { }

            TypeHierarchyIterator &operator++() {
                const auto n = current->nextPool;
                if (n && endHeight < n->typeHierarchyHeight)
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
            const AbstractStoragePool *next() {
                auto p = current;
                const auto n = current->nextPool;
                if (n && endHeight < n->typeHierarchyHeight)
                    current = n;
                else
                    current = nullptr;
                return p;
            }

            //! @return true, iff another element can be returned
            bool hasNext() const {
                return current;
            }

            //! @note all empty iterators are considered equal
            bool operator==(const TypeHierarchyIterator &rhs) const {
                return (!current && !rhs.current) || (current == rhs.current && endHeight == rhs.endHeight);
            }

            //! @note all empty iterators are considered equal
            bool operator!=(const TypeHierarchyIterator &rhs) const {
                return (current || rhs.current) && (current != rhs.current || endHeight != rhs.endHeight);
            }

            const AbstractStoragePool &operator*() const { return *current; }

            const AbstractStoragePool* operator->() const { return current; }

            //!iterators themselves can be used in generalized for loops
            //!@note this will not consume the iterator
            TypeHierarchyIterator begin() const {
                return *this;
            }

            TypeHierarchyIterator end() const {
                return TypeHierarchyIterator();
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_TYPEHIERARCHYITERATOR_H
