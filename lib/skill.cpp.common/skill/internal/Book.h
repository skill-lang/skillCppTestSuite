//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_BOOK_H
#define SKILL_CPP_COMMON_BOOK_H

#include <vector>
#include "../common.h"

namespace skill {
    namespace internal {
        /**
         * management of T instances; basically a freelist approach
         * @author Timm Felden
         */
        template<class T>
        class Book {
            std::vector<T *> freelist;
            //! @invariant: if not current page then, T is used or T is in freeList
            std::vector<T *> pages;

            T *currentPage;
            /**
             * we need remaining and next index, because pages can have variable size
             */
            int currentPageRemaining;
            int currentPageNextIndex;

            //! size of unhinted pages
            static const int defaultPageSize = 128;
            static_assert(defaultPageSize > 0, "pages must have positive size");

        public:
            /**
             * a new book that has pre allocated instances for the expected size
             */
            Book(SKilLID expectedSize)
                    : freelist(), pages(), currentPage(new T[expectedSize]),
                      currentPageRemaining(expectedSize), currentPageNextIndex(0) {
                pages.push_back(currentPage);
            }

            virtual ~Book() {
                for (T *page : pages) {
                    delete[] page;
                }
            }

            /**
             * return the next free instance
             */
            T *next() {
                // first we try to take from current page
                if (currentPageRemaining) {
                    --currentPageRemaining;
                    return currentPage + (currentPageNextIndex++);
                } else if (freelist.size()) {
                    // deplete freelist before allocating a new page
                    T *r = freelist.back();
                    freelist.pop_back();
                    return r;
                } else {
                    // we have to allocate a new page
                    currentPage = new T[defaultPageSize];
                    currentPageRemaining = defaultPageSize;
                    currentPageNextIndex = 0;

                    // return first object
                    --currentPageRemaining;
                    return currentPage + (currentPageNextIndex++);
                }
            }

            /**
             * recycle the argument instance
             */
            void free(T *target) {
                freelist.push_back(target);
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_BOOK_H
