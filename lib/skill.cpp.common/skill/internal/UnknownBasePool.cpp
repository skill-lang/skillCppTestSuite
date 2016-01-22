//
// Created by Timm Felden on 20.11.15.
//

#include "UnknownBasePool.h"
#include "LazyField.h"

using namespace skill;
using namespace internal;

FieldDeclaration *UnknownBasePool::addField(const AbstractStringKeeper *const keeper,
                                            TypeID id, const FieldType *type, api::String name) {
    auto rval = new internal::LazyField(type, name, this);
    dataFields.push_back(rval);
    return rval;
}
