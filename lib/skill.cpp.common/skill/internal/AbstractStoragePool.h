//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_ABSTRACTSTORAGEPOOL_H
#define SKILL_CPP_COMMON_ABSTRACTSTORAGEPOOL_H

#include <vector>
#include <assert.h>

#include "../common.h"
#include "FileStructure.h"
#include "FieldDeclaration.h"
#include "../fieldTypes/FieldType.h"
#include "../utils.h"
#include "../restrictions/TypeRestriction.h"
#include "../fieldTypes/BuiltinFieldType.h"
#include "StringPool.h"

namespace skill {
    namespace api {
        class SkillFile;
    }
    namespace iterators {
        class AllObjectIterator;
    }
    using restrictions::TypeRestriction;
    namespace internal {
        class FileWriter;

        template<typename T>
        class BasePool;

/**
 * this class reflects all storage pool properties, that do not depend on types
 *
 * @note let us assume, that the Abstract storage pool is of type T and its base pool is of type B
 */
        class AbstractStoragePool : public fieldTypes::FieldType {
            friend class FileWriter;

            template<typename T>
            friend
            class BasePool;

        protected:
            AbstractStoragePool(TypeID typeID, AbstractStoragePool *superPool,
                                api::String const name, std::set<TypeRestriction *> *restrictions);

        public:
            virtual ~AbstractStoragePool();

        protected:

            friend class api::SkillFile;


            virtual SKilLID newObjectsSize() const = 0;

            virtual void clearNewObjects() = 0;

            SKilLID newDynamicInstancesSize() const;

            /**
             * number of deleted instances currently stored in this pool
             */
            SKilLID deletedCount = 0;

            /**
             * Delete shall only be called from skill state
             *
             * @param target
             *            the object to be deleted
             * @note we type target using the erasure directly, because the C++ type system is too weak to express correct
             *       typing, when taking the pool from a map
             */
            inline void free(api::Object *target) {
                // @note we do not need null check or 0 check, because both happen in SkillState
                target->id = 0;
                deletedCount++;
            }

        public:
            /**
             * ensure that data is set correctly
             *
             * @note internal use only!
             */
            virtual void allocateData() = 0;

            /**
             * ensure that instances are created correctly
             *
             * @note internal use only!
             * @note will parallelize over blocks and can be invoked in parallel
             */
            virtual void allocateInstances() = 0;

            /**
             * returns an instance by skillID, without further knowledge of the type
             * of the returned instance.
             *
             * @note if there is static knowledge that the pool is of type T, a cast to T* is always safe
             */
            virtual api::Object *getAsAnnotation(SKilLID id) const = 0;

            /**
             * restrictions of this pool
             */
            std::set<TypeRestriction *> *const restrictions;

            /**
             * the name of this pool
             */
            const api::String name;

            /**
             * offset into the type array
             */
            const int poolOffset() const {
                return typeID - 32;
            }

            /**
             * in fact a Pool[? >: T,B]
             */
            AbstractStoragePool *const superPool;

            /**
             * in fact a BasePool[B]
             */
            AbstractStoragePool *const basePool;

            /**
             * distance to the base type
             */
            const int typeHierarchyHeight;

            /**
             * in fact Pool[? <: T,B]
             */
            std::vector<AbstractStoragePool *> subPools;

            /**
             * a pointer to the next pool in a type order traversal
             *
             * @note in fact StoragePool[_ <: B, B]
             */
            AbstractStoragePool *nextPool;

            //! calculate next references
            static void setNextPools(AbstractStoragePool *base) {
                assert(nullptr == base->superPool);
                base->setNextPool(nullptr);
            }

        private:
            //! initialize the next pointer
            void setNextPool(AbstractStoragePool *nx) {
                if (subPools.size()) {
                    nextPool = subPools[0];
                    for (size_t i = 0; i < subPools.size() - 1; i++)
                        subPools[i]->setNextPool(subPools[i + 1]);

                    subPools[subPools.size() - 1]->setNextPool(nx);
                } else
                    nextPool = nx;
            }

        protected:
            SkillFile *owner;

        public:
            SkillFile *getOwner() const {
                return owner;
            }

        protected:

            //! called in owner constructor to complete creation of this pool
            virtual void complete(SkillFile *owner);

        public:

            /**
             * create an anonymous subtype
             */
            virtual AbstractStoragePool *makeSubPool(skill::TypeID typeID,
                                                     skill::api::String name,
                                                     std::set<TypeRestriction *> *restrictions) = 0;

            //! internal use only
            SKilLID cachedSize = 0;
        private:

            /**
             * can be used to fix states, thereby making some operations (dynamic size) cacheable
             *
             * no instances can be added or deleted in a fixed state
             */
            bool fixed = false;

        public:

            /**
             * sets fixed for this pool; automatically adjusts sub/super pools
             *
             * @note takes deletedCount into account, thus the size may decrease by fixing
             */
            void fix(bool setFixed) {
                // only do something if there is action required
                if (setFixed != fixed) {
                    fixed = setFixed;
                    if (fixed) {
                        cachedSize = staticSize() - deletedCount;
                        for (auto s : subPools) {
                            s->fix(true);
                            cachedSize += s->cachedSize;
                        }
                    } else if (superPool) {
                        superPool->fix(false);
                    }
                }
            }

        private:
            //! only implemented by base pools; this is a tribute to lack of wildcards
            virtual void compress(SKilLID *lbpoMap) = 0;

        protected:
            void updateAfterCompress(SKilLID *lbpoMap);

        public:

            //! internal use only
            SKilLID staticDataInstances = 0;

            /**
             * the size of this pool including new object, but excluding subpools
             */
            SKilLID staticSize() const {
                return staticDataInstances + newObjectsSize();
            }

            /**
             * Returns instances as api::SkillObjects; required for reflection
             */
            virtual std::unique_ptr<iterators::AllObjectIterator> allObjects() const = 0;

            /**
             * the size of this pool, including subpools and new objects
             */
            SKilLID size() const {
                if (fixed)
                    return cachedSize;
                else {
                    auto r = staticSize();
                    for (auto sub : subPools)
                        r += sub->size();

                    return r;
                }
            }

            //! internal use only
            std::vector<Block> blocks;


            //! internal use only
            std::vector<FieldDeclaration *> dataFields;

            //!internal use only
            virtual FieldDeclaration *addField(const AbstractStringKeeper *const keeper,
                                               TypeID id, const FieldType *type, api::String name) = 0;

            virtual api::Box read(streams::InStream &in) const;

            virtual uint64_t offset(const api::Box &target) const {
                const auto v = target.annotation;
                return v ? fieldTypes::V64FieldType::offset(v->id) : 1;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                const auto v = target.annotation;
                if (v)
                    out->v64(target.annotation->id);
                else
                    out->i8(0);
            }

            /**
             * destroyed by the skill file
             */
            virtual bool requiresDestruction() const {
                return false;
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_ABSTRACTSTORAGEPOOL_H
