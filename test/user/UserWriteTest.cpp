//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include <skill/internal/UnknownObject.h>
#include "../../src/user/File.h"
#include "../common/utils.h"

using ::user::api::SkillFile;

/**
 * This test checks that users can have names, which use \0 inside of strings.
 */
TEST(UserWriteTest, NullStrings) {
    try {
        auto sf = common::tempFile<SkillFile>();
        char name_img[5] = {0, 1, 0, 0, 1};
        auto name = sf->strings->add(name_img, 5);
        sf->User->add(5, name);
        sf->close();

        ASSERT_EQ(1, sf->User->size());
        ASSERT_EQ(5, sf->User->get(1)->getAge());
        ASSERT_EQ(0, sf->User->get(1)->getName()->compare(0, 5, name_img, 5));

        auto sf2 = SkillFile::open(sf->currentPath());
        ASSERT_EQ(1, sf2->User->size());
        ASSERT_EQ(5, sf2->User->get(1)->getAge());
        ASSERT_EQ(0, sf2->User->get(1)->getName()->compare(0, 5, name_img, 5));
        delete sf2;
    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (skill::SkillException e) {
        GTEST_FAIL() << "unexpected failure: " << e.message;
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}