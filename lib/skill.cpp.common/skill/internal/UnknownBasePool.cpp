//
// Created by Timm Felden on 20.11.15.
//

#include "UnknownBasePool.h"
#include "LazyField.h"

using namespace skill;
using namespace internal;

FieldDeclaration *UnknownBasePool::addField(TypeID id, const FieldType *type,
                                            api::String name) {
    auto rval = new internal::LazyField(type, name);
    dataFields.push_back(rval);
    return rval;
}
