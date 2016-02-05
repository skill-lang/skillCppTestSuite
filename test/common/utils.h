//
// Created by Timm Felden on 05.02.16.
//

#ifndef SKILL_CPP_TESTSUITE_UTILS_H
#define SKILL_CPP_TESTSUITE_UTILS_H

namespace common {
    template<typename sf>
    std::unique_ptr<sf> tempFile(skill::api::WriteMode mode = ::skill::api::WriteMode::write) {
        return std::unique_ptr<sf>(sf::create(std::string(tmpnam(nullptr)), mode));
    }
}

#endif //SKILL_CPP_TESTSUITE_UTILS_H
