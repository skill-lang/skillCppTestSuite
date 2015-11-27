//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_ABSTRACTSTORAGEPOOL_H
#define SKILL_CPP_COMMON_ABSTRACTSTORAGEPOOL_H

#include <vector>

#include "../common.h"
#include "FileStructure.h"
#include "FieldDeclaration.h"
#include "../fieldTypes/FieldType.h"
#include "../utils.h"
#include "../fieldTypes/BuiltinFieldType.h"
#include "../restrictions/TypeRestriction.h"

namespace skill {
    namespace api {
        class SkillFile;
    }
    using restrictions::TypeRestriction;
    namespace internal {
/**
 * this class reflects all storage pool properties, that do not depend on types
 *
 * @note let us assume, that the Abstract storage pool is of type T and its base pool is of type B
 */
        class AbstractStoragePool : public fieldTypes::FieldType {
        protected:
            AbstractStoragePool(TypeID typeID, AbstractStoragePool *superPool,
                                api::String const name, std::set<TypeRestriction *> *restrictions);

            friend class api::SkillFile;

            //! only skill files can delete pools
            virtual ~AbstractStoragePool();

            virtual SKilLID newObjectsSize() const = 0;

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
             * @note we type target using the erasure directly, because the Java type system is too weak to express correct
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
             * in fact Pool[? <: T,B]
             */
            std::vector<AbstractStoragePool *> subPools;

            /**
             * create an anonymous subtype
             */
            virtual AbstractStoragePool *makeSubPool(skill::TypeID typeID,
                                                     skill::api::String name,
                                                     std::set<TypeRestriction *> *restrictions) = 0;

            //! internal use only
            SKilLID cachedSize = 0;
            /**
             * can be used to fix states, thereby making some operations (dynamic size) cacheable
             *
             * no instances can be added or deleted in a fixed state
             */
        private:
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
                        SK_TODO;
                        /*cachedSize = staticSize - deletedCount;
                        for(auto s : subPools)
                            s->fix(true);
                            cachedSize += s->cachedSize;
                        }*/
                    } else if (superPool) {
                        superPool->fix(false);
                    }
                }
            }

            //! internal use only
            SKilLID staticDataInstances = 0;

            //! internal use only
            std::vector<Block> blocks;


            //! internal use only
            std::vector<FieldDeclaration *> dataFields;

            //!internal use only
            virtual FieldDeclaration *addField(TypeID id, const FieldType *type, api::String name) = 0;

            virtual api::Box read(streams::MappedInStream &in) const {
                SK_TODO;
            }

            virtual uint64_t offset(api::Box &target) const {
                return fieldTypes::V64.offset(target.annotation->id);
            }

            virtual void write(outstream &out, api::Box &target) const {
                SK_TODO;
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_ABSTRACTSTORAGEPOOL_H
