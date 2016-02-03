//
// Created by Timm Felden on 03.02.16.
//

#ifndef SKILL_CPP_COMMON_FILEWRITER_H
#define SKILL_CPP_COMMON_FILEWRITER_H

#include "../streams/FileOutputStream.h"
#include "AbstractStoragePool.h"

namespace skill {
    namespace api {
        class SkillFile;
    }
    namespace internal {
        class FileWriter {
            //!there is no instance
            FileWriter() { }

            static void writeType(const FieldType *const type, streams::FileOutputStream &out);

            static void restrictions(skill::internal::AbstractStoragePool *pool, streams::FileOutputStream &stream);

            static void restrictions(skill::internal::FieldDeclaration *f, streams::FileOutputStream &out);

            static void writeFieldData(SkillFile *state, streams::FileOutputStream &out, size_t offset,
                                       std::vector<FieldDeclaration *> &fields);

        public:

            static void write(api::SkillFile *state, const std::string &path);
        };
    }
}

#endif //SKILL_CPP_COMMON_FILEWRITER_H
