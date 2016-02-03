//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGPOOL_H
#define SKILL_CPP_COMMON_STRINGPOOL_H

#include <set>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <future>

#include "../api/StringAccess.h"
#include "../streams/FileInputStream.h"

namespace skill {
    using namespace api;
    namespace internal {

        /**
         * String keepers keep references to known strings as fields, so that they can be accessed in
         * constant time with a guarantee of uniqueness.
         */
        struct AbstractStringKeeper {

        };

        /**
         * Implementation of all string handling.
         *
         * @author Timm Felden
         */
        class StringPool : public StringAccess {
            mutable std::mutex lock;

            streams::FileInputStream *in;

            /**
   * the set of known strings, including new strings
   *
   * @note having all strings inside of the set instead of just the new ones, we can optimize away some duplicates,
   * while not having any additional cost, because the duplicates would have to be checked upon serialization anyway.
   * Furthermore, we can unify type and field names, thus we do not have to have duplicate names laying around,
   * improving the performance of hash containers and name checks:)
   */
            mutable std::unordered_set<String, equalityHash, equalityEquals> knownStrings;
        public:
            const AbstractStringKeeper *const keeper;

        private:

            /**
             * get string by ID
             */
            mutable std::vector<String> idMap;

            /**
             * ID ⇀ (absolute offset, length)
             *
             * will be used if idMap contains a null reference
             *
             * @note there is a fake entry at ID 0
             */
            std::vector<std::pair<long, int>> stringPositions;

            /**
             * next legal ID, used to check access
             */
            SKilLID lastID;

        public:
            StringPool(streams::FileInputStream *in, AbstractStringKeeper *keeper);

            virtual ~StringPool();

            /**
             * add a string to the pool
             */
            virtual String add(const char *target);

            /**
             * add a literal string to the pool. This has the fancy guarantee, that
             * result->c_str() == target.
             *
             * @note this wont work, if the string is already known!
             */
            virtual String addLiteral(const char *target);

            inline void addPosition(std::pair<long, int> position) {
                idMap.push_back(nullptr);
                stringPositions.push_back(position);
                lastID++;
            }

            /**
             * search a string by id it had inside of the read file, may block if the string has not yet been read
             */
            String get(SKilLID index) const {
                if (index <= 0) return nullptr;
                else if (index > lastID) throw SkillException("index of StringPool::get too large");
                else {
                    std::unique_lock<std::mutex>(lock);
                    String result = idMap[index];
                    if (nullptr == result) {
                        // read result
                        auto off = stringPositions[index];
                        long mark = in->getPosition();
                        in->jump(off.first);
                        result = in->string(off.second, index);
                        in->jump(mark);

                        // unify result with known strings
                        auto it = knownStrings.find(result);
                        if (it == knownStrings.end())
                            // a new string
                            knownStrings.insert(result);
                        else {
                            // a string that exists already;
                            // the string cannot be from the file, so set the id
                            delete result;
                            result = *it;
                            const_cast<string_t *>(result)->id = index;
                        }

                        idMap[index] = result;
                    }
                    return result;
                }
            }

            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                r.string = get((SKilLID) in.v64());
                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                return fieldTypes::V64FieldType::offset(target.string->id);
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                out->v64(target.string->id);
            }

            inline void write(streams::MappedOutStream *out, const api::String target) const {
                out->v64(target->id);
            }

            /**
             * destroyed by the skill file
             */
            virtual bool requiresDestruction() const {
                return false;
            }

            /**
             * prepare the pool and write it to tho stream
             */
            void prepareAndWrite(skill::streams::FileOutputStream *out);

        private:
            void prepareSerialization();
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGPOOL_H
