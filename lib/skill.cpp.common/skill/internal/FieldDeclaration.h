//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_FIELDDECLARATION_H
#define SKILL_CPP_COMMON_FIELDDECLARATION_H

#include "FileStructure.h"
#include "../api/AbstractField.h"

namespace skill {
    namespace internal {

        /**
         * internal view onto field RTTI
         *
         * @author Timm Felden
         */
        class FieldDeclaration : public api::AbstractField {
        public:
            /**
             * reflective access to the enclosing type
             */
            AbstractStoragePool *const owner;
        protected:
            FieldDeclaration(const FieldType *const type, api::String const name,
                             AbstractStoragePool *const owner)
                    : AbstractField(type, name), owner(owner) { }

        public:
            virtual ~FieldDeclaration() { };

            //! internal use only!
            std::vector<Chunk *> dataChunks;

            void addChunk(Chunk *c) {
                dataChunks.push_back(c);
            }

            /**
             * Read data from a mapped input stream and set it accordingly. This is invoked at the very end of state
             * construction and done massively in parallel.
             */
            virtual void read(const streams::MappedInStream *in, const Chunk *target) = 0;
        };
    }
}


#endif //SKILL_CPP_COMMON_FIELDDECLARATION_H
