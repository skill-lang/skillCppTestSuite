//
// Created by Timm Felden on 01.12.15.
//

#ifndef SKILL_CPP_COMMON_ANNOTATIONTYPE_H
#define SKILL_CPP_COMMON_ANNOTATIONTYPE_H

#include "../internal/AbstractStoragePool.h"
#include "BuiltinFieldType.h"
#include "../api/SkillFile.h"
#include "../internal/StringPool.h"
#include <memory>

namespace skill {
    using streams::InStream;
    namespace fieldTypes {

        /**
         * the implementation of annotations may not be fast (at all)
         *
         * @note maybe, the skill names returned by instances should point to
         * their pool's references, so that we can create an internal mapping from
         * const char* ⇒ type, which is basically a perfect hash map, because strings
         * will then be unique and we no longer have to care for identical copies
         */
        class AnnotationType : public BuiltinFieldType<api::Object *, 5> {
            std::vector<std::unique_ptr<internal::AbstractStoragePool>> *const types;
            /**
             * annotations will not be queried from outside, thus we can directly use char* obtained from
             * skill string pointers
             */
            mutable std::unordered_map<const char *, internal::AbstractStoragePool *> typesByName;

        public:
            AnnotationType(std::vector<std::unique_ptr<internal::AbstractStoragePool>> *types)
                    : types(types), typesByName() { }

            virtual ~AnnotationType() { }

            /**
             * initialize the annotation type, i.e. establishing the typesByName mapping
             *
             * @return this, so that init can be used in a constructor
             */
            AnnotationType *init() {
                for (const auto &t : *types) {
                    typesByName[t->name->c_str()] = t.get();
                }
                return this;
            }

            /**
             * map a type name to a pool
             *
             * @note intended for internal use
             */
            internal::AbstractStoragePool *type(const char *name) {
                return typesByName[name];
            }


            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                TypeID t = (TypeID) in.v64();
                SKilLID id = (SKilLID) in.v64();
                r.annotation = t ? types->at(t)->getAsAnnotation(id) : nullptr;

                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                if (target.annotation) {
                    return V64FieldType::offset(typesByName[target.annotation->skillName()]->typeID)
                           + V64FieldType::offset(target.annotation->id);
                } else
                    return 2;
            }

            virtual void write(outstream &out, api::Box &target) const {
                SK_TODO;
            }

            virtual bool requiresDestruction() const {
                return false;
            }
        };
    }
}
#endif //SKILL_CPP_COMMON_ANNOTATIONTYPE_H
