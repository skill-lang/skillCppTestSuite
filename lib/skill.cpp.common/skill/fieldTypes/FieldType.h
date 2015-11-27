//
// Created by Timm Felden on 20.11.15.
//

#ifndef SKILL_CPP_COMMON_FIELDTYPE_H
#define SKILL_CPP_COMMON_FIELDTYPE_H

#include "../common.h"
#include "../streams/MappedInStream.h"
#include "../api/Box.h"

typedef int outstream;

namespace skill {
    /**
     * this namespace contains field type information used by skill
     */
    namespace fieldTypes {

        /**
         * Field types as used in reflective access.
         *  @author Timm Felden
         *  @note field types have a mental type T that represents the base type of the field
         */
        class FieldType {

        protected:
            FieldType(const TypeID typeID) : typeID(typeID) { }

        public:
            //! the id of this type
            const TypeID typeID;

            /**
             * read a box from a stream using the correct read implementation
             */
            virtual api::Box read(streams::MappedInStream &in) const = 0;

            /**
             * calculate the offset of a single box
             */
            virtual uint64_t offset(api::Box &target) const = 0;

            /**
             * write a box to a stream
             */
            virtual void write(outstream &out, api::Box &target) const = 0;
        };
    }
}


#endif //SKILL_CPP_COMMON_FIELDTYPE_H
