//
// Created by Timm Felden on 04.11.15.
//

#include <assert.h>
#include "StringPool.h"

using namespace skill;
using api::String;

internal::StringPool::StringPool(streams::FileInputStream *in, AbstractStringKeeper *keeper)
        : in(in), knownStrings(), keeper(keeper), idMap(), stringPositions(), lastID(0) {
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

String internal::StringPool::add(const char *target, int length) {
    String result = new string_t(target, length, -1);
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

void internal::StringPool::prepareAndWrite(skill::streams::FileOutputStream *out) {
    prepareSerialization();

    // Insert new strings to the map;
    // this is where duplications with lazy strings will be detected and eliminated
    for (auto s : knownStrings) {
        if (-1 == s->id) {
            const_cast<api::string_t *>(s)->id = (SKilLID) idMap.size();
            idMap.push_back(s);
        }
    }

    const int64_t count = idMap.size() - 1;
    // write block, if nonempty
    if (count) {
        auto table = out->jumpAndMap(fieldTypes::V64FieldType::offset(count) + 4 * count);
        table->v64(count);
        int off = 0;
        for(int i = 1; i <= count; i++){
            const auto data = idMap[i];
            off += data->length();
            table->i32(off);
            out->put(data);
        }
        out->unmap(table);
    } else {
        out->i8(0);
    }
}

void internal::StringPool::prepareSerialization() {
    // ensure all strings are present
    for (auto i = stringPositions.size() - 1; i != 0; i--)
        get(i);
}
