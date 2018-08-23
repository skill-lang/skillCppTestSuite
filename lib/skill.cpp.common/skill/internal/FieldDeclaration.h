//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_FIELDDECLARATION_H
#define SKILL_CPP_COMMON_FIELDDECLARATION_H

#include "FileStructure.h"
#include "../api/AbstractField.h"
#include "../restrictions/FieldRestriction.h"
#include <vector>
#include <unordered_set>

namespace skill {
    namespace internal {

        /**
         * internal view onto field RTTI
         *
         * @author Timm Felden
         */
        class FieldDeclaration : public api::AbstractField {
        public:
            //! skill field index
            const TypeID index;

            /**
             * reflective access to the enclosing type
             */
            AbstractStoragePool *const owner;
        protected:
            FieldDeclaration(const FieldType *const type, api::String const name, const TypeID index,
                             AbstractStoragePool *const owner)
                    : AbstractField(type, name), index(index), owner(owner) {}

            inline SkillException ParseException(long position, long begin, long end, const std::string &msg) {
                std::stringstream message;
                message << "ParseException while parsing field.\n Position" << position
                        << "\n inside " << begin << ", " << end << "\n reason: "
                        << msg << std::endl;
                return SkillException(message.str());
            }

            inline SKilLID skillID(const api::Object *ref) const {
                return ref->id;
            }

        public:
            virtual ~FieldDeclaration();

            //! internal use only!
            std::vector<Chunk *> dataChunks;

            void addChunk(Chunk *c) {
                dataChunks.push_back(c);
            }

            virtual void resetChunks(SKilLID lbpo, SKilLID newSize) {
                for (auto c : dataChunks)
                    delete c;
                dataChunks.clear();
                dataChunks.push_back(new SimpleChunk(-1, -1, newSize, lbpo));
            }

            /**
             * Restriction handling.
             */
        protected:
            std::unordered_set<const restrictions::CheckableRestriction *> checkedRestrictions;
            std::unordered_set<const restrictions::FieldRestriction *> otherRestrictions;

        public:

            /**
             * @return true, iff there are checked restrictions
             */
            bool hasRestrictions() {
                return 0 != checkedRestrictions.size();
            }

            void addRestriction(const restrictions::FieldRestriction *r);

            /**
             * Check restrictions of the field.
             *
             * @return true, iff field fulfills all restrictions
             */
            virtual bool check() const = 0;

            /**
             * Read data from a mapped input stream and set it accordingly. This is invoked at the very end of state
             * construction and done in parallel.
             *
             * @param i current index of object to be processed
             * @param h stop index, i.e. process until i==h
             * @param in stream to be read
             */
            virtual void rsc(SKilLID i, const SKilLID h, streams::MappedInStream *in) = 0;

            /**
             * Read data from a mapped input stream and set it accordingly. This is invoked at the very end of state
             * construction and done in parallel.
             */
            virtual void rbc(streams::MappedInStream *in, const BulkChunk *target);

            /**
             * Calculate offset for a simple chunk.
             *
             * @note we do not append. Hence, there cannot be any bulk chunks on write
             */
            virtual size_t osc() const = 0;

            //! offset cache for now
            //! TODO replace by fixed async code
            size_t awaitOffset;

            /**
             * write data belonging to this field to the stream
             *
             * @note we do not append. Hence, there cannot be any bulk chunks on write
             */
            virtual void wsc(streams::MappedOutStream *out) const = 0;
        };
    }
}


#endif //SKILL_CPP_COMMON_FIELDDECLARATION_H
