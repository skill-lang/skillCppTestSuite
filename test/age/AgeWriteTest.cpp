//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include <skill/internal/UnknownObject.h>
#include "../../src/age/File.h"
#include "../common/utils.h"

using ::age::api::SkillFile;

TEST(AgeWriteTest, WriteAge) {
    try {
        auto sf = common::tempFile<SkillFile>();
        sf->Age->add(1);
        sf->Age->add(28);
        sf->close();

        ASSERT_EQ(2, sf->Age->size());
        ASSERT_EQ(1, sf->Age->get(1)->getAge());
        ASSERT_EQ(28, sf->Age->get(2)->getAge());

        auto sf2 = SkillFile::open(sf->currentPath());
        ASSERT_EQ(2, sf2->Age->size());
        ASSERT_EQ(1, sf2->Age->get(1)->getAge());
        ASSERT_EQ(28, sf2->Age->get(2)->getAge());
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}