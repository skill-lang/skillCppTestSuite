//
// Created by Timm Felden on 04.11.15.
//

#include "AbstractStoragePool.h"
#include "../restrictions/TypeRestriction.h"
#include "UnknownObject.h"
#include "Book.h"
#include "StoragePool.h"

using namespace skill;
using namespace internal;
using restrictions::TypeRestriction;

skill::internal::AbstractStoragePool::AbstractStoragePool(TypeID typeID, AbstractStoragePool *superPool,
                                                          api::String const name,
                                                          std::set<TypeRestriction *> *restrictions)
        : FieldType(typeID), restrictions(restrictions),
          name(name),
          superPool(superPool),
          basePool(superPool ? superPool->basePool : this) {
    if (superPool)
        superPool->subPools.push_back(this);
}

SKilLID skill::internal::AbstractStoragePool::newDynamicInstancesSize() const {
    SKilLID r = newObjectsSize();
    for (auto sub : subPools)
        r += sub->newDynamicInstancesSize();

    return r;
}

internal::AbstractStoragePool::~AbstractStoragePool() {
    delete restrictions;

    for (auto f : dataFields)
        delete f;
}