//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_SKILLSTATE_H
#define SKILL_CPP_COMMON_SKILLSTATE_H

#include "../api/SkillFile.h"
#include "../fieldTypes/AnnotationType.h"

namespace skill {
    namespace internal {

        class SkillState : public api::SkillFile {
        protected:
            fieldTypes::AnnotationType *const annotation;
        public:
            //! TODO make private and remove test code!
            SkillState(streams::FileInputStream *in, api::WriteMode mode,
                       StringPool *stringPool, fieldTypes::AnnotationType *annotation,
                       std::vector<AbstractStoragePool *> *types,
                       api::typeByName_t *typesByName)
                    : SkillFile(in, mode, stringPool, types, typesByName),
                      annotation(annotation) {
            }

            virtual ~SkillState();

        };
    }
}


#endif //SKILL_CPP_COMMON_SKILLSTATE_H
