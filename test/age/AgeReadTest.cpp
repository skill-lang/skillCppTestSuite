//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include "../../src/age/File.h"

using ::age::api::SkillFile;

TEST(AgeParser, ReadAge) {
    auto s = std::unique_ptr<SkillFile>(SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/ageUnrestricted.sf"));
    s->ages;
}
