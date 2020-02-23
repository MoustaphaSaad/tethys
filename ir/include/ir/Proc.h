#pragma once

#include "ir/Exports.h"
#include "ir/Bag.h"
#include "ir/Type.h"
#include "ir/Basic_Block.h"
#include "ir/Ins.h"
#include "ir/Value.h"
#include "ir/Handles.h"

#include <mn/Str.h>
#include <mn/Buf.h>

namespace ir
{
	struct Proc
	{
		Proc_ID id;
		mn::Str name;
		Type* type;
		Basic_Block_Bag blocks;
		Ins_Bag ins;
	};

	IR_EXPORT Proc
	proc_new(const mn::Str& name, Type *type);

	inline static Proc
	proc_new(const char* name, Type* type)
	{
		return proc_new(mn::str_lit(name), type);
	}

	IR_EXPORT void
	proc_free(Proc& self);

	inline static void
	destruct(Proc& self)
	{
		proc_free(self);
	}

	IR_EXPORT Value
	proc_arg(const Proc& self, size_t ix);
}