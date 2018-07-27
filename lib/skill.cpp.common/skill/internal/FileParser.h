//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_FILEPARSER_H_H
#define SKILL_CPP_COMMON_FILEPARSER_H_H

#include "../common.h"
#include "../api/SkillFile.h"
#include "ParseException.h"
#include "../streams/FileInputStream.h"
#include "StringPool.h"
#include "AbstractStoragePool.h"
#include "../restrictions/FieldRestriction.h"
#include "../restrictions/TypeRestriction.h"
#include "../fieldTypes/BuiltinFieldType.h"
#include "../fieldTypes/AnnotationType.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cassert>

#if defined(_OPENMP)
#include <omp.h>
#endif

/**
 * set to 1, to enable debug output; this should be disabled on all commits
 */
#define debugOnly if(0)

namespace skill {
    using namespace streams;
    using namespace fieldTypes;
    using namespace restrictions;
    namespace internal {

        /**
         * Turns a field type into a preliminary type information. In case of user types, the declaration
         * of the respective user type may follow after the field declaration.
         */
        inline const FieldType *parseFieldType(FileInputStream *in,
                                               const std::vector<AbstractStoragePool *> *types,
                                               StringPool *String,
                                               AnnotationType *Annotation,
                                               int blockCounter) {
            const TypeID i = (TypeID) in->v64();
            switch (i) {
                case 0 :
                    return new ConstantI8(in->i8());
                case 1 :
                    return new ConstantI16(in->i16());
                case 2 :
                    return new ConstantI32(in->i32());
                case 3 :
                    return new ConstantI64(in->i64());
                case 4 :
                    return new ConstantV64(in->v64());
                case 5 :
                    return Annotation;
                case 6 :
                    return &BoolType;
                case 7 :
                    return &I8;
                case 8 :
                    return &I16;
                case 9 :
                    return &I32;
                case 10:
                    return &I64;
                case 11:
                    return &V64;
                case 12:
                    return &F32;
                case 13:
                    return &F64;
                case 14:
                    return String;
                case 15: {
                    int64_t length = in->v64();
                    auto t = parseFieldType(in, types, String, Annotation, blockCounter);
                    return new ConstantLengthArray(length, t);
                }
                case 17:
                    return new VariableLengthArray(parseFieldType(in, types, String, Annotation, blockCounter));
                case 18:
                    return new ListType(parseFieldType(in, types, String, Annotation, blockCounter));
                case 19:
                    return new SetType(parseFieldType(in, types, String, Annotation, blockCounter));
                case 20:
                    return new MapType(parseFieldType(in, types, String, Annotation, blockCounter),
                                       parseFieldType(in, types, String, Annotation, blockCounter));

                default:
                    if (i >= 32 && i - 32 < (TypeID) types->size())
                        return types->at(i - 32);
                    else
                        throw ParseException(in, blockCounter,
                                             "Invalid type ID");
            }

        }


        /**
         * create a new empty skill file; parametrized by specification dependent functionality.
         */
        template<
                //!ensures that names of pools and known fields are known upfront, so that it is safe
                // to compare their names by pointer value
                StringPool *initializeStrings(FileInputStream *),

                //!create a new pool in the target type system
                AbstractStoragePool *newPool(TypeID typeID,
                                             String name,
                                             AbstractStoragePool *superPool,
                                             std::set<TypeRestriction *> *restrictions,
                                             const AbstractStringKeeper *const keeper),

                //! create a new state in the target type system
                SkillFile *makeState(FileInputStream *in,
                                     WriteMode mode,
                                     StringPool *String,
                                     AnnotationType *Annotation,
                                     std::vector<AbstractStoragePool *> *types,
                                     api::typeByName_t *typesByName,
                                     std::vector<std::unique_ptr<MappedInStream>> &dataList)
        >
        SkillFile *newFile(const std::string &path, WriteMode mode) {
            FileInputStream *in = new FileInputStream(path, "w");
            StringPool *String = initializeStrings(in);
            std::vector<AbstractStoragePool *> *types =
                    new std::vector<AbstractStoragePool *>;
            AnnotationType *Annotation = new AnnotationType(types);
            api::typeByName_t *typesByName = new api::typeByName_t;
            std::vector<std::unique_ptr<MappedInStream>> dataList;

            return makeState(in, mode, String,
                             Annotation, types,
                             typesByName,
                             dataList);
        }


        /**
         * parses a skill file; parametrized by specification dependent functionality.
         */
        template<
                //!ensures that names of pools and known fields are known upfront, so that it is safe
                // to compare their names by pointer value
                StringPool *initializeStrings(FileInputStream *),

                //!create a new pool in the target type system
                AbstractStoragePool *newPool(TypeID typeID,
                String name,
                AbstractStoragePool *superPool,
                std::set<TypeRestriction *> *restrictions,
                const AbstractStringKeeper *const keeper ),

        //! create a new state in the target type system
        SkillFile *makeState(FileInputStream *in,
                             WriteMode mode,
                             StringPool *String,
                             AnnotationType *Annotation,
                             std::vector<AbstractStoragePool *> *types,
                             api::typeByName_t *typesByName,
                             std::vector<std::unique_ptr<MappedInStream>> &dataList)

        >

        SkillFile *parseFile(std::unique_ptr<FileInputStream> in, WriteMode mode) {
            struct LFEntry {
                LFEntry(AbstractStoragePool *const pool, SKilLID count)
                        : pool(pool), count(count) {}

                AbstractStoragePool *const pool;
                const SKilLID count;
            };

            // PARSE STATE
            std::unique_ptr<StringPool> String(initializeStrings(in.get()));
            std::vector<AbstractStoragePool *> *types =
                    new std::vector<AbstractStoragePool *>;
            std::unique_ptr<AnnotationType> Annotation(new AnnotationType(types));
            std::unique_ptr<api::typeByName_t> typesByName(new api::typeByName_t);
            std::vector<std::unique_ptr<MappedInStream>> dataList;

            // process stream
            debugOnly {
                std::cout << std::endl << "file " << in->getPath() << std::endl;
            }
            for (int blockCounter = 0; !in->eof(); blockCounter++) {
                debugOnly {
                    std::cout << "block " << blockCounter << " starting at " << in->getPosition() << std::endl;
                }

                // string block
                try {
                    const int count = (int) in->v64();
                    debugOnly {
                        std::cout << count << " strings" << std::endl;
                    }

                    if (0 != count) {
                        int last = 0, offset = 0;
                        const long position = in->getPosition() + 4 * count;
                        for (int i = count; i != 0; i--) {
                            offset = in->i32();
                            String->addPosition(std::pair<long, int>(position + last, offset - last));
                            last = offset;
                        }
                        in->jump(in->getPosition() + last);
                    }

                } catch (SkillException e) {
                    throw ParseException(in, blockCounter, "corrupted string block");
                }

                debugOnly {
                    std::cout << "string block ended at " << in->getPosition() << std::endl;
                }

                // type block
                try {
                    TypeID typeCount = (TypeID) in->v64();

                    // this barrier is strictly increasing inside of each block and reset to 0 at the beginning of each block
                    TypeID blockIDBarrier = 0;
                    std::set<api::String> seenTypes;

                    std::vector<AbstractStoragePool *> resizeQueue;

                    // number of fields to expect for that type in this block
                    std::vector<LFEntry> localFields;

                    // parse type definitions
                    while (typeCount-- > 0) {
                        api::String name = String->get((SKilLID) in->v64());

                        // check null name
                        if (nullptr == name)
                            throw ParseException(in, blockCounter,
                                                 "Corrupted file, nullptr in typename");

                        debugOnly {
                            std::cout << "processing type " << *name << " at " << in->getPosition()
                                      << std::endl;
                        }

                        // check duplicate types
                        if (seenTypes.find(name) != seenTypes.end())
                            throw ParseException(
                                    in, blockCounter,
                                    std::string("Duplicate definition of type ").append(*name));

                        seenTypes.insert(name);

                        const int count = (int) in->v64();

                        auto defIter = typesByName->find(name);
                        if (defIter == typesByName->end()) {
                            // unknown type

                            // type restrictions
                            int restrictionCount = (int) in->v64();
                            auto rest = std::unique_ptr<std::set<TypeRestriction *>>(new std::set<TypeRestriction *>);
                            //! TODO restrictions
                            // rest.sizeHint(restrictionCount)
                            while (restrictionCount-- > 0) {
                                switch ((char) in->v64()) {
                                    case 0: //restrictions.Unique
                                        break;
                                    case 1: // restrictions.Singleton
                                        break;
                                    case 2: // restrictions.Monotone
                                        break;
                                    case 3: // restrictions.Abstract
                                        break;
                                    case 5:
                                        in->v64(); // restrictions.DefaultTypeRestriction(in.v64.toInt)
                                        break;

                                    default:
                                        ParseException(
                                                in, blockCounter,
                                                "Found an unknown type restriction. Please regenerate your binding, if possible.");
                                }

                                // TODO rest +=
                            }

                            // super
                            const TypeID superID = (TypeID) in->v64();
                            AbstractStoragePool *superPool;
                            if (0 == superID)
                                superPool = nullptr;
                            else if (superID > (TypeID) types->size()) {
                                throw ParseException(
                                        in, blockCounter,
                                        std::string("Type ").append(*name).append(
                                                " refers to an ill-formed super type."));
                            } else {
                                superPool = types->at(superID - 1);
                                assert(superPool);
                            }

                            // allocate pool
                            AbstractStoragePool *r = newPool(
                                    (TypeID) types->size() + 32, name, superPool, rest.get(), String->keeper);
                            rest.release();

                            types->push_back(r);
                            defIter = typesByName->insert(
                                    std::pair<api::String, AbstractStoragePool *>(name, r)).first;
                        }
                        AbstractStoragePool *const definition = defIter->second;

                        if (blockIDBarrier < definition->typeID)
                            blockIDBarrier = definition->typeID;
                        else
                            throw ParseException(in, blockCounter, "Found unordered type block.");

                        // in contrast to prior implementation, bpo is the position inside of data, even if there are no actual
                        // instances. We need this behavior, because that way we can cheaply calculate the number of static instances
                        const SKilLID lbpo =
                                definition->basePool->cachedSize + (nullptr == definition->superPool ? 0 : (
                                        0 != count ? (SKilLID) in->v64() :
                                        definition->superPool->blocks.back().bpo));

                        // ensure that bpo is in fact inside of the parents block
                        if (definition->superPool) {
                            const auto &b = definition->superPool->blocks.back();
                            if (lbpo < b.bpo || b.bpo + b.dynamicCount < lbpo)
                                throw ParseException(in, blockCounter,
                                                     "Found broken bpo.");
                        }

                        // static count and cached size are updated in the resize phase
                        // @note we assume that all dynamic instance are static instances as well, until we know for sure
                        definition->blocks.push_back(Block(blockCounter, lbpo, count, count));
                        definition->staticDataInstances += count;

                        resizeQueue.push_back(definition);
                        localFields.push_back(LFEntry(definition, (SKilLID) in->v64()));
                    }

                    // resize pools, i.e. update cachedSize and staticCount
                    for (auto &p : resizeQueue) {
                        const auto &b = p->blocks.back();
                        p->cachedSize += b.dynamicCount;

                        if (0 != b.dynamicCount) {
                            // calculate static count of our parent
                            const auto &parent = p->superPool;
                            if (parent) {
                                auto &sb = parent->blocks.back();
                                // assumed static instances, minus what static instances would be, if p were the first sub pool.
                                const auto delta = sb.staticCount - (b.bpo - sb.bpo);
                                // if positive, then we have to subtract it from the assumed static count (local and global)
                                if (delta > 0) {
                                    sb.staticCount -= delta;
                                    parent->staticDataInstances -= delta;
                                }
                            }
                        }
                    }

                    // track offset information, so that we can create the block maps and jump to the next block directly after
                    // parsing field information
                    long dataEnd = 0L;

                    // parse fields
                    for (const auto &e : localFields) {
                        const auto &p = e.pool;
                        TypeID legalFieldIDBarrier = 1 + (TypeID) p->dataFields.size();
                        const auto &block = p->blocks.back();
                        auto localFieldCount = e.count;

                        while (localFieldCount-- > 0) {
                            const TypeID id = (TypeID) in->v64();
                            if (id <= 0 || legalFieldIDBarrier < id)
                                throw ParseException(in, blockCounter,
                                                     "Found an illegal field ID.");

                            long endOffset = 0;
                            if (id == legalFieldIDBarrier) {
                                // new field
                                legalFieldIDBarrier++;
                                const api::String fieldName = String->get((SKilLID) in->v64());
                                if (!fieldName)
                                    throw ParseException(in, blockCounter,
                                                         "A field has a nullptr as name.");

                                debugOnly {
                                    std::cout << "processing new field " << *p->name << "." << *fieldName
                                              << " at " << in->getPosition() << std::endl;
                                }

                                const auto t = parseFieldType(in.get(), types, String.get(), Annotation.get(),
                                                              blockCounter);

                                // parse field restrictions
                                std::set<const restrictions::FieldRestriction *> rest;
                                int fieldRestrictionCount = (int) in->v64();
                                for (; fieldRestrictionCount != 0; fieldRestrictionCount--) {
                                    const int i = (const int) in->v64();
                                    switch (i) {
                                        case 0: {// nonnull
                                            rest.insert(restrictions::NonNull::get());
                                            break;
                                        }

                                        case 1: {// default
                                            if (5 == t->typeID || 32 <= t->typeID)
                                                in->v64();
                                            else
                                                t->read(*in);
                                            break;
                                        }

                                        case 3: {
                                            //range
                                            switch (t->typeID) {
                                                case 7:
                                                    rest.insert(new restrictions::Range<int8_t>(in->i8(), in->i8()));
                                                    break;
                                                case 8:
                                                    rest.insert(new restrictions::Range<int16_t>(in->i16(), in->i16()));
                                                    break;
                                                case 9:
                                                    rest.insert(new restrictions::Range<int32_t>(in->i32(), in->i32()));
                                                    break;
                                                case 10:
                                                    rest.insert(new restrictions::Range<int64_t>(in->i64(), in->i64()));
                                                    break;
                                                case 11:
                                                    rest.insert(new restrictions::Range<int64_t>(in->v64(), in->v64()));
                                                    break;
                                                case 12:
                                                    rest.insert(new restrictions::Range<float>(in->f32(), in->f32()));
                                                    break;
                                                case 13:
                                                    rest.insert(new restrictions::Range<double>(in->f64(), in->f64()));
                                                    break;
                                                default:
                                                    throw ParseException(
                                                            in, blockCounter,
                                                            "Range restricton on a type that can not be restricted.");
                                            }
                                            break;
                                        }

                                        case 5: { // coding
                                            String->get((SKilLID) in->v64());
                                            break;
                                        }

                                        case 7: {
                                            // constant length pointer
                                            break;
                                        }

                                        case 9: {
                                            // oneof

                                            // read array of type IDs
                                            for (int c = in->v64(); c != 0; c--)
                                                in->v64();
                                            break;
                                        }

                                        default:
                                            throw ParseException(
                                                    in, blockCounter,
                                                    "Found an unknown field restriction. Please regenerate your binding, if possible.");
                                    }
                                }
                                endOffset = in->v64();

                                auto f = p->addField(String->keeper, id, t, fieldName);
                                for (auto r : rest)
                                    f->addRestriction(r);

                                f->addChunk(
                                        new BulkChunk(dataEnd, endOffset, p->cachedSize, p->blocks.size()));
                            } else {
                                // known field
                                endOffset = in->v64();
                                p->dataFields[id - 1]->addChunk(
                                        new SimpleChunk(dataEnd, endOffset, block.dynamicCount, block.bpo));
                            }
                            dataEnd = endOffset;
                        }
                    }

                    debugOnly {
                        std::cout << "reached end of type header at " << in->getPosition() << std::endl;
                    }


                    // jump over data and continue in the next block
                    dataList.push_back(std::unique_ptr<MappedInStream>(in->jumpAndMap(dataEnd)));
                } catch (SkillException e) {
                    throw e;
                } catch (...) {
                    throw ParseException(in, blockCounter, "unexpected foreign exception");
                }
            }

            // note there still isn't a single instance
            return makeState(in.release(), mode, String.release(), Annotation.release(), types,
                             typesByName.release(),
                             dataList);
        }

        /**
         * has to be called by make state after instances have been allocated to ensure
         * that required fields are read from file
         */
        inline void triggerFieldDeserialization(std::vector<AbstractStoragePool *> *types,
                                                std::vector<std::unique_ptr<MappedInStream>> &dataList) {

            std::vector<std::string *> results;

#pragma omp parallel for schedule(dynamic) num_threads(omp_get_max_threads()/2)
            for (size_t i = 0; i < types->size(); i++) {
                auto t = types->at(i);
#pragma omp parallel for schedule(dynamic) num_threads(2)
                for (size_t j = 0; j < t->dataFields.size(); j++) {
                    auto f = t->dataFields[j];

                    int bsIndex = 0;

                    for (Chunk *dc : f->dataChunks) {
                        if (dynamic_cast<BulkChunk *>(dc)) {
                            // skip blocks that do not contain data for our field
                            bsIndex += ((BulkChunk *) dc)->blockCount - 1;
                        }

                        const int blockIndex = t->blocks[bsIndex++].blockIndex;
                        if (dc->count) {
                            MappedInStream *in = dataList[blockIndex].get();
                            try {
                                f->read(in, dc);
                            } catch (SkillException e) {
#pragma omp critical
                                {
                                    results.push_back(new std::string(e.message));
                                }
                            } catch (...) {
#pragma omp critical
                                {
                                    results.push_back(new std::string("unknown error in concurrent read"));
                                }
                            }
                        }
                    }
                }
            }

            // check for errors
            if (results.size()) {
                std::stringstream msg;

                for (const auto s : results) {
                    if (s) {
                        msg << *s << std::endl;
                        delete s;
                    }
                }
                throw SkillException(msg.str());
            }
        }
    }
}

#undef debugOnly

#endif //SKILL_CPP_COMMON_FILEPARSER_H_H
