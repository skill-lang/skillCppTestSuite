//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_MAPPEDINSTREAM_H
#define SKILL_CPP_COMMON_MAPPEDINSTREAM_H


#include "InStream.h"

namespace skill {
    namespace streams {
        class FileInputStream;

        /**
         * Name chosen for historical reasons. In fact, all streams share the same map.
         */
        class MappedInStream : public InStream {
        private:
            friend class FileInputStream;

            //! only file input streams can create mapped streams
            MappedInStream(const void *base, uint8_t *position, const void *end);

        public:

            //! requires no action; all resources are managed elsewhere
            ~MappedInStream(){}
        };
    }
}


#endif //SKILL_CPP_COMMON_MAPPEDINSTREAM_H