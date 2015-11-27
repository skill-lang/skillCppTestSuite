//
// Created by Timm Felden on 04.11.15.
//

#ifndef SKILL_CPP_COMMON_STRINGPOOL_H
#define SKILL_CPP_COMMON_STRINGPOOL_H

#include <set>
#include <vector>
#include <unordered_set>
#include <mutex>

#include "../api/StringAccess.h"
#include "../streams/FileInputStream.h"

namespace skill {
    using namespace api;
    namespace internal {

        /**
         * Implementation of all string handling.
         *
         * @author Timm Felden
         */
        class StringPool : public StringAccess {
            std::mutex lock;

            streams::FileInputStream *in;

            // note: this is public, because most internal stuff uses this
        public:
            StringPool(streams::FileInputStream *in);

            virtual ~StringPool();

            /**
   * the set of known strings, including new strings
   *
   * @note having all strings inside of the set instead of just the new ones, we can optimize away some duplicates,
   * while not having any additional cost, because the duplicates would have to be checked upon serialization anyway.
   * Furthermore, we can unify type and field names, thus we do not have to have duplicate names laying around,
   * improving the performance of hash containers and name checks:)
   */
            std::unordered_set<String, equalityHash, equalityEquals> knownStrings;

            /**
             * ID ⇀ (absolute offset, length)
             *
             * will be used if idMap contains a null reference
             *
             * @note there is a fake entry at ID 0
             */
            std::vector<std::pair<long, int>> stringPositions;


            /**
             * get string by ID
             */
            std::vector<String> idMap;

            virtual String add(const char *target);

            /**
             * search a string by id it had inside of the read file, may block if the string has not yet been read
             */
            String get(SKilLID index) {
                if (index <= 0) return nullptr;
                else {
                    auto result = idMap[index];
                    if (nullptr == result) {
                        // this kind of synchronization is not correct in general and I should know better not to do,
                        // stuff like that (but I did it anyway:) )
                        lock.lock();
                        {
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
                        lock.unlock();
                    }
                    return result;
                }
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_STRINGPOOL_H