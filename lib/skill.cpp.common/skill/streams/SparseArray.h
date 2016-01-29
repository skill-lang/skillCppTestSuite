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
            // construct in an invalid state
            SparseArray() : p(nullptr), size(0) { }

            SparseArray(size_t size) : p(new T[size]), size(size) {
                // TODO not sparse at all
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
