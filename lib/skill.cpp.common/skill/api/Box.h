//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_BOX_H
#define SKILL_CPP_COMMON_BOX_H


#include <cstdint>
#include <vector>
#include <set>
#include <map>
#include "String.h"
#include "Object.h"

namespace skill {
    namespace api {
        /**
         * This union represents the *unknown* object.
         * It is required for skill reflection.
         */
        union Box {
            bool boolean;
            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;
            float f32;
            double f64;
            String string;
            //! T[i]
            Box *array;
            //! T[] & list<T>*
            std::vector<Box> *list;
            std::set<Box> *set;
            std::map<Box, Box> *map;
            Object *annotation;
        };

        inline bool operator<(const Box l, const Box r) {
            return l.i64 < r.i64;
        }

        static_assert(sizeof(Box) == sizeof(int64_t),
                      "I exepct Box and void* to have the same size and a similar behaviour");
    }
}

#endif //SKILL_CPP_COMMON_BOX_H
