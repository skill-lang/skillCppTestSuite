//
// Created by Timm Felden on 27.01.16.
//

#ifndef SKILL_CPP_COMMON_API_MAP_H
#define SKILL_CPP_COMMON_API_MAP_H

#include <memory>
#include <unordered_map>
#include "Box.h"

namespace skill {
    namespace api {

        struct MapIterator {
            virtual bool hasNext() const = 0;

            virtual std::pair<Box, Box> next() = 0;
        };

        /**
         * A map that statically has no information about its content.
         *
         * @author Timm Felden
         * @note if you know the type runtime type, it is safe to cast down to Map<K,V>
         * @note representation is always a std::map
         */
        struct BoxedMap {

            operator Box() {
                Box r;
                r.map = this;
                return r;
            }

            virtual ~BoxedMap() { }

            /**
             * k € this
             */
            virtual bool contains(const Box &k) const = 0;

            /**
             * @return *this[k]
             */
            virtual Box get(const Box &k) const = 0;

            /**
             * add or update k->v mapping
             */
            virtual void update(const Box &k, const Box &v) = 0;

            /**
             * remove a key from the map
             */
            virtual void remove(const Box &k) = 0;

            virtual size_t length() const = 0;

            virtual std::unique_ptr<MapIterator> all() = 0;
        };

        /**
         * Actual representation of skill sets.
         */
        template<typename K, typename V>
        class Map : public std::unordered_map<K, V>, public BoxedMap {

            typedef typename std::unordered_map<K, V>::const_iterator iter;

            class Iterator : public MapIterator {
                iter state;
                const iter last;

                static inline std::pair<Box, Box> boxx(const std::pair<K, V> &p) {
                    return std::pair<Box, Box>(box(p.first), box(p.second));
                };

            public:

                Iterator(const Map *self) : state(self->begin()), last(self->end()) { }

                virtual bool hasNext() const {
                    return state != last;
                }

                virtual std::pair<Box, Box> next() {
                    return boxx(*state++);
                }
            };


        public:

            operator Box() {
                Box r;
                r.map = this;
                return r;
            }

            Map() : std::unordered_map<K, V>() { }

            virtual ~Map() { }

            virtual bool contains(const Box &k) const {
                return this->find(unbox<K>(k)) != this->end();
            }

            virtual Box get(const Box &k) const {
                return box(this->at(unbox<K>(k)));
            }

            virtual void update(const Box &k, const Box &v) {
                (*this)[unbox<K>(k)] = unbox<V>(v);
            }

            virtual void remove(const Box &k) {
                this->erase(unbox<K>(k));
            }

            virtual size_t length() const {
                return this->size();
            }

            virtual std::unique_ptr<MapIterator> all() {
                return std::unique_ptr<MapIterator>(new Iterator(this));
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_API_MAP_H
