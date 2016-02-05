//
// Created by feldentm on 03.11.15.
//

#ifndef SKILL_CPP_COMMON_FILEINPUTSTREAM_H
#define SKILL_CPP_COMMON_FILEINPUTSTREAM_H

#include <string>

#include "InStream.h"
#include "MappedInStream.h"

namespace skill {
    namespace streams {

        class FileInputStream : public InStream {

            /**
             * the path where this stream was opened from
             */
            const std::string path;

            /**
             * the file object used for communication to the fs
             */
            const FILE *const file;

            /**
             * required for replacing begin and end after map
             */
            FileInputStream(void *begin, void *end, const std::string &path, const FILE *file);

        public:

            /**
             * open the file at the target location
             */
            FileInputStream(const std::string path, const char *openMode = "r");

            /**
             * close the stream
             */
            virtual ~FileInputStream();

            /**
             * tell the caller which file we belong to
             */
            const std::string &getPath() {
                return path;
            }

            /**
            * Maps from current position until offset.
            *
            * @return a buffer that has exactly offset many bytes remaining
            */
            MappedInStream *jumpAndMap(long offset) {
                auto r = new MappedInStream(base, position, position + offset);
                position += offset;
                return r;
            }

            /**
             * skip a part of the file
             */
            void jump(long offset) {
                position = (uint8_t *) base + offset;
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_FILEINPUTSTREAM_H
