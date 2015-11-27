//
// Created by Timm Felden on 04.11.15.
//

#include "StringPool.h"

using namespace skill;
using api::String;

internal::StringPool::StringPool(streams::FileInputStream *in)
        : in(in) {
    // ensure existence of fake entry
    stringPositions.push_back(std::pair<long, int>(-1L, -1));
    idMap.push_back(nullptr);
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

internal::StringPool::~StringPool() {
    for (auto s : knownStrings)
        delete s;
}
