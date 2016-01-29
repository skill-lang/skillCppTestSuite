//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_BOX_H
#define SKILL_CPP_COMMON_BOX_H

#include <cstdint>
#include <type_traits>
#include "String.h"
#include "Object.h"

namespace skill {
    namespace api {
        class BoxedArray;

        class BoxedSet;

        class BoxedMap;

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
            BoxedArray *array;
            //! T[] & list<T>*
            BoxedArray *list;
            BoxedSet *set;
            BoxedMap *map;
            Object *annotation;
        };

        // required by reflection
        inline Box box(const Box x) {
            return x;
        }

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

        inline Box box(BoxedArray *const x) {
            Box r;
            r.array = x;
            return r;
        }

        inline Box box(BoxedSet *x) {
            Box r;
            r.set = x;
            return r;
        }

        inline Box box(BoxedMap *x) {
            Box r;
            r.map = x;
            return r;
        }

        inline Box box(Object *x) {
            Box r;
            r.annotation = x;
            return r;
        }

        template<typename T>
        inline T unbox(const Box &b) {

            //! project T onto a boxed map type, if more concrete
            typedef typename std::conditional<std::is_assignable<BoxedMap *, T>::value,
                    BoxedMap *,
                    T>::type flat;

            return unbox<flat>(b);
        }

        template<>
        inline Box unbox(const Box &b) {
            return b;
        }

        template<>
        inline bool unbox(const Box &b) {
            return b.boolean;
        }

        template<>
        inline int8_t unbox(const Box &b) {
            return b.i8;
        }

        template<>
        inline int16_t unbox(const Box &b) {
            return b.i16;
        }

        template<>
        inline int32_t unbox(const Box &b) {
            return b.i32;
        }

        template<>
        inline int64_t unbox(const Box &b) {
            return b.i64;
        }

        template<>
        inline float unbox(const Box &b) {
            return b.f32;
        }

        template<>
        inline double unbox(const Box &b) {
            return b.f64;
        }

        template<>
        inline BoxedArray *unbox(const Box &b) {
            return b.array;
        }

        template<>
        inline BoxedSet *unbox(const Box &b) {
            return b.set;
        }

        template<>
        inline BoxedMap *unbox(const Box &b) {
            return b.map;
        }

        inline bool operator<(const Box l, const Box r) {
            return l.i64 < r.i64;
        }

        static_assert(sizeof(Box) == sizeof(int64_t),
                      "I exepct Box and void* to have the same size and a similar behaviour");
    }
}

#endif //SKILL_CPP_COMMON_BOX_H
