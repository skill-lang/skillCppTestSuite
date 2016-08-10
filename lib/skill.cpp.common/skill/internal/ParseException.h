//
// Created by Timm Felden on 27.11.15.
//

#ifndef SKILL_CPP_COMMON_PARSEEXCEPTION_H
#define SKILL_CPP_COMMON_PARSEEXCEPTION_H

#include "../api/SkillException.h"
#include "../streams/FileInputStream.h"

namespace skill {
    namespace internal {

        /**
         * creates a parse exception
         */
        inline SkillException ParseException(std::unique_ptr<streams::FileInputStream>& stream, int blockCount,
                                             std::string msg) {
            std::stringstream message;
            message << "ParseException in file" << stream->getPath()
            << "\n Position " << stream->getPosition()
            << "\n Block    " << blockCount
            << "\n reason: " << msg << std::endl;
            return SkillException(message.str());
        }

        inline SkillException ParseException(InStream* stream, int blockCount,
                                             std::string msg) {
            std::stringstream message;
            message << "ParseException in mapped stream.\n Position" << stream->getPosition()
            << "\n Block" << blockCount << "\n reason: "
            << msg << std::endl;
            return SkillException(message.str());
        }
    }
}

#endif //SKILL_CPP_COMMON_PARSEEXCEPTION_H
