#pragma once

#include "vm/Exports.h"

#include <mn/Str.h>
#include <mn/Buf.h>
#include <mn/Map.h>

namespace vm
{
	struct Pkg
	{
		mn::Map<mn::Str, mn::Buf<uint8_t>> procs;
	};

	VM_EXPORT Pkg
	pkg_new();

	VM_EXPORT void
	pkg_free(Pkg& self);

	inline static void
	destruct(Pkg& self)
	{
		pkg_free(self);
	}

	VM_EXPORT bool
	pkg_proc_add(Pkg& self, const mn::Str& name, const mn::Buf<uint8_t>& bytes);

	inline static bool
	pkg_proc_add(Pkg& self, const char* name, const mn::Buf<uint8_t>& bytes)
	{
		return pkg_proc_add(self, mn::str_from_c(name), bytes);
	}

	VM_EXPORT void
	pkg_save(const Pkg& self, const mn::Str& filename);

	inline static void
	pkg_save(const Pkg& self, const char* filename)
	{
		pkg_save(self, mn::str_lit(filename));
	}

	VM_EXPORT Pkg
	pkg_load(const mn::Str& filename);

	inline static Pkg
	pkg_load(const char* filename)
	{
		return pkg_load(mn::str_lit(filename));
	}

	// prepares the bytecode for vm execution
	VM_EXPORT mn::Buf<uint8_t>
	pkg_load_proc(const Pkg& self, const mn::Str& name);

	inline static mn::Buf<uint8_t>
	pkg_load_proc(const Pkg& self, const char* name)
	{
		return pkg_load_proc(self, mn::str_lit(name));
	}
}