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
    ASSERT_EQ(28, sf->Age->get(2)->getAge());
}

TEST(AgeReadTest, ReadAgeForachAPI) {
    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/ageUnrestricted.sf"));
    ASSERT_EQ(2, sf->Age->size());
    const char *as = "\x01\x1c";
    for (auto &age : *sf->Age) {
        ASSERT_EQ(*as++, age.getAge()) << "found wrong age";
    }
    ASSERT_EQ(0, *as) << "less or more as then expected";
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
    for (skill::SKilLID i = 1; i <= sf->Age->size(); i++) {
        ASSERT_EQ(as.next()->skillID(), i);
    }
}

TEST(AgeReadTest, CheckLBPOTypeHierarchyOrder) {

    auto checkType = [](skill::internal::AbstractStoragePool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            skill::iterators::TypeHierarchyIterator ts(t);
            for (auto &p : ts)
                ASSERT_EQ(*types++, p.name->at(0))
                                            << name << " contained wrong instances";
            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/localBasePoolOffset.sf"));
    ASSERT_EQ(5, sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "abdc");
        checkType(t, "b", "bd");
        checkType(t, "c", "c");
        checkType(t, "d", "d");
    }
}

TEST(AgeReadTest, ReadLBPOCheckTypeOrder) {
    typedef ::skill::internal::StoragePool<skill::internal::UnknownObject, skill::internal::UnknownObject> *uPool;

    auto checkType = [](skill::internal::AbstractStoragePool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            uPool pool = (uPool) t;
            const auto& is = pool->allInTypeOrder();
            for (auto &i : is)
                ASSERT_EQ(*types++, i.skillName()[0])
                                            << name << " contained wrong instances";

            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/localBasePoolOffset.sf"));
    ASSERT_EQ(5, sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "aaabbbbbdddcc");
        checkType(t, "b", "bbbbbddd");
        checkType(t, "c", "cc");
        checkType(t, "d", "ddd");
    }
}

TEST(AgeReadTest, ReadLBPOCheckStaticInstances) {
    typedef ::skill::internal::StoragePool<skill::internal::UnknownObject, skill::internal::UnknownObject> *uPool;

    auto checkType = [](skill::internal::AbstractStoragePool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            uPool pool = (uPool) t;
            for (auto &i : pool->staticInstances())
                ASSERT_EQ(*types++, ((age::Age &) i).skillID())
                                            << name << " contained wrong instances";

            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/localBasePoolOffset.sf"));
    ASSERT_EQ(5, sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "\x01\x02\x0B");
        checkType(t, "b", "\x03\x04\x05\x07\x08");
        checkType(t, "c", "\x06\x0D");
        checkType(t, "d", "\x09\x0A\x0C");
    }
}

TEST(AgeReadTest, ReadLBPOCheckAllocationOrder) {
    typedef ::skill::internal::StoragePool<skill::internal::UnknownObject, skill::internal::UnknownObject> *uPool;

    auto checkType = [](skill::internal::AbstractStoragePool *t, const char *name,
                        const char *types) -> void {
        if (*t->name == std::string(name)) {
            uPool pool = (uPool) t;
            for (auto &i : pool->all())
                ASSERT_EQ(*types++, i.skillName()[0])
                                            << name << " contained wrong instances";


            ASSERT_EQ(0, *types) << name << " is not at end of string";
        }
    };

    auto sf = std::unique_ptr<SkillFile>(
            SkillFile::open("../../src/test/resources/genbinary/[[empty]]/accept/localBasePoolOffset.sf"));
    ASSERT_EQ(5, sf->size());
    for (auto t : *sf) {
        checkType(t, "a", "aabbbcbbddadc");
        checkType(t, "b", "bbbbbddd");
        checkType(t, "c", "cc");
        checkType(t, "d", "ddd");
    }
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
