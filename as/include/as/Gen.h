#pragma once

#include "as/Exports.h"

#include <vm/Pkg.h>

namespace as
{
	struct Src;

	AS_EXPORT vm::Pkg
	src_gen(Src* src);
}