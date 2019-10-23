#pragma once

#include "as/Tkn.h"

#include <mn/Buf.h>

namespace as
{
	struct Ins
	{
		Tkn op;
		Tkn dst;
		Tkn src;
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
}