//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_FIELDRESTRICTION_H
#define SKILL_CPP_COMMON_FIELDRESTRICTION_H

#include <assert.h>
#include "../api/Object.h"
#include "../api/Box.h"

namespace skill {
    namespace restrictions {
        struct FieldRestriction {
            virtual ~FieldRestriction();
        };

        using api::Box;

        struct CheckableRestriction : public FieldRestriction {

            /**
             * checks argument v.
             * @return true, iff argument fulfills the restriction
             */
            virtual bool check(Box v) const = 0;
        };

        struct NonNull : public CheckableRestriction {
            static const NonNull *get();

            virtual bool check(Box v) const {
                return nullptr != v.annotation;
            }

        private:
            const static NonNull instance;

            NonNull() { }
        };

        template<typename T>
        struct FieldDefault : public FieldRestriction {
            const T value;

            FieldDefault(T v) : value(v) { };
        };

        template<typename T>
        struct Range : public CheckableRestriction {
            const T min;
            const T max;

            /**
             * construct from inclusive ranges
             */
            Range(T min, T max) : min(min), max(max) { assert(min <= max); };

            virtual bool check(Box v) const {
                const T x = api::unbox<T>(v);
                return min <= x && x <= max;
            }
        };

        struct Coding : public FieldRestriction {
            const api::String coding;

            Coding(api::String coding) : coding(coding) { }
        };

        struct ConstantLengthPointer : public FieldRestriction {
            static const ConstantLengthPointer *get();

        private:
            const static ConstantLengthPointer instance;

            ConstantLengthPointer() { }
        };

    }
}


#endif //SKILL_CPP_COMMON_FIELDRESTRICTION_H
