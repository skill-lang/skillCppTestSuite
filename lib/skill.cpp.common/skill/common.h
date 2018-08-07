//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_COMMON_H_H
#define SKILL_CPP_COMMON_COMMON_H_H

#include <cstdint>

namespace skill {
    /**
     * keep the skill id type, and thus the number of treatable skill objects, configurable
     */
    typedef int SKilLID;

    /**
     * keep the number of types configurable and independent of skill ids
     */
    typedef int TypeID;

    /**
     * zero cost signed to unsigned conversion
     */
    inline uint64_t unsign(int64_t v) {
        union {
            int64_t s;
            uint64_t u;
        } x;
        static_assert(sizeof(x) == sizeof(int64_t), "assumption on size failed");
        x.s = v;
        return x.u;
    }

    /**
     * zero cost signed to unsigned conversion
     */
    inline uint32_t unsign(int32_t v) {
        union {
            int32_t s;
            uint32_t u;
        } x;
        static_assert(sizeof(x) == sizeof(int32_t), "assumption on size failed");
        x.s = v;
        return x.u;
    }
}

#endif //SKILL_CPP_COMMON_COMMON_H_H
