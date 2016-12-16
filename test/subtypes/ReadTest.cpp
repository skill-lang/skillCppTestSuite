
#include <gtest/gtest.h>
#include "../../src/subtypes/File.h"

using ::subtypes::api::SkillFile;

struct Visitor : public subtypes::api::AVisitor {
    std::string s;

    virtual void visit(subtypes::A *node) {
        s += "a";
    }

    virtual void visit(subtypes::B *node) {
        s += "b";
    }

    virtual void visit(subtypes::C *node) {
        s += "c";
    }

    virtual void visit(subtypes::D *node) {
        s += "d";
    }
};

TEST(SubtypesRead, VisitorTest) {
    try {
        auto s = std::unique_ptr<SkillFile>(
                SkillFile::open("../../src/test/resources/genbinary/subtypes/accept/localBasePoolOffset.sf"));
        Visitor v;
        for (auto iter = s->A->all(); iter.hasNext();) {
            iter.next()->accept(&v);
        }
        GTEST_ASSERT_EQ(v.s, "aabbbcbbddacd");
    } catch (skill::SkillException e) {
        GTEST_FAIL() << "an exception was thrown:" << std::endl << e.message;
    }
    GTEST_SUCCEED();
}
