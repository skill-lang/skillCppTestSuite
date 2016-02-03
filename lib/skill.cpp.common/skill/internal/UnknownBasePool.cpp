//
// Created by Timm Felden on 20.11.15.
//

#include "UnknownBasePool.h"
#include "LazyField.h"

using namespace skill;
using namespace internal;

FieldDeclaration *UnknownBasePool::addField(const AbstractStringKeeper *const keeper,
                                            TypeID id, const FieldType *type, api::String name) {
    auto rval = new internal::LazyField(type, name, (SKilLID)dataFields.size() + 1, this);
    dataFields.push_back(rval);
    return rval;
}

void UnknownBasePool::allocateInstances() {
    book = new Book<UnknownObject>(staticDataInstances);
    UnknownObject *page = book->firstPage();
    int idx = 0;
    for (const auto &b : blocks) {
        SKilLID i = b.bpo + 1;
        const auto last = i + b.staticCount;
        for (; i < last; i++) {
            UnknownObject *const p = page + idx++;
            data[i] = p;
            p->byPassConstruction(i, this);
        }
    }
}
