//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include "../../src/age/File.h"

using ::age::api::SkillFile;

TEST(AgeParser, ReadAge) {
    auto sf = std::unique_ptr<SkillFile>(SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/ageUnrestricted.sf"));
    ASSERT_EQ(2, sf->Age->size());
    for(auto i = sf->Age->size(); i > 0; i--){
        sf->Age->get(i);
    }
    ASSERT_EQ(nullptr, sf->Age->get(0));
    ASSERT_EQ(1, sf->Age->get(1)->getAge());
    ASSERT_EQ(23, sf->Age->get(2)->getAge());
}
