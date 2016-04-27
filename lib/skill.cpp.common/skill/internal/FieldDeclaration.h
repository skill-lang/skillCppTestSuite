//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_FIELDDECLARATION_H
#define SKILL_CPP_COMMON_FIELDDECLARATION_H

#include "FileStructure.h"
#include "../api/AbstractField.h"
#include "../restrictions/FieldRestriction.h"
#include "../concurrent/ThreadPool.h"
#include <vector>
#include <unordered_set>
#include <future>

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
                    : AbstractField(type, name), index(index), owner(owner) { }

            inline SkillException ParseException(long position, long begin, long end, const std::string &msg) {
                std::stringstream message;
                message << "ParseException while parsing field.\n Position" << position
                << "\n inside " << begin << ", " << end << "\n reason: "
                << msg << std::endl;
                return SkillException(message.str());
            }

        public:
            virtual ~FieldDeclaration();

            //! internal use only!
            std::vector<Chunk *> dataChunks;

            void addChunk(Chunk *c) {
                dataChunks.push_back(c);
            }

            virtual void resetChunks(SKilLID newSize) {
                for (auto c : dataChunks)
                    delete c;
                dataChunks.clear();
                dataChunks.push_back(new BulkChunk(-1, -1, newSize, 1));
            }

            /**
             * Restriction handling.
             */
        protected:
            std::unordered_set<const restrictions::CheckableRestriction *> checkedRestrictions;
            std::unordered_set<const restrictions::FieldRestriction *> otherRestrictions;
        public:
            virtual size_t offset() const = 0;

            //std::future<size_t> offsetResult;
            // offset cache for now
            // TODO replace by fixed async code
            size_t awaitOffset;
        //protected:
            virtual void write(streams::MappedOutStream *out) const = 0;

        public:
            void addRestriction(const restrictions::FieldRestriction *r);

            /**
             * Check restrictions of the field.
             *
             * @return true, iff field fulfills all restrictions
             */
            virtual bool check() const = 0;

            /**
             * Read data from a mapped input stream and set it accordingly. This is invoked at the very end of state
             * construction and done massively in parallel.
             */
            virtual void read(const streams::MappedInStream *in, const Chunk *target) = 0;

            //! start offset calculation
            /*void asyncOffset() {
                offsetResult = concurrent::ThreadPool::global.execute([this]() {
                    return this->offset();
                });
            }

            //! await result of offset calculation
            size_t awaitOffset() {
                return offsetResult.get();
            }*/

            //! start write job
            /*std::future<void> asyncWrite(const streams::MappedOutStream *map) {
                return concurrent::ThreadPool::global.execute([this](const streams::MappedOutStream *map) {
                    Chunk *c = dataChunks.back();
                    return this->write(map->clone(c->begin, c->end));
                }, map);
            }*/
        };
    }
}


#endif //SKILL_CPP_COMMON_FIELDDECLARATION_H
