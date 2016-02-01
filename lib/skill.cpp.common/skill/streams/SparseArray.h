//
// Created by Timm Felden on 29.01.16.
//

#ifndef SKILL_CPP_COMMON_SPARSEARRAY_H
#define SKILL_CPP_COMMON_SPARSEARRAY_H

#include <cstddef>

namespace skill {
    namespace streams {

        /**
         * creates a fixed size sparse array using a private memory mapping to a temporary file
         *
         * @author Timm Felden
         */
        template<typename T>
        struct SparseArray {
        private:
            SparseArray(T *p, size_t size)
                    : p(p), size(size) { }

        public:
            //! construct in an invalid state
            SparseArray() : p(nullptr), size(0) { }

            /**
             * @param size
             *      the desired size of the array
             *
             * @param dense
             *      if set to true, the representation will likely be an actual array
             */
            SparseArray(size_t size, bool dense = false) : p(nullptr), size(size) {
                // represent small and dense arrays by arrays
                if (dense || (sizeof(T) * size) < 4096L)
                    new(this) SparseArray(new T[size], size);
                else {
                    throw "TODO";
                }
            }

            ~SparseArray() {
                if (p)
                    delete[] p;
            }

            T *const p;
            const size_t size;

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
