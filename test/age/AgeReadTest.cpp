//
// Created by Timm Felden on 07.12.15.
//


#include <gtest/gtest.h>
#include <skill/internal/UnknownObject.h>
#include "../../src/age/File.h"
#include "../../src/age/StringKeeper.h"

using ::age::api::SkillFile;

TEST(AgeReadTest, ReadAge) {
    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/ageUnrestricted.sf"));
    ASSERT_EQ(2, sf->Age->size());
    for (auto i = sf->Age->size(); i > 0; i--) {
        sf->Age->get(i);
    }
    ASSERT_EQ(nullptr, sf->Age->get(0));
    ASSERT_EQ(1, sf->Age->get(1)->getAge());
    ASSERT_EQ(23, sf->Age->get(2)->getAge());
}

TEST(AgeReadTest, ReadAgeCheckTypes) {
    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/ageUnrestricted.sf"));
    ASSERT_EQ(1, sf->size());
    for (auto t : *sf) {
        ASSERT_EQ(((age::StringKeeper *) ((skill::internal::StringPool *) sf->strings)->keeper)->age,
                  t->name);
    }
}

TEST(AgeReadTest, ReadAgeCheckInstanceOrder) {
    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/ageUnrestricted.sf"));
    ASSERT_EQ(1, sf->size());
    auto as = sf->Age->allInTypeOrder();
    for (size_t i = 1; i <= sf->Age->size(); i++) {
        ASSERT_EQ(as.next()->skillID(), i);
    }
}

TEST(AgeReadTest, ReadLBPOCheckInstanceAllocation) {
    typedef ::skill::internal::StoragePool<skill::internal::UnknownObject, skill::internal::UnknownObject> *uPool;

    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/localBasePoolOffset.sf"));
    ASSERT_EQ(5, sf->size());
    bool foundA = false;
    const char *types = "aaabbbbbdddcc";
    for (auto t : *sf) {
        if (*t->name == std::string("a")) {
            foundA = true;
            uPool pool = (uPool) t;
            auto as = pool->allInTypeOrder();
            while (as.hasNext()) {
                ASSERT_EQ(*types, as.next()->skillName()[0]);
                types++;
            }
        }
    }
    ASSERT_TRUE(foundA); // saw an a
    ASSERT_EQ(0, *types); // at end of string
}

TEST(AgeReadTest, ReadDate) {
    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/date.sf"));
    ASSERT_NE(nullptr, sf->Age);
    ASSERT_EQ(0, sf->Age->size());
    for (auto i = sf->Age->size(); i > 0; i--) {
        sf->Age->get(i);
    }
    ASSERT_EQ(nullptr, sf->Age->get(0));
}
