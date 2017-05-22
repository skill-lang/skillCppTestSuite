//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_LAZYFIELD_H
#define SKILL_CPP_COMMON_LAZYFIELD_H

#include <map>
#include "DistributedField.h"

namespace skill {
    namespace internal {
        class LazyField : public DistributedField {

            // pending parts that have to be loaded
            std::map<const Chunk *, streams::MappedInStream *> *parts;

            inline bool isLoaded() { return nullptr == parts; }

            void load();

        public:
            LazyField(const FieldType *const type, const api::string_t *name,
                      const TypeID index, AbstractStoragePool *const owner)
                    : DistributedField(type, name, index, owner),
                      parts(new std::map<const Chunk *, streams::MappedInStream *>) {}

            virtual ~LazyField();

            virtual void resetChunks(SKilLID lbpo, SKilLID newSize) {
                ensureIsLoaded();
                DistributedField::resetChunks(lbpo, newSize);
            }

            inline void ensureIsLoaded() {
                if (!isLoaded())
                    load();
            }

            virtual api::Box getR(const api::Object *i);

            virtual void setR(api::Object *i, api::Box v);

            virtual void read(const streams::MappedInStream *in, const Chunk *target);

            virtual bool check() const;
        };
    }
}


#endif //SKILL_CPP_COMMON_LAZYFIELD_H
