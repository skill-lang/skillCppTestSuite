//
// Created by Timm Felden on 28.01.16.
//

#include "DistributedField.h"
#include "AbstractStoragePool.h"

using namespace skill;
using namespace internal;

api::Box DistributedField::getR(const api::Object *i) {
    return (-1 == i->id) ?
           newData[i] :
           data[i->id - 1];
}

void DistributedField::setR(api::Object *i, api::Box v) {
    ((-1 == i->id) ? newData[i] : data[i->id - 1]) = v;
}

void DistributedField::read(const streams::MappedInStream *part, const Chunk *target) {
    if (!data.p)
        new(&data) streams::SparseArray<api::Box>((size_t) owner->basePool->size());

    skill::streams::MappedInStream in(part, target->begin, target->end);

    try {
        if (target->isSimple()) {
            for (::skill::SKilLID i = ((::skill::internal::SimpleChunk *) target)->bpo,
                         high = i + target->count; i != high; i++)
                data[i] = type->read(in);
        } else {
            //case bci : BulkChunk ⇒
            for (int i = 0; i < ((::skill::internal::BulkChunk *) target)->blockCount; i++) {
                const auto &b = owner->blocks[i];
                for (::skill::SKilLID i = b.bpo, end = i + b.dynamicCount; i != end; i++)
                    data[i] = type->read(in);
            }
        }
    } catch (::skill::SkillException e) {
        throw ParseException(
                in.getPosition(),
                part->getPosition() + target->begin,
                part->getPosition() + target->end, e.message);
    } catch (...) {
        throw ParseException(
                in.getPosition(),
                part->getPosition() + target->begin,
                part->getPosition() + target->end, "unexpected foreign exception");
    }

    if (!in.eof())
        throw ParseException(
                in.getPosition(),
                part->getPosition() + target->begin,
                part->getPosition() + target->end, "did not consume all bytes");
}

bool DistributedField::check() const {
    if (checkedRestrictions.size()) {
        for (const auto &b : owner->blocks)
            for (auto i = b.bpo; i < b.bpo + b.dynamicCount; i++)
                for (auto r : checkedRestrictions)
                    if (!r->check(data[i]))
                        return false;

        for (const auto &i : newData) {
            for (auto r : checkedRestrictions)
                if (!r->check(i.second))
                    return false;
        }
    }
    return true;
}
