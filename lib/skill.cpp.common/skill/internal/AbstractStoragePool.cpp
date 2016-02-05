//
// Created by Timm Felden on 04.11.15.
//

#include "AbstractStoragePool.h"
#include "UnknownObject.h"
#include "StoragePool.h"
#include "../iterators/TypeHierarchyIterator.h"

using namespace skill;
using namespace internal;
using restrictions::TypeRestriction;

skill::internal::AbstractStoragePool::AbstractStoragePool(TypeID typeID, AbstractStoragePool *superPool,
                                                          api::String const name,
                                                          std::set<TypeRestriction *> *restrictions)
        : FieldType(typeID), restrictions(restrictions),
          name(name),
          superPool(superPool),
          basePool(superPool ? superPool->basePool : this),
          typeHierarchyHeight(superPool ? superPool->typeHierarchyHeight + 1 : 0) {
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

void AbstractStoragePool::updateAfterCompress(SKilLID *lbpoMap) {
    // update pointer to data
    if (superPool)
        allocateData();

    // reset data chunks
    for (auto f : dataFields)
        f->resetChunks(cachedSize);

    // reset blocks
    // @note reset of data chunks may have required reading old block structure
    blocks.clear();
    blocks.push_back(Block(0, lbpoMap[poolOffset()], staticSize(), cachedSize));
    for (auto s : subPools)
        s->updateAfterCompress(lbpoMap);

    clearNewObjects();
}

void AbstractStoragePool::complete(SkillFile *owner) {
    this->owner = owner;
    // valid if there are no known pools
}
