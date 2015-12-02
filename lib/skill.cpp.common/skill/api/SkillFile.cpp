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
                     std::vector<std::unique_ptr<AbstractStoragePool>> *types, typeByName_t *typesByName,
                     fieldTypes::AnnotationType *const annotation)
        : strings(stringPool), annotation(annotation), types(types),
          typesByName(typesByName), fromFile(in) {

}

SkillFile::~SkillFile() {
    delete types;
    delete typesByName;
    delete strings;
    delete annotation;
    delete fromFile;
}
