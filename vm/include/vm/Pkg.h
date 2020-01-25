#pragma once

#include "vm/Exports.h"
#include "vm/C.h"

#include <mn/Str.h>
#include <mn/Buf.h>
#include <mn/Map.h>
#include <mn/Result.h>

namespace vm
{
	struct Section
	{
		enum KIND: uint8_t
		{
			KIND_CONSTANT,
			KIND_BYTECODE,
		};

		KIND kind;
		mn::Str name;
		mn::Block bytes;
	};

	VM_EXPORT Section
	section_constant_new(const mn::Str& name, mn::Block bytes);

	VM_EXPORT Section
	section_bytecode_new(const mn::Str& name, mn::Block bytes);

	VM_EXPORT void
	section_free(Section& self);

	inline static void
	destruct(Section& self)
	{
		section_free(self);
	}

	VM_EXPORT void
	section_save(const Section &self, mn::Stream stream);

	VM_EXPORT Section
	section_load(mn::Stream stream);


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

	VM_EXPORT void
	reloc_save(const Reloc& self, mn::Stream stream);

	VM_EXPORT Reloc
	reloc_load(mn::Stream stream);


	struct Pkg
	{
		mn::Map<mn::Str, Section> sections;
		mn::Buf<Reloc> relocs;
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

	VM_EXPORT void
	pkg_proc_add(Pkg& self, const mn::Str& name, mn::Block bytes);

	inline static void
	pkg_proc_add(Pkg& self, const char* name, mn::Block bytes)
	{
		pkg_proc_add(self, mn::str_lit(name), bytes);
	}

	VM_EXPORT void
	pkg_constant_add(Pkg& self, const mn::Str &constant_name, mn::Block bytes);

	inline static void
	pkg_constant_add(Pkg& self, const char* name, mn::Block bytes)
	{
		pkg_constant_add(self, mn::str_lit(name), bytes);
	}

	VM_EXPORT void
	pkg_reloc_add(Pkg& self, const mn::Str &source_name, uint64_t source_offset, const mn::Str &target_name);

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