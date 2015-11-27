//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_SKILLSTATE_H
#define SKILL_CPP_COMMON_SKILLSTATE_H

#include "../api/SkillFile.h"

namespace skill {
    namespace internal {

        class SkillState : public api::SkillFile {
        public:
            //! TODO make private and remove test code!
            SkillState(streams::FileInputStream *in, api::WriteMode mode,
                       StringPool *stringPool, int *annotation,
                       std::vector<AbstractStoragePool *> *types,
                       api::typeByName_t *typesByName)
                    : SkillFile(in, mode, stringPool, types, typesByName) {
                //! TODO no solution;)
                delete annotation;
            }

            virtual ~SkillState();

        };
    }
}


#endif //SKILL_CPP_COMMON_SKILLSTATE_H
