//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include <skill/internal/UnknownObject.h>
#include "../../src/custom/File.h"
#include "../common/utils.h"

using ::custom::api::SkillFile;

TEST(CustomFieldTest, UseAny) {
    try {
        auto sf = common::tempFile<SkillFile>();
        uint8_t v = 17;
        {
            auto c = sf->Custom->add();
            // create a local pointer
            c->any = &v;
            sf->close();
        }

        // ensure that the pointer refers to the right value
        ASSERT_EQ(17, *(sf->Custom->get(1)->any));
        v = 23;
        ASSERT_EQ(23, *(sf->Custom->get(1)->any));
        ASSERT_EQ(&v, sf->Custom->get(1)->any);

    } catch (std::exception &e) {
        GTEST_FAIL() << "std::exception: " << e.what();
    } catch (skill::SkillException e) {
        GTEST_FAIL() << "unexpected failure: " << e.message;
    } catch (...) {
        GTEST_FAIL() << "unexpected exception";
    }
}