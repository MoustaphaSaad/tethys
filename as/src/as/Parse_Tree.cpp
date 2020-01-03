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

	Decl*
	decl_c_proc_new(C_Proc c_proc)
	{
		auto self = mn::alloc_zerod<Decl>();
		self->kind = Decl::KIND_C_PROC;
		self->c_proc = c_proc;
		return self;
	}

	void
	decl_free(Decl* self)
	{
		switch(self->kind)
		{
		case Decl::KIND_PROC:
			proc_free(self->proc);
			break;
		case Decl::KIND_C_PROC:
			c_proc_free(self->c_proc);
			break;
		case Decl::KIND_CONSTANT:
		case Decl::KIND_NONE:
			// do nothing
			break;
		default:
			assert(false && "unreachable");
			break;
		}
		mn::free(self);
	}
}
