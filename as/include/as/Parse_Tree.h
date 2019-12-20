#pragma once

#include "as/Tkn.h"

#include <mn/Buf.h>

namespace as
{
	struct Ins
	{
		Tkn op;  // operation
		Tkn dst; // destination
		Tkn src; // source
		Tkn lbl; // label
	};

	struct Proc
	{
		Tkn name;
		mn::Buf<Ins> ins;
	};

	inline static Proc
	proc_new()
	{
		Proc self{};
		self.ins = mn::buf_new<Ins>();
		return self;
	}

	inline static void
	proc_free(Proc& self)
	{
		mn::buf_free(self.ins);
	}

	inline static void
	destruct(Proc& self)
	{
		proc_free(self);
	}

	struct Constant
	{
		Tkn name;
		Tkn value;
	};

	struct Decl
	{
		enum KIND
		{
			KIND_NONE,
			KIND_PROC,
			KIND_CONSTANT
		};

		KIND kind;
		union
		{
			Proc proc;
			Constant constant;
		};
	};

	Decl*
	decl_proc_new(Proc proc);

	Decl*
	decl_constant_new(Constant constant);

	void
	decl_free(Decl* self);

	inline static void
	destruct(Decl* self)
	{
		decl_free(self);
	}
}