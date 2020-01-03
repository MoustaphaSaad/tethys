#pragma once

#include "vm/Exports.h"
#include "vm/C.h"

#include <mn/Str.h>
#include <mn/Buf.h>
#include <mn/Map.h>
#include <mn/Result.h>

namespace vm
{
	// relocations is used to fix proc address on loading in call instructions
	struct Reloc
	{
		mn::Str source_name;
		mn::Str target_name;
		uint64_t source_offset;
	};

	VM_EXPORT Reloc
	reloc_new();

	VM_EXPORT void
	reloc_free(Reloc& self);

	inline static void
	destruct(Reloc& self)
	{
		reloc_free(self);
	}

	struct Pkg
	{
		mn::Map<mn::Str, mn::Buf<uint8_t>> constants;
		mn::Map<mn::Str, mn::Buf<uint8_t>> procs;
		mn::Buf<Reloc> relocs;
		mn::Buf<Reloc> constant_relocs;
		mn::Buf<C_Proc> c_procs;
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
	pkg_reloc_add(Pkg& self, mn::Str source_name, uint64_t source_offset, mn::Str target_name);

	VM_EXPORT bool
	pkg_constant_add(Pkg& self, mn::Str constant_name, mn::Block bytes);

	VM_EXPORT void
	pkg_constant_reloc_add(Pkg& self, mn::Str source_name, uint64_t source_offset, mn::Str target_name);

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

	// this will load the package bytecode into a cpu core
	struct Core;

	VM_EXPORT mn::Err
	pkg_core_load(const Pkg& self, Core& core, uint64_t stack_size_in_bytes = 8ULL * 1024ULL * 1024ULL);
}