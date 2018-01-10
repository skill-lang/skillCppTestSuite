//
// Created by Timm Felden on 28.12.15.
//

#ifndef SKILL_CPP_COMMON_STATIC_DATA_ITERATOR_H
#define SKILL_CPP_COMMON_STATIC_DATA_ITERATOR_H

#include <iterator>


namespace skill {
    namespace internal {
        template<class T, class B>
        class StoragePool;
    }
    using internal::StoragePool;

    namespace iterators {
        /**
         * Iterates efficiently over static instances of a pool.
         *
         * @author Timm Felden
         */
        template<class T, class B>
        class StaticDataIterator :
                public std::iterator<std::input_iterator_tag, T> {

            //! target pool
            const StoragePool<T, B> *const p;

            SKilLID secondIndex;
            const SKilLID lastBlock;
            SKilLID index;
            SKilLID last;

        public:
            //! creates an empty iterator
            StaticDataIterator()
                    : p(nullptr), secondIndex(0), lastBlock(0),
                      index(0), last(0) { }

            StaticDataIterator(const StoragePool<T, B> *p)
                    : p(p), secondIndex(0), lastBlock(p->blocks.size()), index(0), last(0) {

                while (index == last && secondIndex < lastBlock) {
                    const auto &b = p->blocks[secondIndex];
                    index = b.bpo;
                    last = index + b.staticCount;
                    secondIndex++;
                }
                // mode switch, if there is no other block
                if (index == last && secondIndex == lastBlock) {
                    secondIndex++;
                    index = 0;
                    last = p->newObjectsSize();
                }
            }

            StaticDataIterator(const StaticDataIterator &iter)
                    : p(iter.p), secondIndex(iter.secondIndex), lastBlock(iter.lastBlock),
                      index(iter.index), last(iter.last) { }

            StaticDataIterator &operator++() {
                if (secondIndex <= lastBlock) {
                    index++;
                    if (index == last) {
                        while (index == last && secondIndex < lastBlock) {
                            const auto &b = p->blocks[secondIndex];
                            index = b.bpo;
                            last = index + b.staticCount;
                            secondIndex++;
                        }
                        // mode switch, if there is no other block
                        if (index == last && secondIndex == lastBlock) {
                            secondIndex++;
                            index = 0;
                            last = p->newObjectsSize();
                        }
                    }
                } else {
                    index++;
                }
                return *this;
            }

            StaticDataIterator operator++(int) {
                StaticDataIterator tmp(*this);
                operator++();
                return tmp;
            }

            //! move to next position and return current element
            T *next() {
                if (secondIndex <= lastBlock) {
                    // @note increment happens before access, because we shifted data by 1
                    index++;
                    T *r = p->data[index];
                    if (index == last) {
                        while (index == last && secondIndex < lastBlock) {
                            const auto &b = p->blocks[secondIndex];
                            index = b.bpo;
                            last = index + b.staticCount;
                            secondIndex++;
                        }
                        // mode switch, if there is no other block
                        if (index == last && secondIndex == lastBlock) {
                            secondIndex++;
                            index = 0;
                            last = p->newObjectsSize();
                        }
                    }
                    return r;
                } else {
                    T *r = p->newObjects[index];
                    index++;
                    return r;
                }
            }

            //! @return true, iff another element can be returned
            bool hasNext() const {
                return index != last;
            }

            //! @note all empty iterators are considered equal
            bool operator==(const StaticDataIterator<T, B> &iter) const {
                return (!hasNext() && !iter.hasNext())
                       || (p == iter.p &&
                           secondIndex == iter.secondIndex &&
                           lastBlock == iter.lastBlock &&
                           index == iter.index &&
                           last == iter.last);
            }

            //! @note all empty iterators are considered equal
            bool operator!=(const StaticDataIterator<T, B> &iter) const {
                return (hasNext() || iter.hasNext())
                       && !(p == iter.p &&
                            secondIndex == iter.secondIndex &&
                            lastBlock == iter.lastBlock &&
                            index == iter.index &&
                            last == iter.last);
            }

            T &operator*() const {
                // @note increment happens before access, because we shifted data by 1
                return *(secondIndex <= lastBlock ? p->data[index + 1] : p->newObjects[index]);
            }

            T* operator->() const {
                return secondIndex <= lastBlock ? p->data[index + 1] : p->newObjects[index];
            }

            //!iterators themselves can be used in generalized for loops
            //!@note this will not consume the iterator
            StaticDataIterator <T, B> begin() const {
                return *this;
            }

            StaticDataIterator <T, B> end() const {
                return StaticDataIterator<T, B>();
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_STATIC_DATA_ITERATOR_H
