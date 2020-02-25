#pragma once

#include "ir/Exports.h"
#include "ir/Type.h"
#include "ir/Value.h"

#include <mn/Str.h>
#include <mn/Buf.h>
#include <mn/Pool.h>

namespace ir
{
	struct Ins
	{
		enum OP
		{
			OP_IGL,
			OP_ADD,
			OP_RET,
		};

		OP op;
		union
		{
			struct
			{
				Value a, b;
			} op_add;

			Value op_ret;
		};
	};

	inline static Ins
	ins_add(Value a, Value b)
	{
		Ins self{};
		self.op = Ins::OP_ADD;
		self.op_add.a = a;
		self.op_add.b = b;
		return self;
	}

	inline static Ins
	ins_ret(Value a)
	{
		Ins self{};
		self.op = Ins::OP_RET;
		self.op_ret = a;
		return self;
	}


	struct Basic_Block
	{
		Proc* proc;
		mn::Buf<Ins*> code;
	};

	struct Proc
	{
		mn::Str name;
		Type* type;
		mn::Pool ins_pool;
		mn::Pool block_pool;
		mn::Buf<Basic_Block*> blocks;
	};

	struct Pkg
	{
		mn::Str name;
		mn::Pool proc_pool;
		mn::Buf<Proc*> procs;
	};

	IR_EXPORT Pkg
	pkg_new(const mn::Str& name);

	inline static Pkg
	pkg_new(const char* name)
	{
		return pkg_new(mn::str_lit(name));
	}

	IR_EXPORT void
	pkg_free(Pkg& self);

	inline static void
	destruct(Pkg& self)
	{
		pkg_free(self);
	}

	IR_EXPORT Proc*
	pkg_proc_new(Pkg& self, const mn::Str& name, Type* type);

	inline static Proc*
	pkg_proc_new(Pkg& self, const char* name, Type* type)
	{
		return pkg_proc_new(self, mn::str_lit(name), type);
	}

	IR_EXPORT void
	pkg_proc_free(Pkg& self, Proc* proc);

	IR_EXPORT Value
	proc_arg(Proc* self, size_t ix);

	IR_EXPORT Basic_Block*
	proc_basic_block_new(Proc* proc);

	IR_EXPORT void
	proc_basic_block_free(Proc* self, Basic_Block* bb);

	IR_EXPORT Value
	basic_block_ins_add(Basic_Block* bb, Value a, Value b);

	IR_EXPORT Value
	basic_block_ins_ret(Basic_Block* bb, Value v);
}