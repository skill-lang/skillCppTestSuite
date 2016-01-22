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

        inline Box box(const bool x) {
            Box r;
            r.boolean = x;
            return r;
        }

        inline Box box(const int8_t x) {
            Box r;
            r.i8 = x;
            return r;
        }

        inline Box box(const int16_t x) {
            Box r;
            r.i16 = x;
            return r;
        }

        inline Box box(const int32_t x) {
            Box r;
            r.i32 = x;
            return r;
        }

        inline Box box(const int64_t x) {
            Box r;
            r.i64 = x;
            return r;
        }

        inline Box box(const float x) {
            Box r;
            r.f32 = x;
            return r;
        }

        inline Box box(const double x) {
            Box r;
            r.f64 = x;
            return r;
        }

        inline Box box(const String x) {
            Box r;
            r.string = x;
            return r;
        }

        inline Box box(Box *const x) {
            Box r;
            r.array = x;
            return r;
        }

        inline Box box(std::vector<Box> *x) {
            Box r;
            r.list = x;
            return r;
        }

        inline Box box(std::set<Box> *x) {
            Box r;
            r.set = x;
            return r;
        }

        inline Box box(std::map<Box, Box> *x) {
            Box r;
            r.map = x;
            return r;
        }

        inline Box box(Object *x) {
            Box r;
            r.annotation = x;
            return r;
        }

        inline bool operator<(const Box l, const Box r) {
            return l.i64 < r.i64;
        }

        static_assert(sizeof(Box) == sizeof(int64_t),
                      "I exepct Box and void* to have the same size and a similar behaviour");
    }
}

#endif //SKILL_CPP_COMMON_BOX_H
