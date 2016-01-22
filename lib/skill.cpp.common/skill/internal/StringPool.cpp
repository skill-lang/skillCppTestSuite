//
// Created by Timm Felden on 04.11.15.
//

#include <assert.h>
#include "StringPool.h"

using namespace skill;
using api::String;

internal::StringPool::StringPool(streams::FileInputStream *in, AbstractStringKeeper *keeper)
        : lock(), in(in), knownStrings(), keeper(keeper), idMap(), stringPositions(), lastID(0) {
    // ensure existence of fake entry
    stringPositions.push_back(std::pair<long, int>(-1L, -1));
    idMap.push_back(nullptr);
}

internal::StringPool::~StringPool() {
    for (auto s : knownStrings)
        delete s;
    delete keeper;
}

String internal::StringPool::add(const char *target) {
    String result = new string_t(target, -1);
    auto it = knownStrings.find(result);
    if (it != knownStrings.end()) {
        delete result;
        return *it;
    } else {
        knownStrings.insert(result);
        return result;
    }
}

String internal::StringPool::addLiteral(const char *target) {
    String result = new string_t(target);
    assert(knownStrings.find(result) == knownStrings.end());

    knownStrings.insert(result);
    return result;
}
