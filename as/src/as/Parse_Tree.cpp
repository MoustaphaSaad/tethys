#include "as/Parse_Tree.h"

#include <mn/Memory.h>

namespace as
{
	// API
	Decl*
	decl_proc_new(Proc proc)
	{
		auto self = mn::alloc_zerod<Decl>();
		self->kind = Decl::KIND_PROC;
		self->proc = proc;
		return self;
	}

	Decl*
	decl_constant_new(Constant constant)
	{
		auto self = mn::alloc_zerod<Decl>();
		self->kind = Decl::KIND_CONSTANT;
		self->constant = constant;
		return self;
	}

	void
	decl_free(Decl* self)
	{
		if (self->kind == Decl::KIND_PROC)
			proc_free(self->proc);
		mn::free(self);
	}
}
