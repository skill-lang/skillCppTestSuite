//
// Created by Timm Felden on 28.01.16.
//

#ifndef SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H
#define SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H

#include "FieldDeclaration.h"
#include <unordered_map>

namespace skill {
    namespace internal {

        class DistributedField : public FieldDeclaration {
        protected:
            // data held as in storage pools
            // @note see paper notes for O(1) implementation
            std::unordered_map<const api::Object *, api::Box> data; //Array[T]()
            std::unordered_map<const api::Object *, api::Box> newData;

        public:
            DistributedField(const FieldType *const type, const api::string_t *name,
                             AbstractStoragePool *const owner)
                    : FieldDeclaration(type, name, owner), data(), newData() { }

            virtual ~DistributedField() { }

            virtual api::Box getR(const api::Object *i);

            virtual void setR(api::Object *i, api::Box v);

            virtual void read(const streams::MappedInStream *in, const Chunk *target);

            virtual bool check() const;
        };
    }
}


#endif //SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H
