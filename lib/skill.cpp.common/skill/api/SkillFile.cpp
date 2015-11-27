//
// Created by Timm Felden on 04.11.15.
//

#include "SkillFile.h"
#include "../internal/StringPool.h"

using namespace skill;
using namespace api;
using namespace internal;

SkillFile::SkillFile(
        streams::FileInputStream *in, WriteMode mode, StringPool *stringPool,
        std::vector<AbstractStoragePool *> *types, typeByName_t *typesByName)
        : strings(stringPool), types(types), typesByName(typesByName), fromFile(in) {

}

SkillFile::~SkillFile() {
    for (auto t : *types)
        delete t;
    delete types;
    delete typesByName;
    delete strings;
    delete fromFile;
}
