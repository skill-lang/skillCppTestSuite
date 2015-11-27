//
// Created by feldentm on 03.11.15.
//

#ifndef SKILL_CPP_COMMON_INSTREAM_H
#define SKILL_CPP_COMMON_INSTREAM_H

#include <string>
#include <cstdint>
#include "../api/String.h"
#include "../api/Box.h"
#include "../api/SkillException.h"

namespace skill {
    namespace streams {

        using skill::api::String;

        /**
        * Abstract input streams.
        *
        * @author Timm Felden
        */
        class InStream {
            static inline void ensure(const bool condition) {
                if (!condition)
                    throw SkillException("unexpected end of stream");
            }

        protected:

            /**
             * base pointer of the stream.
             * We keep the base pointer, because it is required for unmap and sane error reporting.
             */
            const void *base;

            /**
             * position inside of the stream
             */
            uint8_t *position;
            /**
             * end pointer of the stream. The stream is done, if position reached end.
             */
            const void *end;

        public:

            /**
             * Proper destruction happens in child destructors
             */
            virtual ~InStream() { }

            inline int8_t i8() {
                ensure(position < end);
                return *(position++);
            }

            static inline api::Box i8Box(InStream &self) {
                register api::Box r;
                r.i8 = self.i8();
                return r;
            }

            inline int16_t i16() {
                ensure(position + 1 < end);
                register uint16_t r = *(position++) << 8;
                r |= *(position++);
                return r;
            }

            static inline api::Box i16Box(InStream &self) {
                register api::Box r;
                r.i16 = self.i16();
                return r;
            }

            inline int32_t i32() {
                ensure(position + 3 < end);
                register uint32_t r = *(position++) << 24;
                r |= *(position++) << 16;
                r |= *(position++) << 8;
                r |= *(position++);
                return r;
            }

            static inline api::Box i32Box(InStream &self) {
                register api::Box r;
                r.i32 = self.i32();
                return r;
            }

            inline int64_t i64() {
                ensure(position + 7 < end);
                register uint64_t r = ((uint64_t) *(position++)) << 56;
                r |= ((uint64_t) *(position++)) << 48;
                r |= ((uint64_t) *(position++)) << 40;
                r |= ((uint64_t) *(position++)) << 32;
                r |= *(position++) << 24;
                r |= *(position++) << 16;
                r |= *(position++) << 8;
                r |= *(position++);
                return r;
            }

            static inline api::Box i64Box(InStream &self) {
                register api::Box r;
                r.i64 = self.i64();
                return r;
            }

            inline int64_t v64() {
                register uint64_t r, rval;

                if (0 != ((rval = i8()) & 0x80)) {
                    rval = (rval & 0x7f) | (((r = i8()) & 0x7f) << 7);

                    if (0 != (r & 0x80)) {
                        rval |= ((r = i8()) & 0x7f) << 14;

                        if (0 != (r & 0x80)) {
                            rval |= ((r = i8()) & 0x7f) << 21;

                            if (0 != (r & 0x80)) {
                                rval |= ((uint64_t) (r = i8()) & 0x7f) << 28;

                                if (0 != (r & 0x80)) {
                                    rval |= ((uint64_t) (r = i8()) & 0x7f) << 35;

                                    if (0 != (r & 0x80)) {
                                        rval |= ((uint64_t) (r = i8()) & 0x7f) << 42;

                                        if (0 != (r & 0x80)) {
                                            rval |= ((uint64_t) (r = i8()) & 0x7f) << 49;

                                            if (0 != (r & 0x80)) {
                                                rval |= (((uint64_t) i8()) << 56);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return rval;
            }

            static inline api::Box v64Box(InStream &self) {
                register api::Box r;
                r.i64 = self.v64();
                return r;
            }

            inline float f32() {
                register union {
                    uint32_t i;
                    float f;
                } result;
                result.i = i32();
                return result.f;
            }

            static inline api::Box f32Box(InStream &self) {
                register api::Box r;
                r.f32 = self.f32();
                return r;
            }

            inline double f64() {
                register union {
                    uint64_t i;
                    double f;
                } result;
                result.i = i64();
                return result.f;
            }

            static inline api::Box f64Box(InStream &self) {
                register api::Box r;
                r.f64 = self.f64();
                return r;
            }


            inline bool boolean() {
                return *(position++) != 0;
            }

            static inline api::Box boolBox(InStream &self) {
                register api::Box r;
                r.boolean = self.boolean();
                return r;
            }

            /**
             * create a string from the stream
             * @note the caller owns the string!
             */
            String string(int length, SKilLID id) {
                ensure(position + length <= end);
                ensure(id > 0);
                String rval = new api::string_t((const char *) position, length, id);
                position = position + length;
                return rval;
            }

            /**
             * the position of this stream inside of its bounds
             */
            long getPosition() const {
                return (long) position - (long) base;
            }

            inline bool eof() const {
                return position >= end;
            }
        };
    }
}
#endif //SKILL_CPP_COMMON_INSTREAM_H
