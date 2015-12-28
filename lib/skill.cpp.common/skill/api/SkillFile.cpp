//
// Created by Timm Felden on 04.11.15.
//

#include "SkillFile.h"
#include "../internal/StringPool.h"
#include "../fieldTypes/AnnotationType.h"

using namespace skill;
using namespace api;
using namespace internal;

SkillFile::SkillFile(streams::FileInputStream *in, WriteMode mode, StringPool *stringPool,
                     fieldTypes::AnnotationType *annotation,
                     std::vector<std::unique_ptr<AbstractStoragePool>> *types, typeByName_t *typesByName)
        : strings(stringPool), annotation(annotation), types(new internal::AbstractStoragePool *[types->size()]),
          typesByName(typesByName), fromFile(in) {
    for (size_t i = 0; i < types->size(); i++)
        const_cast<AbstractStoragePool **>(this->types)[i] = types->at(i).release();
}

SkillFile::~SkillFile() {
    for (size_t i = 0; i < size(); i++)
        delete types[i];
    delete[] types;
    delete typesByName;
    delete strings;
    delete annotation;
    delete fromFile;
}
