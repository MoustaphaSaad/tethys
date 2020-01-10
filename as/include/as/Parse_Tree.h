#pragma once

#include "as/Exports.h"
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


	struct C_Proc
	{
		Tkn name;
		mn::Buf<Tkn> args;
		Tkn ret;
	};

	inline static C_Proc
	c_proc_new()
	{
		C_Proc self{};
		self.args = mn::buf_new<Tkn>();
		return self;
	}

	inline static void
	c_proc_free(C_Proc& self)
	{
		mn::buf_free(self.args);
	}

	inline static void
	destruct(C_Proc& self)
	{
		c_proc_free(self);
	}


	struct Decl
	{
		enum KIND
		{
			KIND_NONE,
			KIND_PROC,
			KIND_CONSTANT,
			KIND_C_PROC,
		};

		KIND kind;
		union
		{
			Proc proc;
			Constant constant;
			C_Proc c_proc;
		};
	};

	AS_EXPORT Decl*
	decl_proc_new(Proc proc);

	AS_EXPORT Decl*
	decl_constant_new(Constant constant);

	AS_EXPORT Decl*
	decl_c_proc_new(C_Proc c_proc);

	AS_EXPORT void
	decl_free(Decl* self);

	inline static void
	destruct(Decl* self)
	{
		decl_free(self);
	}
}