//
// Created by Timm Felden on 20.11.15.
//

#include "FieldDeclaration.h"
#include "AbstractStoragePool.h"

void skill::internal::FieldDeclaration::addRestriction(const skill::restrictions::FieldRestriction *r) {
    if (dynamic_cast<const restrictions::CheckableRestriction *>(r))
        checkedRestrictions.insert((restrictions::CheckableRestriction *) r);
    else
        otherRestrictions.insert(r);
}

skill::internal::FieldDeclaration::~FieldDeclaration() {
    // delete stateful restrictions
    for (auto c : checkedRestrictions) {
        if (3 == c->id) // range
            delete c;
    }
    for (auto c : otherRestrictions) {
        if (1 == c->id || 5 == c->id) // default, coding
            delete c;
    }

    for (auto c : dataChunks)
        delete c;
}

void skill::internal::FieldDeclaration::rbc(skill::streams::MappedInStream *in,
                                            const skill::internal::BulkChunk *target) {
    auto& blocks = owner->blocks;
    int blockIndex = 0;
    const int endBlock = target->blockCount;
    while (blockIndex < endBlock) {
        Block b = blocks[blockIndex++];
        int i = b.bpo + 1;
        rsc(i, i + b.dynamicCount, in);
    }
}
