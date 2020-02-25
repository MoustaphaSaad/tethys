#include "ir/Type.h"

#include <mn/Memory.h>

#include <assert.h>

namespace ir
{
	template<typename T>
	inline static Type
	_generate_basic_type(Type::KIND k)
	{
		Type self{};
		self.kind = k;
		if constexpr (std::is_same_v<T, void> == false)
		{
			self.size = sizeof(T);
			self.alignment = alignof(T);
		}
		return self;
	}

	#define BASIC_TYPE(kind, type, name) \
	static Type _basic_type_##name = _generate_basic_type<type>(kind); \
	Type* type_##name = &_basic_type_##name

	BASIC_TYPE(Type::KIND_VOID, void, void);
	BASIC_TYPE(Type::KIND_INT8, int8_t, int8);
	BASIC_TYPE(Type::KIND_INT16, int16_t, int16);
	BASIC_TYPE(Type::KIND_INT32, int32_t, int32);
	BASIC_TYPE(Type::KIND_INT64, int64_t, int64);
	BASIC_TYPE(Type::KIND_UINT8, uint8_t, uint8);
	BASIC_TYPE(Type::KIND_UINT16, uint16_t, uint16);
	BASIC_TYPE(Type::KIND_UINT32, uint32_t, uint32);
	BASIC_TYPE(Type::KIND_UINT64, uint64_t, uint64);
	BASIC_TYPE(Type::KIND_FLOAT32, float, float32);
	BASIC_TYPE(Type::KIND_FLOAT64, double, float64);

	#undef BASIC_TYPE

	// API
	Type*
	type_ptr(Type* base)
	{
		auto self = mn::alloc<Type>();
		self->kind = Type::KIND_PTR;
		self->size = sizeof(void*);
		self->alignment = alignof(void*);
		self->ptr.base = base;
		return self;
	}

	Type*
	type_array(Type* base, size_t count)
	{
		auto self = mn::alloc<Type>();
		self->kind = Type::KIND_ARRAY;
		self->size = base->size * count;
		self->alignment = base->alignment;
		self->array.base = base;
		self->array.count = count;
		return self;
	}

	Type*
	type_proc(Type* ret, mn::Buf<Type*> args)
	{
		auto self = mn::alloc<Type>();
		self->kind = Type::KIND_PROC;
		self->size = sizeof(void*);
		self->alignment = alignof(void*);
		self->proc.ret = ret;
		self->proc.args = args;
		return self;
	}

	void
	type_free(Type* self)
	{
		switch(self->kind)
		{
		case Type::KIND_VOID:
		case Type::KIND_INT8:
		case Type::KIND_INT16:
		case Type::KIND_INT32:
		case Type::KIND_INT64:
		case Type::KIND_UINT8:
		case Type::KIND_UINT16:
		case Type::KIND_UINT32:
		case Type::KIND_UINT64:
		case Type::KIND_FLOAT32:
		case Type::KIND_FLOAT64:
			//do nothing
			break;
		case Type::KIND_PTR:
			type_free(self->ptr.base);
			mn::free(self);
			break;
		case Type::KIND_ARRAY:
			type_free(self->array.base);
			mn::free(self);
			break;
		case Type::KIND_PROC:
			type_free(self->proc.ret);
			destruct(self->proc.args);
			mn::free(self);
			break;
		default:
			assert(false && "unreachable");
			break;
		}
	}
}