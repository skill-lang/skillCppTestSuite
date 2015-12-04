//
// Created by Timm Felden on 02.12.15.
//

#ifndef SKILL_CPP_COMMON_UNKNOWNSUBPOOL_CPP_H
#define SKILL_CPP_COMMON_UNKNOWNSUBPOOL_CPP_H

#include "UnknownSubPool.h"

template<class T, class B>
void skill::internal::UnknownSubPool<T, B>::allocateDeferred() {
    this->book = new Book<T>(this->staticDataInstances);
    T *page = this->book->firstPage();
    int idx = 0;
    for (const auto &b : this->blocks) {
        SKilLID i = b.bpo + 1;
        const auto last = i + b.staticCount;
        for (; i < last; i++) {
            T *const p = page + idx++;
            this->data[i] = p;
            p->byPassConstruction(i, this);
        }
    }
}

#endif //SKILL_CPP_COMMON_UNKNOWNSUBPOOL_CPP_H
