//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_SKILLFILE_H
#define SKILL_CPP_COMMON_SKILLFILE_H

#include <unordered_map>
#include "String.h"
#include "../internal/AbstractStoragePool.h"
#include "../streams/FileInputStream.h"

namespace skill {
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
        protected:
            internal::StringPool *const strings;

            /**
             * types managed by this file
             */
            const std::vector<internal::AbstractStoragePool *> *const types;

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

            SkillFile(streams::FileInputStream *string, WriteMode mode, internal::StringPool *pPool,
                      std::vector<internal::AbstractStoragePool *> *pVector, typeByName_t *pMap);

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
            const std::string &currentPath();

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
            void check() { };

            /**
             * Check consistency and write changes to disk.
             *
             * @note this will not sync the file to disk, but it will block until all in-memory changes are written to buffers.
             * @note if check fails, then the state is guaranteed to be unmodified compared to the state before flush
             * @throws SkillException
             *             if check fails
             */
            void flush() { };

            /**
             * Same as flush, but will also sync and close file, thus the state must not be used afterwards.
             */
            void close() { };
        };
    }
}

#endif //SKILL_CPP_COMMON_SKILLFILE_H
