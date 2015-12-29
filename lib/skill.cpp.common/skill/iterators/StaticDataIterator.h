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

            SKilLID secondIndex = 0;
            const SKilLID lastBlock = p.blocks.size;
            SKilLID index = 0;
            SKilLID end = 0;

        public:
            StaticDataIterator(const StoragePool<T, B> *p)
                    : p(p), secondIndex(0), lastBlock(p->blocks.size()), index(0), end(0) {

                while (index == end && secondIndex < lastBlock) {
                    const auto &b = p->blocks[secondIndex];
                    index = b.bpo;
                    end = index + b.staticCount;
                    secondIndex++;
                }
                // mode switch, if there is no other block
                if (index == end)
                    secondIndex = 0;
            }

            StaticDataIterator(const StaticDataIterator &iter)
                    : p(iter.p), secondIndex(iter.secondIndex), lastBlock(iter.lastBlock),
                      index(iter.index), end(iter.end) { }

            StaticDataIterator &operator++() {
                if (index < end) {
                    // @note increment happens before access, because we shifted data by 1
                    index++;
                    while (index == end && secondIndex < lastBlock) {
                        const auto &b = p->blocks[secondIndex];
                        index = b.bpo;
                        end = index + b.staticCount;
                        secondIndex++;
                    }
                    // mode switch, if there is no other block
                    if (index == end)
                        secondIndex = 0;
                } else {
                    secondIndex++;
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
                if (index < end) {
                    // @note increment happens before access, because we shifted data by 1
                    index++;
                    T *r = p->data[index];
                    while (index == end && secondIndex < lastBlock) {
                        const auto &b = p->blocks[secondIndex];
                        index = b.bpo;
                        end = index + b.staticCount;
                        secondIndex++;
                    }
                    // mode switch, if there is no other block
                    if (index == end)
                        secondIndex = 0;
                    return r;
                } else {
                    T *r = p->newObjects[secondIndex];
                    secondIndex++;
                    return r;
                }
            }

            //! @return true, iff another element can be returned
            bool hasNext() const {
                return index < end || secondIndex < p->newObjects.size();
            }

            bool operator==(const StaticDataIterator &iter) const {
                return p == iter.p &&
                       secondIndex == iter.secondIndex &&
                       lastBlock == iter.lastBlock &&
                       index == iter.index &&
                       end == iter.end;
            }

            bool operator!=(const StaticDataIterator &rhs) const {
                return !(this->operator==(rhs));
            }

            T &operator*() const {
                // @note increment happens before access, because we shifted data by 1
                return *(index < end ? p->data[index + 1] : p->newObjects[secondIndex]);
            }

            T &operator->() const {
                return *(index < end ? p->data[index + 1] : p->newObjects[secondIndex]);
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_STATIC_DATA_ITERATOR_H
