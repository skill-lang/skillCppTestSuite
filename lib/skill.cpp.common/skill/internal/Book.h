//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_BOOK_H
#define SKILL_CPP_COMMON_BOOK_H

#include <vector>
#include "../common.h"
#include <cassert>

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
             * we don't need an index, because we will only have a variable page size for the first page
             */
            int currentPageRemaining;

            //! size of unhinted pages
            static const int defaultPageSize = 128;
            static_assert(defaultPageSize > 0, "pages must have positive size");

        public:
            /**
             * a new book that has pre allocated instances for the expected size
             */
            Book(SKilLID expectedSize)
                    : freelist(), pages(), currentPage(new T[expectedSize]),
                      currentPageRemaining(0) {
                pages.push_back(currentPage);
            }

            virtual ~Book() {
                for (T *page : pages) {
                    delete[] page;
                }
            }

            /**
             * returns the first page. only to be used on initial allocateInstances call!
             */
            T *firstPage() {
                assert(pages[0] == currentPage);
                return currentPage;
            }

            /**
             * return the next free instance
             *
             * @note must never be called, while using the first page
             */
            T *next() {
                // first we try to take from current page
                if (currentPageRemaining) {
                    return currentPage + (defaultPageSize - currentPageRemaining--);
                } else if (freelist.size()) {
                    // deplete freelist before allocating a new page
                    T *r = freelist.back();
                    freelist.pop_back();
                    return r;
                } else {
                    // we have to allocate a new page
                    currentPage = new T[defaultPageSize];
                    pages.push_back(currentPage);
                    // return first object
                    currentPageRemaining = defaultPageSize - 1;
                    return currentPage;
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
