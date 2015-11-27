//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGACCESS_H
#define SKILL_CPP_COMMON_STRINGACCESS_H

#include "String.h"

namespace skill {
    namespace api {
        /**
         * This class is the public API for string manipulation in a SKilL file.
         *
         * @author Timm Felden
         */
        class StringAccess {
            virtual String add(const char *target) = 0;
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGACCESS_H
