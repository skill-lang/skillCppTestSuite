//
// Created by Timm Felden on 23.11.15.
//

#ifndef SKILL_CPP_COMMON_BUILTINFIELDTYPE_H
#define SKILL_CPP_COMMON_BUILTINFIELDTYPE_H

#include "FieldType.h"
#include "../utils.h"
#include "../api/types.h"

namespace skill {
    using streams::InStream;
    namespace fieldTypes {

        /**
         * generic implementation of builtin types, where <T> is the base type in C++
         */
        template<typename T, TypeID id>
        class BuiltinFieldType : public FieldType {

        protected:
            BuiltinFieldType() : FieldType(id) { }
        };

        template<typename T, TypeID id>
        class ConstantFieldType : public BuiltinFieldType<T, id> {
        public:
            virtual bool requiresDestruction() const {
                return true;
            }

        protected:
            ConstantFieldType(T value) : BuiltinFieldType<T, id>(), value(value) { }

        public:
            /**
             * value of the constant
             */
            const T value;

            virtual uint64_t offset(const api::Box &target) const {
                return 0;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                // nothing to do
            }
        };

        class ConstantI8 : public ConstantFieldType<int8_t, 0> {
        public:
            ConstantI8(int8_t value) : ConstantFieldType(value) { }

            virtual api::Box read(streams::MappedInStream &in) const {
                register api::Box r;
                r.i8 = value;
                return r;
            }
        };

        class ConstantI16 : public ConstantFieldType<int16_t, 1> {
        public:

            ConstantI16(int16_t value) : ConstantFieldType(value) { }

            virtual api::Box read(streams::MappedInStream &in) const {
                register api::Box r;
                r.i16 = value;
                return r;
            }
        };

        class ConstantI32 : public ConstantFieldType<int32_t, 2> {
        public:
            ConstantI32(int32_t value) : ConstantFieldType(value) { }

            virtual api::Box read(streams::MappedInStream &in) const {
                register api::Box r;
                r.i32 = value;
                return r;
            }
        };

        class ConstantI64 : public ConstantFieldType<int64_t, 3> {
        public:
            ConstantI64(int64_t value) : ConstantFieldType(value) { }

            virtual api::Box read(streams::MappedInStream &in) const {
                register api::Box r;
                r.i64 = value;
                return r;
            }
        };

        class ConstantV64 : public ConstantFieldType<int64_t, 4> {
        public:
            ConstantV64(int64_t value) : ConstantFieldType(value) { }

            virtual api::Box read(streams::MappedInStream &in) const {
                register api::Box r;
                r.i64 = value;
                return r;
            }
        };

        /**
         * all field types that have no state whatsoever
         */
        template<typename T, TypeID id,
                api::Box Read(InStream &)/*,
                uint64_t Offset(api::Box &target),
                void Write(streams::MappedOutStream *out, api::Box &target)*/>
        struct StatelessFieldType : BuiltinFieldType<T, id> {
            StatelessFieldType() : BuiltinFieldType<T, id>() { }

            virtual api::Box read(streams::MappedInStream &in) const {
                return Read(in);
            }

            virtual bool requiresDestruction() const {
                return false;
            }
        };

        /**
         * all types with a fixed size serialization
         */
        template<typename T, TypeID id,
                api::Box Read(InStream &),
                uint64_t size>
        struct FixedSizeType : StatelessFieldType<T, id, Read> {

            FixedSizeType() { }

            virtual api::Box read(streams::MappedInStream &in) const {
                return Read(in);
            }

            virtual uint64_t offset(const api::Box &target) const {
                return size;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }
        };

        struct V64FieldType : public StatelessFieldType<int64_t, 11, InStream::v64Box> {
            V64FieldType() : StatelessFieldType<int64_t, 11, InStream::v64Box>() { }

            static uint64_t offset(int v) {
                return (0L == (v & 0xFFFFFF80L)) ? 1 :
                       ((0L == (v & 0xFFFFC000L)) ? 2 :
                        ((0L == (v & 0xFFE00000L)) ? 3 :
                         ((0L == (v & 0xF0000000L)) ? 4 : 5)
                        )
                       );
            }

            static uint64_t offset(int64_t v) {
                return (0L == (v & 0xFFFFFFFFFFFFFF80L)) ? 1 :
                       ((0L == (v & 0xFFFFFFFFFFFFC000L)) ? 2 :
                        ((0L == (v & 0xFFFFFFFFFFE00000L)) ? 3 :
                         ((0L == (v & 0xFFFFFFFFF0000000L)) ? 4 :
                          ((0L == (v & 0xFFFFFFF800000000L)) ? 5 :
                           ((0L == (v & 0xFFFFFC0000000000L)) ? 6 :
                            ((0L == (v & 0xFFFE000000000000L)) ? 7 :
                             ((0L == (v & 0xFF00000000000000L)) ? 8 : 9)
                            )
                           )
                          )
                         )
                        )
                       );
            }

            virtual uint64_t offset(const api::Box &target) const {
                return offset(target.i64);
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }
        };

        const FixedSizeType<bool, 6, InStream::boolBox, 1> BoolType;
        const FixedSizeType<int8_t, 7, InStream::i8Box, 1> I8;
        const FixedSizeType<int16_t, 8, InStream::i16Box, 2> I16;
        const FixedSizeType<int32_t, 9, InStream::i32Box, 4> I32;
        const FixedSizeType<int64_t, 10, InStream::i64Box, 8> I64;
        const V64FieldType V64;
        const FixedSizeType<float, 12, InStream::f32Box, 4> F32;
        const FixedSizeType<double, 13, InStream::f64Box, 8> F64;

        template<typename T, TypeID id>
        struct SingleBaseTypeContainer : public BuiltinFieldType<T *, 15> {
            const FieldType *const base;
        private:
            const bool destroyBase;
        public:

            SingleBaseTypeContainer(const FieldType *const base)
                    : base(base),
                      destroyBase(base->requiresDestruction()) { }

            virtual ~SingleBaseTypeContainer() {
                if (destroyBase)
                    delete base;
            }

            virtual bool requiresDestruction() const {
                return true;
            }

        };

        struct ConstantLengthArray : public SingleBaseTypeContainer<api::Box *, 15> {
            const size_t length;

            ConstantLengthArray(int64_t length, const FieldType *const base)
                    : SingleBaseTypeContainer<api::Box *, 15>(base), length((size_t) length) { }

            template<typename T>
            api::Array<T> *read(streams::MappedInStream &in) const {
                api::Array<T> *r = new api::Array<T>(length);
                for (size_t i = 0; i < length; i++)
                    r->update(i, this->base->read(in));
                return r;
            }

            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                r.array = new api::Array<api::Box>(length);
                for (size_t i = 0; i < length; i++)
                    r.array->update(i, this->base->read(in));
                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                uint64_t rval = 0;
                for (size_t i = 0; i < length; i++)
                    rval += this->base->offset(target.array->get(i));
                return rval;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }
        };

        struct VariableLengthArray : public SingleBaseTypeContainer<std::vector<api::Box> *, 17> {

            VariableLengthArray(const FieldType *const base)
                    : SingleBaseTypeContainer(base) { }


            template<typename T>
            api::Array<T> *read(streams::MappedInStream &in) const {
                size_t length = (size_t) in.v64();
                api::Array<T> *r = new api::Array<T>(length);
                for (size_t i = 0; i < length; i++)
                    r->update(i, this->base->read(in));
                return r;
            }

            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                size_t length = (size_t) in.v64();
                r.list = new api::Array<api::Box>(length);
                for (size_t i = 0; i < length; i++)
                    r.list->update(i, this->base->read(in));
                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                uint64_t rval = V64FieldType::offset((int64_t) target.list->length());
                for (size_t i = 0; i < target.list->length(); i++)
                    rval += this->base->offset(target.list->get(i));
                return rval;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }
        };

        struct ListType : public SingleBaseTypeContainer<std::vector<api::Box> *, 18> {

            ListType(const FieldType *const base)
                    : SingleBaseTypeContainer(base) { }

            template<typename T>
            api::Array<T> *read(streams::MappedInStream &in) const {
                size_t length = (size_t) in.v64();
                api::Array<T> *r = new api::Array<T>(length);
                for (size_t i = 0; i < length; i++)
                    r->update(i, this->base->read(in));
                return r;
            }

            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                size_t length = (size_t) in.v64();
                r.list = new api::Array<api::Box>(length);
                for (size_t i = 0; i < length; i++)
                    r.list->update(i, this->base->read(in));
                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                uint64_t rval = V64FieldType::offset((int64_t) target.list->length());
                for (size_t i = 0; i < target.list->length(); i++)
                    rval += this->base->offset(target.list->get(i));
                return rval;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }
        };

        struct SetType : public SingleBaseTypeContainer<api::BoxedSet *, 19> {

            SetType(const FieldType *const base)
                    : SingleBaseTypeContainer(base) { }

            template<typename T>
            api::Set<T> *read(streams::MappedInStream &in) const {
                size_t length = (size_t) in.v64();
                api::Set<T> *r = new api::Set<T>();
                for (size_t i = 0; i < length; i++)
                    r->add(this->base->read(in));
                return r;
            }

            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                size_t length = (size_t) in.v64();
                r.set = new api::Set<api::Box>();
                for (size_t i = 0; i < length; i++)
                    r.set->add(this->base->read(in));
                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                uint64_t rval = V64FieldType::offset((int64_t) target.set->length());
                auto bs = target.set->all();
                while (bs->hasNext())
                    rval += this->base->offset(bs->next());
                return rval;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }
        };

        struct MapType : public BuiltinFieldType<api::BoxedMap *, 20> {
            const FieldType *const key;
            const FieldType *const value;
        private:
            const bool destroyKey;
            const bool destroyValue;
        public:

            MapType(const FieldType *const key, const FieldType *const value)
                    : key(key), value(value), destroyKey(key->requiresDestruction()),
                      destroyValue(value->requiresDestruction()) { }

            virtual ~MapType() {
                if (destroyKey)
                    delete key;
                if (destroyValue)
                    delete value;
            }

            virtual api::Box read(streams::MappedInStream &in) const {
                api::Box r;
                size_t length = (size_t) in.v64();
                r.map = new api::Map<api::Box, api::Box>;
                for (size_t i = 0; i < length; i++) {
                    const auto &k = key->read(in);
                    const auto &v = value->read(in);
                    r.map->update(k, v);
                }
                return r;
            }

            virtual uint64_t offset(const api::Box &target) const {
                uint64_t rval = V64FieldType::offset((int64_t) target.map->length());
                auto bs = target.map->all();
                while (bs->hasNext()) {
                    auto b = bs->next();
                    rval += key->offset(b.first);
                    rval += value->offset(b.second);
                }
                return rval;
            }

            virtual void write(streams::MappedOutStream *out, api::Box &target) const {
                SK_TODO;
            }

            virtual bool requiresDestruction() const {
                return true;
            }
        };

    }

}


#endif //SKILL_CPP_COMMON_BUILTINFIELDTYPE_H
