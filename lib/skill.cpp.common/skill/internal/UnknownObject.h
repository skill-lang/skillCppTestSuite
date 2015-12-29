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
            const AbstractStoragePool *owner;

            //! bulk allocation constructor
            UnknownObject() { };

            friend class Book<UnknownObject>;

        public:
            /**
             * !internal use only!
             */
            inline void byPassConstruction(SKilLID id, AbstractStoragePool *owner) {
                this->id = id;
                this->owner = owner;
            }

            UnknownObject(SKilLID id) : Object(id), owner(nullptr) {
                throw SkillException("one cannot create an unknown object without supllying a name");
            }

            UnknownObject(SKilLID id, const AbstractStoragePool *owner)
                    : Object(id), owner(owner) { }

            virtual const char *skillName() const;

            virtual void prettyString(std::ostream &os) const {
                os << skillName() << "#" << id << "(unknown)";
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_UNKNOWNOBJECT_H
