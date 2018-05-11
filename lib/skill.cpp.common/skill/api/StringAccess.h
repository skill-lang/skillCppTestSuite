//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGACCESS_H
#define SKILL_CPP_COMMON_STRINGACCESS_H

#include "String.h"
#include "../fieldTypes/BuiltinFieldType.h"
#include "../streams/FileOutputStream.h"

namespace skill {
    namespace api {
        /**
         * This class is the public API for string manipulation in a SKilL file.
         *
         * @author Timm Felden
         */
        struct StringAccess : public fieldTypes::BuiltinFieldType<String, 14> {
            virtual String add(const char *target) = 0;
            virtual String add(const char *target, int length) = 0;

            virtual ~StringAccess() { };
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGACCESS_H
