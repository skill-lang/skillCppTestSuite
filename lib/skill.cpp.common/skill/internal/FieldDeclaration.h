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
        protected:
            FieldDeclaration(const FieldType *const type, api::String const name)
                    : AbstractField(type, name) { }

        public:
            virtual ~FieldDeclaration() { };

            void addChunk(Chunk *c) {
                //! TODO not a solution;)
                delete c;
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_FIELDDECLARATION_H
