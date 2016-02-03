//
// Created by Timm Felden on 03.02.16.
//

#ifndef SKILL_CPP_COMMON_OUTSTREAM_H
#define SKILL_CPP_COMMON_OUTSTREAM_H

#include <cstdint>
#include <cstddef>

namespace skill {
    namespace streams {

        /**
         * Abstract skill stream.
         *
         * @author Timm Felden
         */
        class Stream {

        protected:

            /**
             * base pointer of the stream.
             * We keep the base pointer, because it is required for unmap and sane error reporting.
             */
            void *const base;

            /**
             * position inside of the stream
             */
            uint8_t *position;
            /**
             * end pointer of the stream. The stream is done, if position reached end.
             */
            void *const end;

            Stream(void *base, void *end) : base(base), position((uint8_t*)base), end(end) { }

        public:

            /**
             * Proper destruction happens in child destructors
             */
            virtual ~Stream() { }

            /**
             * the position of this stream inside of its bounds
             */
            long getPosition() const noexcept {
                return (long) position - (long) base;
            }

            inline bool eof() const noexcept {
                return position >= end;
            }

            inline bool has(size_t amountLeft) const noexcept {
                return (position + amountLeft) < end;
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_OUTSTREAM_H
