//
// Created by Timm Felden on 04.11.15.
//

#include "SkillFile.h"
#include "../fieldTypes/AnnotationType.h"
#include "../internal/FileWriter.h"
#include "../streams/FileOutputStream.h"

using namespace skill;
using namespace api;
using namespace internal;

SkillFile::SkillFile(streams::FileInputStream *in, WriteMode mode, StringPool *stringPool,
                     fieldTypes::AnnotationType *annotation,
                     std::vector<AbstractStoragePool *> *types, typeByName_t *typesByName)
        : strings(stringPool), annotation(annotation), types(new internal::AbstractStoragePool *[types->size()]),
          typesByName(typesByName), fromFile(in), currentWritePath(fromFile->getPath()), mode(mode) {
    for (size_t i = 0; i < types->size(); i++) {
        auto t = types->at(i);
        const_cast<AbstractStoragePool **>(this->types)[i] = t;
    }
}

SkillFile::~SkillFile() {
    delete[] types;
    delete typesByName;
    delete strings;
    delete annotation;
    delete fromFile;
}

void SkillFile::check() {

    // TODO par
    // TODO a more efficient solution would be helpful
    // TODO lacks type restrictions
    // @note this should be more like, each pool is checking its type restriction, aggergating its field restrictions,
    // and if there are any, then they will all be checked using (hopefully) overridden check methods
    for (const auto &p : *this)
        for (const auto &f : p->dataFields)
            if (!f->check()) {
                std::stringstream message;
                message << "check failed in " << *p->name << "." << *f->name << std::endl;
                throw SkillException(message.str());
            }
}

void SkillFile::changePath(std::string path) {
    if(mode == write){
        currentWritePath = path;
    } else
        throw std::invalid_argument("target path can only be changed in write mode");
}

const std::string &SkillFile::currentPath() {
    return currentWritePath;
}


void SkillFile::changeMode(WriteMode writeMode) {
    if (mode == writeMode)
        return;
    if (readOnly == mode || append == writeMode)
        throw std::invalid_argument("this file is in write mode");

    mode = writeMode;
}


void SkillFile::flush() {
    switch (mode) {
        case write: {
            FileWriter::write(this, currentPath());
            break;
        }
        case append:
            break;
        case readOnly:
            throw std::invalid_argument("this file is in write mode");
    }
}

void SkillFile::close() {
    flush();
    changeMode(readOnly);
}