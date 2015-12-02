//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_OBJECT_H
#define SKILL_CPP_COMMON_OBJECT_H

#include "String.h"

namespace skill {
    namespace internal {
        class AbstractStoragePool;
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

            friend class fieldTypes::AnnotationType;

        protected:
            SKilLID id;

            //! bulk allocation
            Object() { }

            //! actual constructor
            Object(SKilLID id) : id(id) { }

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
            //! TODO ostream& operator<< (ostream& os, const string& str); + virtual prettyString
        };
    }
}


#endif //SKILL_CPP_COMMON_OBJECT_H
