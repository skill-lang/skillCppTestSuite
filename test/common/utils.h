//
// Created by Timm Felden on 05.02.16.
//

#ifndef SKILL_CPP_TESTSUITE_UTILS_H
#define SKILL_CPP_TESTSUITE_UTILS_H

#include <skill/api/SkillFile.h>
#include <skill/api/Arrays.h>
#include <skill/api/Sets.h>
#include <skill/api/Maps.h>

namespace common {
    template<typename sf>
    std::unique_ptr<sf> tempFile(skill::api::WriteMode mode = ::skill::api::WriteMode::write) {
        return std::unique_ptr<sf>(sf::create(std::string(tmpnam(nullptr)), mode));
    }

    /**
     * create an array; used by test generation
     *
     * @note the likely memory leak is an acceptable issue
     */
    template<typename T>
    skill::api::Array<T> *array() {
        return new skill::api::Array<T>(0);
    }

    /**
     * create a set; used by test generation
     *
     * @note the likely memory leak is an acceptable issue
     */
    template<typename T>
    skill::api::Set<T> *set() {
        return new skill::api::Set<T>(0);
    }

    /**
     * create a map; used by test generation
     *
     * @note the likely memory leak is an acceptable issue
     */
    template<typename K, typename V>
    skill::api::Map<K,V> *map() {
        return new skill::api::Map<K,V>();
    }

    /**
     * put values into an array
     */
    template<typename T>
    skill::api::Array<T> *put(skill::api::Array<T> *arr, T v) {
        arr->push_back(v);
        return arr;
    }

    /**
     * put values into a set
     */
    template<typename T>
    skill::api::Set<T> *put(skill::api::Set<T> *arr, T v) {
        arr->insert(v);
        return arr;
    }

    /**
     * put values into a map
     */
    template<typename K, typename V>
    skill::api::Map<K,V> *put(skill::api::Map<K,V> *m, K k, V v) {
        (*m)[k] = v;
        return m;
    }
}

#endif //SKILL_CPP_TESTSUITE_UTILS_H
