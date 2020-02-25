#pragma once

#include "ir/Exports.h"

#include <mn/Buf.h>

namespace ir
{
	struct Type
	{
		enum KIND
		{
			KIND_VOID,
			KIND_INT8,
			KIND_INT16,
			KIND_INT32,
			KIND_INT64,
			KIND_UINT8,
			KIND_UINT16,
			KIND_UINT32,
			KIND_UINT64,
			KIND_FLOAT32,
			KIND_FLOAT64,
			KIND_PTR,
			KIND_ARRAY,
			KIND_PROC,
		};

		KIND kind;
		size_t size;
		size_t alignment;
		union
		{
			struct
			{
				Type* base;
			} ptr;

			struct
			{
				Type* base; size_t count;
			} array;

			struct
			{
				Type* ret;
				mn::Buf<Type*> args;
			} proc;
		};
	};

	extern IR_EXPORT Type* type_void;
	extern IR_EXPORT Type* type_int8;
	extern IR_EXPORT Type* type_int16;
	extern IR_EXPORT Type* type_int32;
	extern IR_EXPORT Type* type_int64;
	extern IR_EXPORT Type* type_uint8;
	extern IR_EXPORT Type* type_uint16;
	extern IR_EXPORT Type* type_uint32;
	extern IR_EXPORT Type* type_uint64;
	extern IR_EXPORT Type* type_float32;
	extern IR_EXPORT Type* type_float64;

	IR_EXPORT Type*
	type_ptr(Type* base);

	IR_EXPORT Type*
	type_array(Type* base, size_t count);

	IR_EXPORT Type*
	type_proc(Type* ret, mn::Buf<Type*> args);

	inline static Type*
	type_proc(Type* ret, const std::initializer_list<Type*>& args)
	{
		return type_proc(ret, mn::buf_lit(args));
	}

	IR_EXPORT void
	type_free(Type* self);

	inline static void
	destruct(Type* self)
	{
		type_free(self);
	}
}