#pragma once

#include "vm/Exports.h"

#include <mn/Str.h>
#include <mn/Buf.h>

namespace vm
{
	enum C_TYPE: int32_t
	{
		C_TYPE_VOID,
		C_TYPE_INT8,
		C_TYPE_INT16,
		C_TYPE_INT32,
		C_TYPE_INT64,
		C_TYPE_UINT8,
		C_TYPE_UINT16,
		C_TYPE_UINT32,
		C_TYPE_UINT64,
		C_TYPE_FLOAT32,
		C_TYPE_FLOAT64,
		C_TYPE_PTR,
	};

	struct C_Proc
	{
		mn::Str lib;
		mn::Str name;
		mn::Buf<C_TYPE> arg_types;
		C_TYPE ret;
	};

	VM_EXPORT C_Proc
	c_proc_new();

	VM_EXPORT void
	c_proc_free(C_Proc& self);

	inline static void
	destruct(C_Proc& self)
	{
		c_proc_free(self);
	}

	VM_EXPORT C_Proc
	c_proc_clone(const C_Proc& self, mn::Allocator allocator = mn::allocator_top());

	inline static C_Proc
	clone(const C_Proc& self)
	{
		return c_proc_clone(self);
	}
}