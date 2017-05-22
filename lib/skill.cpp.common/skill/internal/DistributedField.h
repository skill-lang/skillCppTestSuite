//
// Created by Timm Felden on 28.01.16.
//

#ifndef SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H
#define SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H

#include "FieldDeclaration.h"
#include "../streams/SparseArray.h"
#include <unordered_map>

namespace skill {
    namespace internal {

        class DistributedField : public FieldDeclaration {
        protected:
            // data held as in storage pools
            // @note see paper notes for O(1) implementation
            mutable streams::SparseArray<api::Box> data;
            mutable std::unordered_map<const api::Object *, api::Box> newData;

        public:
            DistributedField(const FieldType *const type, const api::string_t *name,
                             const TypeID index, AbstractStoragePool *const owner)
                    : FieldDeclaration(type, name, index, owner), data(), newData() { }

            virtual ~DistributedField();

            virtual void resetChunks(SKilLID newSize, SKilLID i);

            virtual api::Box getR(const api::Object *i);

            virtual void setR(api::Object *i, api::Box v);

            virtual void read(const streams::MappedInStream *in, const Chunk *target);

            virtual size_t offset() const;

            virtual void write(streams::MappedOutStream* out) const;

            virtual bool check() const;

        private:
            // destructor helper method
            inline void deleteBoxedContainer(api::Box b, const FieldType *const typeID);
        };
    }
}


#endif //SKILL_CPP_COMMON_DISTRIBUTEDFIELD_H
