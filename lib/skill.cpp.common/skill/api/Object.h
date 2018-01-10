//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_OBJECT_H
#define SKILL_CPP_COMMON_OBJECT_H

#include "String.h"

namespace skill {
    namespace internal {
        class FieldDeclaration;

        class DistributedField;

        class LazyField;

        class AbstractStoragePool;

        template<typename T>
        class BasePool;
    }
    namespace fieldTypes {
        class AnnotationType;
    }
    namespace api {

        /**
         * an abstract skill object, i.e. the type of an annotation and the base type of
         * every known and unknown skill type instance.
         */
        class Object {
            friend class internal::AbstractStoragePool;

            template<typename T>
            friend
            class internal::BasePool;

            template<typename T>
            friend
            struct std::hash;

            friend class internal::FieldDeclaration;

            friend class internal::DistributedField;

            friend class internal::LazyField;

            friend class fieldTypes::AnnotationType;

        protected:
            SKilLID id;

            //! bulk allocation
            Object() {}

            //! actual constructor
            Object(SKilLID id) : id(id) {}

        public:
            /**
             * return the skill name of the type of this object
             */
            virtual const char *skillName() const = 0;

            /**
             * query, whether the object is marked for deletion and will be destroyed on flush
             */
            bool isDeleted() const { return 0 == id; }

            //! TODO virtual get(f)
            //! TODO virtual set(f, v)

            /**
             * inserts a human readable presentation of the object into the argument ostream
             *
             * @note constant time operation, i.e. referenced objects are not pretty themselves
             */
            virtual void prettyString(std::ostream &os) const = 0;
        };
    }
}

inline std::ostream &operator<<(std::ostream &os, const skill::api::Object &obj) {
    obj.prettyString(os);
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const skill::api::Object *obj) {
    obj->prettyString(os);
    return os;
}

#endif //SKILL_CPP_COMMON_OBJECT_H
