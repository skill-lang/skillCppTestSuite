//
// Created by Timm Felden on 28.01.16.
//

#include "DistributedField.h"
#include "AbstractStoragePool.h"
#include "StoragePool.h"

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
        new(&data) streams::SparseArray<api::Box>((size_t) owner->basePool->size(),
                                                  owner->blocks.size() <= 1 && !owner->blocks[0].bpo);

    skill::streams::MappedInStream in(part, target->begin, target->end);

    try {
        if (dynamic_cast<const SimpleChunk *>(target)) {
            for (::skill::SKilLID i = ((const ::skill::internal::SimpleChunk *) target)->bpo,
                         high = i + target->count; i != high; i++)
                data[i] = type->read(in);
        } else {
            //case bci : BulkChunk ⇒
            for (int i = 0; i < ((const ::skill::internal::BulkChunk *) target)->blockCount; i++) {
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


inline void DistributedField::deleteBoxedContainer(api::Box b, const FieldType *const type) {
    if (b.annotation)
        switch (type->typeID) {
            case 15:
            case 17:
            case 18:
                delete b.array;
                return;
            case 19:
                delete b.set;
                return;

            case 20:
                if (20 == ((const fieldTypes::MapType *const) type)->value->typeID) {
                    auto ps = b.map->all();
                    while (ps->hasNext())
                        deleteBoxedContainer(ps->next().second, ((const fieldTypes::MapType *const) type)->value);
                }
                delete b.map;
                return;
            default:
                return;
        }
}


DistributedField::~DistributedField() {
    // delete containers stored in a box
    if (this->typeRequiresDestruction) {
        //! all container types require destruction; other types may no longer exist
        //! @note data may not have been forced yet if we are in fact lazy
        if (data.p)
            for (const auto &b : owner->blocks)
                for (auto i = b.bpo; i < b.bpo + b.dynamicCount; i++)
                    deleteBoxedContainer(data[i], this->type);

        for (const auto &i : newData)
            deleteBoxedContainer(i.second, this->type);

    }
}

size_t DistributedField::offset() const {
    size_t result = 0;

    if (dynamic_cast<const ::skill::internal::SimpleChunk *>(dataChunks.back())) {
        // case c : SimpleChunk =>
        const SimpleChunk *c = (const SimpleChunk *) dataChunks.back();
        auto i = c->bpo;
        const auto end = i + c->count;
        while (i != end) {
            result += type->offset(data[i++]);
        }

    } else {
        // case bci : BulkChunk =>
        const BulkChunk *bci = (const BulkChunk *) dataChunks.back();
        const auto &blocks = owner->blocks;
        int blockIndex = 0;
        while (blockIndex < bci->blockCount) {
            const auto &b = blocks[blockIndex];
            blockIndex++;

            auto i = b.bpo;
            const auto end = i + b.dynamicCount;
            while (i != end) {
                result += type->offset(data[i++]);
            }
        }
    }

    return result;
}

void DistributedField::write(streams::MappedOutStream *out) const {
    if (dynamic_cast<const ::skill::internal::SimpleChunk *>(dataChunks.back())) {
        // case c : SimpleChunk =>
        const SimpleChunk *c = (const SimpleChunk *) dataChunks.back();

        auto i = c->bpo;
        const auto end = i + c->count;
        while (i != end) {
            type->write(out, data[i++]);
        }

    } else {
        // case bci : BulkChunk =>
        const BulkChunk *bci = (const BulkChunk *) dataChunks.back();
        const auto &blocks = owner->blocks;
        int blockIndex = 0;
        while (blockIndex < bci->blockCount) {
            const auto &b = blocks[blockIndex];
            blockIndex++;

            auto i = b.bpo;
            const auto end = i + b.dynamicCount;
            while (i != end) {
                type->write(out, data[i++]);
            }
        }
    }
}

/**
 * @note this method is invoked _before_ object IDs get reassigned!
 *
 * @todo append is not implemented, because there is no way to test it, yet.
 *
 * @todo implementation of append may require a second parameter (bool isAppend)
 *
 * @todo ignores deletes!
 */
void DistributedField::resetChunks(SKilLID lbpo, SKilLID newSize) {
// @note we cannot delete objects and we will always write
    // therefore, new IDs will be data ++ newData matching exactly the pool's last block


    // update internal (requires old block structure, so that's the first action)
    {
        // create new data on the stack
        streams::SparseArray<api::Box> d((size_t) (lbpo + newSize), !lbpo);

        // update data -> d
        size_t newI = (size_t) lbpo;
        {
            // todo subtract deleted objects on implementation of delete
            size_t newEnd = (size_t) (lbpo);
            for (const auto *chunk : dataChunks) {
                newEnd += chunk->count;
            }
            auto iter = ((StoragePool<Object, Object>*)owner)->allInTypeOrder();
            while(newI < newEnd) {
                assert(iter.hasNext());
                d[newI++] = data[iter.next()->id-1];
            }
        }

        // update newData -> d
        {
            const size_t newEnd = (size_t) (lbpo + newSize);
            while (newI < newEnd) {
                d[newI] = newData[owner->getAsAnnotation(newI)];
                newI++;
            }
            newData.clear();
        }

        // move d -> data
        data.resize(d);

        // prevent deallocation of d's content as it is by now owned by data
        new(&d) streams::SparseArray<api::Box>;
    }

    FieldDeclaration::resetChunks(newSize, 0);
}
