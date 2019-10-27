#pragma once

#include "as/Exports.h"
#include "as/Parse_Tree.h"
#include "as/Src.h"

#include <vm/Pkg.h>

#include <mn/Buf.h>

namespace as
{
	AS_EXPORT mn::Buf<uint8_t>
	proc_gen(const Proc& proc, mn::Allocator allocator = mn::allocator_top());

	AS_EXPORT vm::Pkg
	src_gen(Src* src);
}