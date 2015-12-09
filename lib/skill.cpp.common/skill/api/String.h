//
// Created by Timm Felden on 06.11.15.
//

#ifndef SKILL_CPP_COMMON_STRING_H
#define SKILL_CPP_COMMON_STRING_H

#include <string>
#include <functional>
#include "../common.h"

namespace skill {
    namespace api {
        class string_t;

        /**
         * we will be using unique pointers to string objects as strings
         */
        typedef const string_t *String;
    }

    namespace internal {
        class StringPool;
    }
    namespace streams {
        class InStream;
    }
    namespace api {

        struct equalityHash;

        /**
         * Managed strings, as used throughout skill files.
         *
         * @note we need our own string implementation for proper resource management
         * @note you can obtain a string from a files string access
         * @note having unique Strings in a file is necessary for our boxing strategy
         * @note strings are immutable
         *
         * @author Timm Felden
         */
        class string_t : public std::string {
        private:
            SKilLID id;

            friend struct equalityHash;

            const size_t hash;

            /**
             * this is the source literal, iff the string was constructed from a literal.
             * otherwise it is the result of std::string::c_str()
             */
            const char *const source;

            inline size_t mkHash(const std::string &c) {
                std::hash<std::string> hash;
                return hash(c);
            }

            /**
             * only a string pool and streams can create new Strings
             */
            friend class internal::StringPool;

            friend class streams::InStream;

            /**
             * users shall not copy strings; managed strings should be unique,
             * so that we can identify them by their pointer and maps over String* behave as
             * intended.
             */
            string_t(const string_t &c)
                    : std::string(c), id(c.id), hash(mkHash(c)), source(std::string::c_str()) { }

            /**
             * internal construction for literal strings only;
             */
            string_t(const char *c)
                    : std::string(c), id(-1), hash(mkHash(*this)), source(c) { }

            /**
             * internal construction only;
             */
            string_t(const char *c, SKilLID id)
                    : std::string(c), id(id), hash(mkHash(*this)), source(std::string::c_str()) { }

            string_t(const char *c, int length, SKilLID id)
                    : std::string(c, length), id(id), hash(mkHash(*this)), source(std::string::c_str()) { }

        public:
            SKilLID getID() const {
                return id;
            }

            /**
             * override c_str by something that is constant, if the string was constructed from a literal
             */
            const char* c_str() const {
                return source;
            }
        };

        using skill::api::String;

        /**
         * image equality based comparison of strings
         */
        struct equalityLess {
            bool operator()(const String &lhs, const String &rhs) const {
                return *lhs < *rhs;
            }
        };

        /**
         * image equality based hashcode of strings
         */
        struct equalityHash {
            size_t operator()(const String &s) const noexcept {
                return s->hash;
            }
        };

        /**
         * image equality based comparison of strings
         */
        struct equalityEquals {
            bool operator()(const String &x, const String &y) const {
                std::equal_to<std::string> eq;
                return eq(*x, *y);
            }
        };
    }
}


#endif //SKILL_CPP_COMMON_STRING_H
