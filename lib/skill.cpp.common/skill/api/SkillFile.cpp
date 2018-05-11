//
// Created by Timm Felden on 04.11.15.
//

#include "SkillFile.h"
#include "../fieldTypes/AnnotationType.h"
#include "../internal/FileWriter.h"
#include "../streams/FileOutputStream.h"

#include <atomic>
#include <iostream>

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
    // TODO lacks type and unknown restrictions

    // collected checked fields
    std::vector<FieldDeclaration *> fields;
    for (size_t i = 0; i < typesByName->size(); i++) {
        const auto p = types[i];
        for (size_t j = 0; j < p->dataFields.size(); j++) {
            const auto f = p->dataFields[j];
            if (f->hasRestrictions())
                fields.push_back(f);
        }
    }

    std::atomic<bool> failed;
    failed = false;

    // @note this should be more like, each pool is checking its type restriction, aggregating its field restrictions,
    // and if there are any, then they will all be checked using (hopefully) overridden check methods
#pragma omp parallel for
    for (size_t i = 0; i < fields.size(); i++) {
        const auto f = fields[i];
        if (!f->check()) {
            std::cerr << "Restriction check failed for " << *(f->owner->name) << "." << *(f->name) << std::endl;
            failed = true;
        }
    }

    if (failed)
        throw SkillException("check failed");
}

void SkillFile::changePath(std::string path) {
    if (mode == write) {
        currentWritePath = path;
    } else
        throw std::invalid_argument("target path can only be changed in write mode");
}

const std::string &SkillFile::currentPath() const {
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