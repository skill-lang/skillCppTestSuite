//
// Created by Timm Felden on 28.12.15.
//

#ifndef SKILL_CPP_COMMON_DYNAMIC_DATA_ITERATOR_H
#define SKILL_CPP_COMMON_DYNAMIC_DATA_ITERATOR_H

#include <iterator>

namespace skill {
    namespace internal {
        template<class T, class B>
        class StoragePool;
    }
    using internal::StoragePool;

    namespace iterators {
        /**
         * Iterates efficiently over dynamic instances of a pool.
         *
         * First phase will iterate over all blocks of the pool.
         * The second phase will iterate over all dynamic instances of the pool.
         *
         * @author Timm Felden
         */
        template<class T, class B>
        class DynamicDataIterator :
                public std::iterator<std::input_iterator_tag, T> {

            //! current type
            TypeHierarchyIterator ts;

            SKilLID secondIndex;
            const SKilLID lastBlock;
            SKilLID index;
            SKilLID last;

        public:
            //! creates an empty iterator
            DynamicDataIterator() : ts(), secondIndex(0), lastBlock(0), index(0), last(0) { }

            DynamicDataIterator(const StoragePool<T, B> *p)
                    : ts(p), secondIndex(0), lastBlock(p->blocks.size()),
                      index(0), last(0) {

                while (index == last && secondIndex < lastBlock) {
                    const auto &b = p->blocks[secondIndex];
                    index = b.bpo;
                    last = index + b.dynamicCount;
                    secondIndex++;
                }
                // mode switch, if there is no other block
                if (index == last && secondIndex == lastBlock) {
                    secondIndex++;
                    while (ts.hasNext()) {
                        const StoragePool<T, B> &t = (const StoragePool<T, B> &) *ts;
                        if (t.newObjects.size() != 0) {
                            index = 0;
                            last = t.newObjects.size();
                            break;
                        }
                        ++ts;
                    }
                }
            }

            DynamicDataIterator(const DynamicDataIterator<T, B> &iter)
                    : ts(iter.ts), secondIndex(iter.secondIndex), lastBlock(iter.lastBlock),
                      index(iter.index), last(iter.last) { }

            DynamicDataIterator &operator++() {
                index++;
                if (index == last) {
                    if (secondIndex <= lastBlock) {
                        const StoragePool<T, B> &p = (const StoragePool<T, B> &) (*ts);
                        while (index == last && secondIndex < lastBlock) {
                            const auto &b = p.blocks[secondIndex];
                            index = b.bpo;
                            last = index + b.dynamicCount;
                            secondIndex++;
                        }
                        // mode switch, if there is no other block
                        if (index == last && secondIndex == lastBlock) {
                            secondIndex++;
                            while (ts.hasNext()) {
                                const StoragePool<T, B> &t = (const StoragePool<T, B> &) *ts;
                                if (t.newObjects.size() != 0) {
                                    index = 0;
                                    last = t.newObjects.size();
                                    break;
                                }
                                ++ts;
                            }
                        }
                    } else {
                        ++ts;
                        while (ts.hasNext()) {
                            const StoragePool<T, B> &t = (const StoragePool<T, B> &) *ts;
                            if (t.newObjects.size() != 0) {
                                index = 0;
                                last = t.newObjects.size();
                                break;
                            }
                            ++ts;
                        }
                    }
                }
                return *this;
            }

            DynamicDataIterator operator++(int) {
                DynamicDataIterator tmp(*this);
                operator++();
                return tmp;
            }

            //! move to next position and return current element
            T *next() {
                const StoragePool<T, B> &p = (const StoragePool<T, B> &) *ts;
                if (secondIndex <= lastBlock) {
                    // @note increment happens before access, because we shifted data by 1
                    index++;
                    T *r = p.data[index];
                    if (index == last) {
                        while (index == last && secondIndex < lastBlock) {
                            const auto &b = p.blocks[secondIndex];
                            index = b.bpo;
                            last = index + b.dynamicCount;
                            secondIndex++;
                        }
                        // mode switch, if there is no other block
                        if (index == last && secondIndex == lastBlock) {
                            secondIndex++;
                            while (ts.hasNext()) {
                                const StoragePool<T, B> *t = (const StoragePool<T, B> *) ts.next();
                                if (t->newObjects.size() != 0) {
                                    index = 0;
                                    last = t->newObjects.size();
                                    break;
                                }
                            }
                        }
                    }
                    return r;
                } else {
                    T *r = p.newObjects[index];
                    index++;
                    if (index == last) {
                        while (ts.hasNext()) {
                            const StoragePool<T, B> *t = (const StoragePool<T, B> *) ts.next();
                            if (t->newObjects.size() != 0) {
                                index = 0;
                                last = t->newObjects.size();
                                break;
                            }
                        }
                    }
                    return r;
                }
            }

            //! @return true, iff another element can be returned
            bool hasNext() const {
                return index != last;
            }

            //! @note all empty iterators are considered equal
            bool operator==(const DynamicDataIterator<T, B> &iter) const {
                return (!hasNext() && !iter.hasNext())
                       || (ts == iter.ts &&
                           secondIndex == iter.secondIndex &&
                           lastBlock == iter.lastBlock &&
                           index == iter.index &&
                           last == iter.last);
            }

            //! @note all empty iterators are considered equal
            bool operator!=(const DynamicDataIterator<T, B> &iter) const {
                return (hasNext() || iter.hasNext())
                       && !(ts == iter.ts &&
                            secondIndex == iter.secondIndex &&
                            lastBlock == iter.lastBlock &&
                            index == iter.index &&
                            last == iter.last);
            }

            T &operator*() const {
                const StoragePool<T, B> &p = (const StoragePool<T, B> &) *ts;
                // @note increment happens before access, because we shifted data by 1
                return *(secondIndex <= lastBlock ? p.data[index + 1] : p.newObjects[index]);
            }

            T* operator->() const {
                const StoragePool<T, B> &p = (const StoragePool<T, B> &) *ts;
                return secondIndex <= lastBlock ? p.data[index + 1] : p.newObjects[index];
            }

            //!iterators themselves can be used in generalized for loops
            //!@note this will not consume the iterator
            DynamicDataIterator<T, B> begin() const {
                return *this;
            }

            DynamicDataIterator<T, B> end() const {
                return DynamicDataIterator<T, B>();
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_DYNAMIC_DATA_ITERATOR_H
