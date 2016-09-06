//
// Created by Timm Felden on 03.02.16.
//

#ifndef SKILL_CPP_COMMON_MAPPEDOUTSTREAM_H
#define SKILL_CPP_COMMON_MAPPEDOUTSTREAM_H

#include "Stream.h"
#include "../api/String.h"

namespace skill {
    namespace streams {

        class FileOutputStream;

        /**
         * @author Timm Felden
         */
        struct MappedOutStream : public Stream {
            friend class FileOutputStream;

            MappedOutStream(void *begin, void *end) : Stream(begin, end) {}

            virtual ~MappedOutStream() {}

            /**
             * create a clone of this outstream, that is to be deleted by the caller
             * and that will have virtually ranges between begin and end.
             * @note DO NOT unmap result
             */
            MappedOutStream *clone(size_t begin, size_t end) const {
                return new MappedOutStream(position + begin, position + end);
            }

            inline void boolean(bool v) {
                *(position++) = v ? 0xff : 0;
            }

            inline void i8(int8_t v) {
                *(position++) = (uint8_t) v;
            }

            inline void i16(int16_t v) {
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i32(int32_t v) {
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i64(int64_t v) {
                *(position++) = (uint8_t) (v >> 56);
                *(position++) = (uint8_t) (v >> 48);
                *(position++) = (uint8_t) (v >> 40);
                *(position++) = (uint8_t) (v >> 32);
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

        private:
            void largeV64(int64_t v) {
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

        public:

            inline void v64(int64_t v) {

                if (0L == (v & 0xFFFFFFFFFFFFFF80L)) {
                    *(position++) = (uint8_t) (v);
                } else {
                    *(position++) = (uint8_t) ((0x80L | v));
                    if (0L == (v & 0xFFFFFFFFFFFFC000L)) {
                        *(position++) = (uint8_t) ((v >> 7));
                    } else {
                        largeV64(v);
                    }
                }
            }


            inline void f32(float v) {
                register union {
                    uint32_t i;
                    float f;
                } tmp;
                tmp.f = v;
                i32(tmp.i);
            }

            inline void f64(double v) {
                register union {
                    uint64_t i;
                    double f;
                } tmp;
                tmp.f = v;
                i64(tmp.i);
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_MAPPEDOUTSTREAM_H
