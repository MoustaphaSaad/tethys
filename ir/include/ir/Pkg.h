#pragma once

#include "ir/Exports.h"
#include "ir/Proc.h"

#include <mn/Str.h>

namespace ir
{
	using Proc_Bag = Bag<Proc>;
	using Proc_ID  = typename Bag<Proc>::Handle;

	struct Pkg
	{
		mn::Str name;
		Proc_Bag procs;
	};

	Pkg
	pkg_new(const mn::Str& name)
	{
		Pkg self{};
		self.name = mn::str_clone(name);
		self.procs = bag_new<Proc>();
		return self;
	}

	inline static Pkg
	pkg_new(const char* name)
	{
		return pkg_new(mn::str_lit(name));
	}

	void
	pkg_free(Pkg& self)
	{
		mn::str_free(self.name);
		bag_free(self.procs);
	}

	inline static void
	destruct(Pkg& self)
	{
		pkg_free(self);
	}
}