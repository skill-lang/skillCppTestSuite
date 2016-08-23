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
            /**
             * @note pools in this vector are owned by annotation, as long as typesByName is empty.
             * That way, we can omit unique_pointers.
             */
            std::vector<internal::AbstractStoragePool *> *const types;
            /**
             * annotations will not be queried from outside, thus we can directly use char* obtained from
             * skill string pointers
             */
            mutable std::unordered_map<const char *, internal::AbstractStoragePool *> typesByName;

        public:
            AnnotationType(std::vector<internal::AbstractStoragePool *> *types)
                    : types(types), typesByName() {}

            virtual ~AnnotationType() {
                for (auto p : *types)
                    delete p;
                delete types;
            }

            /**
             * initialize the annotation type, i.e. establishing the typesByName mapping
             *
             * @return the type by name mapping is returned, so that the make state code can use and update it to get
             * faster access to types by name
             */
            std::unordered_map<const char *, internal::AbstractStoragePool *> &init() {
                for (auto t : *types) {
                    typesByName[t->name->c_str()] = t;
                }
                return this->typesByName;
            }

            /**
             * map a type name to a pool
             *
             * @note intended for internal use
             */
            internal::AbstractStoragePool *type(const char *name) const {
                return typesByName[name];
            }


            virtual api::Box read(streams::InStream &in) const {
                api::Box r;
                TypeID t = (TypeID) in.v64();
                SKilLID id = (SKilLID) in.v64();
                r.annotation = t ? types->at(t - 1)->getAsAnnotation(id) : nullptr;

                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                if (target.annotation) {
                    return V64FieldType::offset(typesByName[target.annotation->skillName()]->typeID)
                           + V64FieldType::offset(target.annotation->id);
                } else
                    return 2;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                if (target.annotation) {
                    out->v64(typesByName[target.annotation->skillName()]->typeID);
                    out->v64(target.annotation->id);
                } else {
                    out->i16(0);
                }
            }

            virtual bool requiresDestruction() const {
                return false;
            }
        };
    }
}
#endif //SKILL_CPP_COMMON_ANNOTATIONTYPE_H
