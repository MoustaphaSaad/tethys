#pragma once

#include "as/Exports.h"
#include "as/Src.h"

namespace as
{
	AS_EXPORT bool
	parse(Src* src);

	AS_EXPORT mn::Str
	decl_dump(Src* src, mn::Allocator allocator = mn::allocator_top());
}