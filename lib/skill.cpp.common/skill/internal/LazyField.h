//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_LAZYFIELD_H
#define SKILL_CPP_COMMON_LAZYFIELD_H


#include "DistributedField.h"

namespace skill {
    namespace internal {
        class LazyField : public DistributedField {

        public:
            LazyField(const FieldType *const type, const api::string_t *name,
                      AbstractStoragePool *const owner)
                    : DistributedField(type, name, owner) { }

            virtual ~LazyField() { }

            virtual api::Box getR(const api::Object *i);

            virtual void setR(api::Object *i, api::Box v);

            virtual void read(const streams::MappedInStream *in, const Chunk *target);
        };
    }
}


#endif //SKILL_CPP_COMMON_LAZYFIELD_H
