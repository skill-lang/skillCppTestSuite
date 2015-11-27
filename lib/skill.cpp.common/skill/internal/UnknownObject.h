//
// Created by Timm Felden on 24.11.15.
//

#ifndef SKILL_CPP_COMMON_UNKNOWNOBJECT_H
#define SKILL_CPP_COMMON_UNKNOWNOBJECT_H

#include "../api/Object.h"
#include "../api/SkillException.h"

namespace skill {
    using api::String;
    namespace internal {
        template<class T>
        class Book;

        class UnknownObject : public api::Object {
            const String _skillName;

            //! bulk allocation constructor
            UnknownObject() : _skillName(nullptr) { };
            friend class Book<UnknownObject>;

        public:
            UnknownObject(SKilLID id) : Object(id), _skillName(nullptr) {
                throw SkillException("one cannot create an unknown object without supllying a name");
            }

            UnknownObject(SKilLID id, String const name)
                    : Object(id), _skillName(name) { }

            virtual String skillName() const {
                return _skillName;
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_UNKNOWNOBJECT_H
