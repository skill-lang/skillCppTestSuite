//
// Created by feldentm on 03.11.15.
//

#ifndef SKILL_CPP_COMMON_INSTREAM_H
#define SKILL_CPP_COMMON_INSTREAM_H

#include <string>
#include "Stream.h"
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
        class InStream : public Stream {
            static inline void ensure(const bool condition) {
                if (!condition)
                    throw SkillException("unexpected end of stream");
            }

        protected:

            InStream(void *base, void *end) : Stream(base, end) { }

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
                register uint64_t r = 0;
                r |= ((uint64_t) *(position++)) << 56;
                r |= ((uint64_t) *(position++)) << 48;
                r |= ((uint64_t) *(position++)) << 40;
                r |= ((uint64_t) *(position++)) << 32;
                r |= ((uint64_t) *(position++)) << 24;
                r |= ((uint64_t) *(position++)) << 16;
                r |= ((uint64_t) *(position++)) << 8;
                r |= ((uint64_t) *(position++));
                return r;
            }

            static inline api::Box i64Box(InStream &self) {
                register api::Box r;
                r.i64 = self.i64();
                return r;
            }

            inline int64_t v64() {
                register uint64_t r, rval;

                if (0 != ((rval = *(position++)) & 0x80)) {
                    rval = (rval & 0x7f) | (((r = *(position++)) & 0x7f) << 7);

                    if (0 != (r & 0x80)) {
                        rval |= ((r = *(position++)) & 0x7f) << 14;

                        if (0 != (r & 0x80)) {
                            rval |= ((r = *(position++)) & 0x7f) << 21;

                            if (0 != (r & 0x80)) {
                                rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 28;

                                if (0 != (r & 0x80)) {
                                    rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 35;

                                    if (0 != (r & 0x80)) {
                                        rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 42;

                                        if (0 != (r & 0x80)) {
                                            rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 49;

                                            if (0 != (r & 0x80)) {
                                                rval |= (((uint64_t) *(position++)) << 56);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (position > end)
                    throw SkillException("unexpected end of stream");
                return rval;
            }

            //! checked version of v64 that will not throw an exception
            //! @note checked means checked by the caller!
            inline int64_t v64checked() noexcept {
                register uint64_t r, rval;

                if (0 != ((rval = *(position++)) & 0x80)) {
                    rval = (rval & 0x7f) | (((r = *(position++)) & 0x7f) << 7);

                    if (0 != (r & 0x80)) {
                        rval |= ((r = *(position++)) & 0x7f) << 14;

                        if (0 != (r & 0x80)) {
                            rval |= ((r = *(position++)) & 0x7f) << 21;

                            if (0 != (r & 0x80)) {
                                rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 28;

                                if (0 != (r & 0x80)) {
                                    rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 35;

                                    if (0 != (r & 0x80)) {
                                        rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 42;

                                        if (0 != (r & 0x80)) {
                                            rval |= ((uint64_t) (r = *(position++)) & 0x7f) << 49;

                                            if (0 != (r & 0x80)) {
                                                rval |= (((uint64_t) *(position++)) << 56);
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
        };
    }
}
#endif //SKILL_CPP_COMMON_INSTREAM_H
