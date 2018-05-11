//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_SKILLFILE_H
#define SKILL_CPP_COMMON_SKILLFILE_H

#include <unordered_map>
#include <memory>
#include "String.h"
#include "../internal/AbstractStoragePool.h"
#include "../streams/FileInputStream.h"
#include "StringAccess.h"

namespace skill {
    namespace internal {
        class FileWriter;
    }

    namespace api {

        enum ReadMode {
            create, read
        };

        enum WriteMode {
            write, append, readOnly
        };

        /**
         * the type of the type by name mapping
         */
        typedef std::unordered_map<
                api::String,
                internal::AbstractStoragePool *,
                equalityHash,
                equalityEquals> typeByName_t;


        /**
         * spec independent public API for skill files
         */
        class SkillFile {
            friend class skill::internal::FileWriter;

        public:
            api::StringAccess *const strings;
        protected:
            fieldTypes::AnnotationType *const annotation;
        public :
            const fieldTypes::AnnotationType *const getAnnotationType() const {
                return annotation;
            }

        protected:


            /**
             * types managed by this file.
             *
             * heap allocated array of pools
             */
            internal::AbstractStoragePool *const *const types;

            /**
             * typename -> type mapping
             */
            const typeByName_t *const typesByName;

            /**
             * the file we read from
             * @note null, iff no file was read
             * @note owned by this
             */
            streams::FileInputStream *const fromFile;

            /**
             * our current path to write to
             */
            std::string currentWritePath;

            /**
             * current mode of this file
             */
            WriteMode mode;

            SkillFile(streams::FileInputStream *in,
                      WriteMode mode,
                      internal::StringPool *stringPool,
                      fieldTypes::AnnotationType *annotation,
                      std::vector<internal::AbstractStoragePool *> *types,
                      typeByName_t *typesByName);

            void ensureFields() {
                for (auto t : *this)
                    t->complete(this);
            }

        public:
            /**
             * Will release resources of this file, but will *NOT* write changes to disk!
             */
            virtual ~SkillFile();

            /**
             * Set a new path for the file. This will influence the next flush/close operation.
             *
             * @throws IOException
             *             if new path can not be used for some reason
             * @note (on implementation) memory maps for lazy evaluation must have been created before invocation of this method
             */
            void changePath(std::string path);

            /**
             * @return the current path pointing to the file
             */
            const std::string &currentPath() const;

            /**
             * Set a new mode.
             *
             * @note not fully implemented
             */
            void changeMode(WriteMode writeMode);

            /**
             * Checks consistency of the current state of the file.
             *
             * @note if check is invoked manually, it is possible to fix the inconsistency and re-check without breaking the
             *       on-disk representation
             * @throws SkillException
             *             if an inconsistency is found
             */
            void check();

            /**
             * Check consistency and write changes to disk.
             *
             * @note this will not sync the file to disk, but it will block until all in-memory changes are written to buffers.
             * @note if check fails, then the state is guaranteed to be unmodified compared to the state before flush
             * @throws SkillException
             *             if check fails
             */
            void flush();

            /**
             * Same as flush, changeMode(readOnly).
             */
            void close();

            /**
             * start iteration over types in the state
             */
            internal::AbstractStoragePool *const *begin() const {
                return types;
            }

            /**
             * end iteration over types in the state
             */
            internal::AbstractStoragePool *const *end() const {
                return types + size();
            }

            /**
             * number of types in the file
             */
            size_t size() const {
                return typesByName->size();
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_SKILLFILE_H
