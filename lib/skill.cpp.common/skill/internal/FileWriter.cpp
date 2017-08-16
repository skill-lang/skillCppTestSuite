//
// Created by Timm Felden on 03.02.16.
//

#include <iostream>
#include "FileWriter.h"
#include "../api/SkillFile.h"
#include "LazyField.h"
#include "../iterators/TypeHierarchyIterator.h"
#include "../iterators/AllObjectIterator.h"

using namespace skill;
using namespace internal;

void FileWriter::write(api::SkillFile *state, const std::string &path) {
    streams::FileOutputStream out(path, false);

    // fix pools to make size operations constant time (happens in amortized constant time)
    for (auto &t : *state)
        t->fix(true);

    std::vector<FieldDeclaration *> fieldQueue;

    /**
     *  collect String instances from known string types; this is required,
     * because we use plain strings
     * @note this is a O(σ) operation:)
     * @note we do not use generation time type info, because we want to treat
     * generic fields as well
     *
     * @todo unify type and field names in string pool, so that it is no longer required to add them here (and
     * checks/searches should be faster that way)
     */
    //////////////////////
    // PHASE 1: Collect //
    //////////////////////

    for (auto t : *state) {
        for (FieldDeclaration *f : t->dataFields) {
            if (dynamic_cast<LazyField *>(f)) {
                ((LazyField *) f)->ensureIsLoaded();
            }
            fieldQueue.push_back(f);
        }
    }

    //////////////////////////////
    // PHASE 2: Check & Reorder //
    //////////////////////////////

    // index → bpo
    //  @note pools.par would not be possible if it were an actual
    std::unique_ptr<SKilLID[]> lbpoMap(new SKilLID[state->size()]);

    //  check consistency of the state, now that we aggregated all instances
    state->check();

    {
        std::vector<AbstractStoragePool *> bases;
        for (auto t : *state)
            if (nullptr == t->superPool)
                bases.push_back(t);

#pragma omp parallel for
        for (size_t i = 0; i < bases.size(); i++) {
            bases[i]->compress(lbpoMap.get());
        }
    }

    ////////////////////
    // PHASE 3: Write //
    ////////////////////

    //  write string block
    ((StringPool *const) state->strings)->prepareAndWrite(&out);

    // Calculate Offsets
    // @note this has to happen after string IDs have been updated
    {
        bool failed = false;
#pragma omp parallel for schedule(dynamic, 4)
        for (size_t i = 0; i < fieldQueue.size(); i++) {
            FieldDeclaration *f = fieldQueue[i];
            try {
                f->awaitOffset = f->offset();
            } catch (SkillException e) {
                failed = true;
                std::cerr << e.message << std::endl;
            }
        }
        if(failed)
            throw SkillException("offset calculation failed");
    }

    // write count of the type block
    out.v64(state->size());

    for (auto t : *state) {
        const auto lCount = t->blocks.front().dynamicCount;
        out.v64(t->name->getID());
        out.v64(lCount);
        restrictions(t, out);
        if (nullptr == t->superPool) {
            out.i8(0);
        } else {
            out.v64(1 + t->superPool->poolOffset());
            if (0 != lCount) {
                out.v64(lbpoMap[t->poolOffset()]);
            }
        }

        out.v64(t->dataFields.size());
    }

    // write fields
    size_t offset = 0;
    size_t endOffset = 0;
    for (auto f : fieldQueue) {
        // write field info
        out.v64(f->index);
        out.v64(f->name->getID());
        writeType(f->type, out);
        restrictions(f, out);
        endOffset = offset + f->awaitOffset;
        out.v64(endOffset);

        // update chunks and prepare write data
        Chunk *c = f->dataChunks.front();
        c->begin = offset;
        c->end = endOffset;

        offset = endOffset;
    }

    writeFieldData(state, out, offset, fieldQueue);
}

void FileWriter::restrictions(skill::internal::AbstractStoragePool *pool, streams::FileOutputStream &out) {
    // TODO
    out.i8(0);
}

void FileWriter::restrictions(skill::internal::FieldDeclaration *f, streams::FileOutputStream &out) {
    // TODO
    out.i8(0);
}

void FileWriter::writeType(const FieldType *const type, streams::FileOutputStream &out) {
    out.v64(type->typeID);

    switch (type->typeID) {
        case 0:
            out.i8(((fieldTypes::ConstantI8 *) type)->value);
            break;
        case 1 :
            out.i16(((fieldTypes::ConstantI16 *) type)->value);
            break;
        case 2 :
            out.i32(((fieldTypes::ConstantI32 *) type)->value);
            break;
        case 3 :
            out.i64(((fieldTypes::ConstantI64 *) type)->value);
            break;
        case 4 :
            out.v64(((fieldTypes::ConstantV64 *) type)->value);
            break;

        case 15: {
            const auto arr = (fieldTypes::ConstantLengthArray *) type;
            out.v64(arr->length);
            writeType(arr->base, out);
            break;
        }

        case 17:
            writeType(((fieldTypes::VariableLengthArray *) type)->base, out);
            break;
        case 18:
            writeType(((fieldTypes::ListType *) type)->base, out);
            break;
        case 19:
            writeType(((fieldTypes::SetType *) type)->base, out);
            break;

        case 20: {
            const auto m = (fieldTypes::MapType *) type;
            writeType(m->key, out);
            writeType(m->value, out);
            break;
        }

        default:
            break;
    }
}

void FileWriter::writeFieldData(SkillFile *state, streams::FileOutputStream &out,
                                size_t offset, std::vector<FieldDeclaration *> &fields) {

    // map field data
    streams::MappedOutStream *source = out.jumpAndMap(offset);

    // write field data
#pragma omp parallel for schedule(dynamic, 4)
    for (size_t i = 0; i < fields.size(); i++) {
        FieldDeclaration *f = fields[i];
        auto c = f->dataChunks.back();
        const auto map = source->clone(c->begin, c->end);
        f->write(map);
        delete map;
    }

    out.unmap(source);

    ///////////////////////
    // PHASE 4: Cleaning //
    ///////////////////////

    // unfix pools
    for (auto t : *state)
        t->fix(false);
}
