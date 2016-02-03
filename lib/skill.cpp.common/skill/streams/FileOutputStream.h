//
// Created by Timm Felden on 03.02.16.
//

#ifndef SKILL_CPP_COMMON_FILEOUTPUTSTREAM_H
#define SKILL_CPP_COMMON_FILEOUTPUTSTREAM_H

#include <string>
#include "Stream.h"

namespace skill {
    namespace streams {

        class MappedOutStream;

        /**
         * File out streams manages file; uses a buffer for its write operations.
         * Can create a map of correct size for mapped streams.
         *
         * @author Timm Felden
         */
        class FileOutputStream : public Stream {

            /**
             * the path where this stream was opened from
             */
            const std::string path;

            /**
             * the file object used for communication to the fs
             */
            const FILE *const file;

            /**
             * flush the buffer
             */
            void flush();

            inline void require(size_t i) {
                if (!has(i))
                    flush();
            }

            //! size of the backing buffer
            static constexpr size_t BUFFER_SIZE = 4096;
            //! the backing buffer (allocate plain to get rid of one pointer deref)
            uint8_t buffer[BUFFER_SIZE];

        public:

            /**
             * open the file at the target location
             *
             * @param append set to true, if content shall be appended to the file
             */
            FileOutputStream(const std::string& path, bool append);

            /**
             * close the stream
             */
            virtual ~FileOutputStream();

            /**
            * Maps from current position until offset.
            *
            * @return a buffer that has exactly offset many bytes remaining
            */
            MappedOutStream *jumpAndMap(long offset);

            inline void i8(int8_t v) {
                require(1);
                *(position++) = (uint8_t) v;
            }

            inline void i16(int16_t v) {
                require(2);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i32(int32_t v) {
                require(4);
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i64(int64_t v) {
                require(8);
                *(position++) = (uint8_t) (v >> 56);
                *(position++) = (uint8_t) (v >> 48);
                *(position++) = (uint8_t) (v >> 40);
                *(position++) = (uint8_t) (v >> 32);
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void v64(int64_t v) {
                require(9);
                if (0L == (v & 0xFFFFFFFFFFFFFF80L)) {
                    *(position++) = (uint8_t) (v);
                } else {
                    *(position++) = (uint8_t) ((0x80L | v));
                    if (0L == (v & 0xFFFFFFFFFFFFC000L)) {
                        *(position++) = (uint8_t) ((v >> 7));
                    } else {
                        *(position++) = (uint8_t) ((0x80L | v >> 7));
                        if (0L == (v & 0xFFFFFFFFFFE00000L)) {
                            *(position++) = (uint8_t) ((v >> 14));
                        } else {
                            *(position++) = (uint8_t) ((0x80L | v >> 14));
                            if (0L == (v & 0xFFFFFFFFF0000000L)) {
                                *(position++) = (uint8_t) ((v >> 21));
                            } else {
                                *(position++) = (uint8_t) ((0x80L | v >> 21));
                                if (0L == (v & 0xFFFFFFF800000000L)) {
                                    *(position++) = (uint8_t) ((v >> 28));
                                } else {
                                    *(position++) = (uint8_t) ((0x80L | v >> 28));
                                    if (0L == (v & 0xFFFFFC0000000000L)) {
                                        *(position++) = (uint8_t) ((v >> 35));
                                    } else {
                                        *(position++) = (uint8_t) ((0x80L | v >> 35));
                                        if (0L == (v & 0xFFFE000000000000L)) {
                                            *(position++) = (uint8_t) ((v >> 42));
                                        } else {
                                            *(position++) = (uint8_t) ((0x80L | v >> 42));
                                            if (0L == (v & 0xFF00000000000000L)) {
                                                *(position++) = (uint8_t) ((v >> 49));
                                            } else {
                                                *(position++) = (uint8_t) ((0x80L | v >> 49));
                                                *(position++) = (uint8_t) ((v >> 56));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_FILEOUTPUTSTREAM_H
