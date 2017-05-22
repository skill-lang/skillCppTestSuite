//
// Created by Timm Felden on 29.01.16.
//

#ifndef SKILL_CPP_COMMON_SPARSEARRAY_H
#define SKILL_CPP_COMMON_SPARSEARRAY_H

#include "../utils.h"

#include <cstddef>
#include <sys/mman.h>

namespace skill {
    namespace streams {

        /**
         * creates a fixed size sparse array using a private memory mapping to a temporary file
         *
         * @note cannot be used if T-instances require initialization
         *
         * @author Timm Felden
         */
        template<typename T>
        struct SparseArray {
        private:
            SparseArray(T *p, size_t size, bool isPrivateMap)
                    : p(p), size(size), isPrivateMap(isPrivateMap) {}

        public:
            //! construct in an invalid state
            SparseArray() : p(nullptr), size(0), isPrivateMap(false) {}

            /**
             * @param size
             *      the desired size of the array
             *
             * @param contiguous
             *      if set to true, the representation will likely be an actual array
             */
            SparseArray(size_t size, bool contiguous = false) : p(nullptr), size(size), isPrivateMap(false) {
                // represent small and dense arrays by arrays
                if (contiguous || (sizeof(T) * size) < 4096L)
                    new(this) SparseArray(new T[size], size, false);
                else {
                    new(this) SparseArray(
                            (T *) mmap(nullptr, sizeof(T) * size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1,
                                       0),
                            size, true);
                }
            }

            ~SparseArray() {
                if (p) {
                    if (isPrivateMap) {
                        munmap(p, sizeof(T) * size);
                    } else {
                        delete[] p;
                    }
                }
            }

            /**
             * changes the size of the sparse array
             */
            void resize(SparseArray<T> &newData) {
                //we cannot delete this directly, as it might be stack-local
                if (p) {
                    if (isPrivateMap) {
                        munmap(p, sizeof(T) * size);
                    } else {
                        delete[] p;
                    }
                }
                new(this)SparseArray<T>(newData.p, newData.size, newData.isPrivateMap);
            }

            T *const p;
            const size_t size;
        private:
            const bool isPrivateMap;

        public:

            inline T &operator[](size_t i) const {
                return p[i];
            }

            inline T *begin() const {
                return p;
            }

            inline T *end() const {
                return p + size;
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_SPARSEARRAY_H
