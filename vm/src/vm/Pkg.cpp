#include "vm/Pkg.h"
#include "vm/Core.h"

#include <mn/File.h>
#include <mn/Path.h>
#include <mn/Defer.h>
#include <mn/Library.h>

namespace vm
{
	inline static void
	_write64(uint8_t* ptr, uint64_t v)
	{
		ptr[0] = uint8_t(v);
		ptr[1] = uint8_t(v >> 8);
		ptr[2] = uint8_t(v >> 16);
		ptr[3] = uint8_t(v >> 24);
		ptr[4] = uint8_t(v >> 32);
		ptr[5] = uint8_t(v >> 40);
		ptr[6] = uint8_t(v >> 48);
		ptr[7] = uint8_t(v >> 56);
	}

	inline static void
	_write_string(mn::Stream out, const mn::Str& str)
	{
		uint32_t len = uint32_t(str.count);
		mn::stream_write(out, mn::block_from(len));
		mn::stream_write(out, mn::block_from(str));
	}

	inline static void
	_write_bytes(mn::Stream out, mn::Block bytes)
	{
		uint32_t len = uint32_t(bytes.size);
		mn::stream_write(out, mn::block_from(len));
		mn::stream_write(out, bytes);
	}

	inline static mn::Str
	_read_string(mn::Stream in)
	{
		uint32_t len = 0;
		mn::stream_read(in, mn::block_from(len));

		auto v = mn::str_new();
		mn::str_resize(v, len);
		mn::stream_read(in, mn::block_from(v));

		return v;
	}

	inline static mn::Block
	_read_bytes(mn::Stream in)
	{
		uint32_t len = 0;
		mn::stream_read(in, mn::block_from(len));

		auto v = mn::alloc(len, alignof(uint8_t));
		mn::stream_read(in, v);

		return v;
	}

	// API
	Section
	section_constant_new(const mn::Str& name, mn::Block bytes)
	{
		Section self{};
		self.kind = Section::KIND_CONSTANT;
		self.name = clone(name);
		self.bytes = mn::block_clone(bytes);
		return self;
	}

	Section
	section_bytecode_new(const mn::Str& name, mn::Block bytes)
	{
		Section self{};
		self.kind = Section::KIND_BYTECODE;
		self.name = clone(name);
		self.bytes = mn::block_clone(bytes);
		return self;
	}

	void
	section_free(Section& self)
	{
		mn::str_free(self.name);
		mn::free(self.bytes);
	}

	void
	section_save(const Section& self, mn::Stream out)
	{
		mn::stream_write(out, mn::block_from(self.kind));
		_write_string(out, self.name);
		_write_bytes(out, self.bytes);
	}

	Section
	section_load(mn::Stream in)
	{
		Section self{};
		mn::stream_read(in, mn::block_from(self.kind));
		self.name = _read_string(in);
		self.bytes = _read_bytes(in);
		return self;
	}


	Reloc
	reloc_new()
	{
		Reloc self{};
		self.source_name = mn::str_new();
		self.target_name = mn::str_new();
		return self;
	}

	void
	reloc_free(Reloc& self)
	{
		mn::str_free(self.source_name);
		mn::str_free(self.target_name);
	}

	void
	reloc_save(const Reloc& self, mn::Stream out)
	{
		_write_string(out, self.source_name);
		_write_string(out, self.target_name);
		mn::stream_write(out, mn::block_from(self.source_offset));
	}

	Reloc
	reloc_load(mn::Stream in)
	{
		Reloc self{};
		self.source_name = _read_string(in);
		self.target_name = _read_string(in);
		mn::stream_read(in, mn::block_from(self.source_offset));
		return self;
	}


	Pkg
	pkg_new()
	{
		Pkg self{};
		self.sections = mn::map_new<mn::Str, Section>();
		self.relocs = mn::buf_new<Reloc>();
		self.c_procs = mn::buf_new<C_Proc>();
		return self;
	}

	void
	pkg_free(Pkg& self)
	{
		for (auto& [key, value] : self.sections)
			section_free(value);
		map_free(self.sections);

		destruct(self.relocs);
		destruct(self.c_procs);
	}

	void
	pkg_proc_add(Pkg& self, const mn::Str& name, mn::Block bytes)
	{
		assert(mn::map_lookup(self.sections, name) == nullptr);
		auto section = section_bytecode_new(name, bytes);
		mn::map_insert(self.sections, section.name, section);
	}

	void
	pkg_constant_add(Pkg& self, const mn::Str &name, mn::Block bytes)
	{
		assert(mn::map_lookup(self.sections, name) == nullptr);
		auto section = section_constant_new(name, bytes);
		mn::map_insert(self.sections, section.name, section);
	}

	void
	pkg_reloc_add(Pkg& self, const mn::Str &source_name, uint64_t source_offset, const mn::Str &target_name)
	{
		mn::buf_push(self.relocs, Reloc{
			clone(source_name),
			clone(target_name),
			source_offset
		});
	}

	void
	pkg_save(const Pkg& self, const mn::Str& filename)
	{
		auto f = mn::file_open(filename, mn::IO_MODE::WRITE, mn::OPEN_MODE::CREATE_OVERWRITE);
		assert(f != nullptr);
		mn_defer(mn::file_close(f));

		// write sections
		uint32_t len = uint32_t(self.sections.count);
		mn::stream_write(f, mn::block_from(len));
		for (const auto& [_, value] : self.sections)
			section_save(value, f);

		len = uint32_t(self.relocs.count);
		mn::stream_write(f, mn::block_from(len));
		for (const auto& reloc : self.relocs)
			reloc_save(reloc, f);


		// write c procs count
		len = uint32_t(self.c_procs.count);
		mn::stream_write(f, mn::block_from(len));

		// write each proc
		for(const auto &proc: self.c_procs)
		{
			_write_string(f, proc.lib);
			_write_string(f, proc.name);

			// write arg_types count
			len = uint32_t(proc.arg_types.count);
			mn::stream_write(f, mn::block_from(len));
			mn::stream_write(f, mn::block_from(proc.arg_types));

			// write return type
			mn::stream_write(f, mn::block_from(proc.ret));
		}
	}

	Pkg
	pkg_load(const mn::Str& filename)
	{
		auto self = pkg_new();

		auto f = mn::file_open(filename, mn::IO_MODE::READ, mn::OPEN_MODE::OPEN_ONLY);
		assert(f != nullptr);
		mn_defer(mn::file_close(f));

		// read sections count
		uint32_t len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::map_reserve(self.sections, len);

		// read each section
		for(size_t i = 0; i < len; ++i)
		{
			auto section = section_load(f);
			mn::map_insert(self.sections, section.name, section);
		}

		// read relocs count
		len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::buf_reserve(self.relocs, len);

		// read each reloc
		for(size_t i = 0; i < len; ++i)
			mn::buf_push(self.relocs, reloc_load(f));

		
		// read c procs count
		len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::buf_reserve(self.c_procs, len);

		// read each c proc
		for(size_t i = 0; i < len; ++i)
		{
			auto proc = c_proc_new();
			proc.lib = _read_string(f);
			proc.name = _read_string(f);

			// read args count
			uint32_t arg_len = 0;
			mn::stream_read(f, mn::block_from(arg_len));
			// read arg_types
			mn::buf_resize(proc.arg_types, arg_len);
			mn::stream_read(f, mn::block_from(proc.arg_types));

			// read return type
			mn::stream_read(f, mn::block_from(proc.ret));

			mn::buf_push(self.c_procs, proc);
		}

		return self;
	}

	mn::Err
	pkg_core_load(const Pkg& self, Core& core, uint64_t stack_size_in_bytes)
	{
		auto loaded_libraries = mn::map_new<mn::Str, size_t>();
		auto loaded_c_procs_table = mn::map_new<mn::Str, size_t>();
		mn_defer({
			mn::map_free(loaded_libraries);
			destruct(loaded_c_procs_table);
		});

		// search and open the libraries
		for(const auto& cproc: self.c_procs)
		{
			// check the loaded libraries
			mn::Library lib = nullptr;
			if(auto it = mn::map_lookup(loaded_libraries, cproc.lib))
			{
				lib = core.c_libraries[it->value];
			}
			else
			{
				if (cproc.lib == "C")
				{
					lib = mn::library_open("msvcrt.dll");
				}
				else
				{
					lib = mn::library_open(mn::str_tmpf("{}.{}", cproc.lib, "dll"));
				}
				if(lib == nullptr)
					return mn::Err{"'{}' library not found", cproc.lib};
				// add the library to the table
				mn::map_insert(loaded_libraries, cproc.lib, core.c_libraries.count);
				// add the library to the core
				mn::buf_push(core.c_libraries, lib);
			}

			// now we have the library we need to get the proc from it
			auto ptr = mn::library_proc(lib, cproc.name);
			if(ptr == nullptr)
				return mn::Err{"'{}.{}' procedure not found", cproc.lib, cproc.name};

			// add the proc to the table
			size_t proc_index = core.c_procs_desc.count;
			if(cproc.lib == "C")
				mn::map_insert(loaded_c_procs_table, mn::strf("C.{}", cproc.name), proc_index);
			else
				mn::map_insert(loaded_c_procs_table, mn::strf("C.{}.{}", cproc.lib, cproc.name), proc_index);

			// add the proc to the core
			mn::buf_push(core.c_procs_desc, clone(cproc));
			mn::buf_push(core.c_procs_address, ptr);
		}

		auto section_offset_table = mn::map_new<mn::Str, uint64_t>();
		mn_defer(mn::map_free(section_offset_table));

		for(const auto&[key, value]: self.sections)
		{
			switch(value.kind)
			{
			case Section::KIND_BYTECODE:
			{
				mn::map_insert(section_offset_table, key, uint64_t(core.bytecode.count));
				auto old_count = core.bytecode.count;
				mn::buf_resize(core.bytecode, value.bytes.size);
				::memcpy(core.bytecode.ptr + old_count, value.bytes.ptr, value.bytes.size);
				break;
			}
			case Section::KIND_CONSTANT:
			{
				mn::map_insert(section_offset_table, key, uint64_t(core.stack.count));
				auto old_count = core.stack.count;
				mn::buf_resize(core.stack, value.bytes.size);
				::memcpy(core.stack.ptr + old_count, value.bytes.ptr, value.bytes.size);
				break;
			}
			default:
				assert(false && "unreachable");
				break;
			}
		}

		mn::buf_resize(core.stack, core.stack.count + stack_size_in_bytes);

		// after loading procs we'll need to perform the relocs
		for(const auto& reloc: self.relocs)
		{
			auto source_it = mn::map_lookup(section_offset_table, reloc.source_name);
			if (source_it == nullptr)
				return mn::Err{ "relocation section '{}' not found", reloc.source_name };

			const auto& [_1, source_section] = *mn::map_lookup(self.sections, reloc.source_name);
			if(source_section.kind != Section::KIND_BYTECODE)
				return mn::Err{ "unsupported relocation in a non-procedure section '{}'", reloc.source_name };

			if(mn::str_prefix(reloc.target_name, "C."))
			{
				auto target_it = mn::map_lookup(loaded_c_procs_table, reloc.target_name);
				if (target_it == nullptr)
					return mn::Err{ "relocation target procedure '{}' not found", reloc.target_name };

				_write64(core.bytecode.ptr + source_it->value + reloc.source_offset, target_it->value);
			}
			else
			{
				auto target_it = mn::map_lookup(section_offset_table, reloc.target_name);
				if (target_it == nullptr)
					return mn::Err{ "relocation target section '{}' not found", reloc.target_name };

				const auto &[_2, target_section] = *mn::map_lookup(self.sections, reloc.target_name);
				switch (target_section.kind)
				{
				case Section::KIND_BYTECODE:
					_write64(
						core.bytecode.ptr + source_it->value + reloc.source_offset,
						target_it->value
					);
					break;
				case Section::KIND_CONSTANT:
					_write64(
						core.bytecode.ptr + source_it->value + reloc.source_offset,
						uint64_t(core.stack.ptr + target_it->value)
					);
					break;
				default:
					assert(false && "unreachable");
					break;
				}
			}
		}

		auto main_it = mn::map_lookup(section_offset_table, mn::str_lit("main"));
		if(main_it == nullptr)
			return mn::Err{ "undefined main proc" };

		core.r[Reg_IP].u64 = main_it->value;
		core.r[Reg_SP].ptr = core.stack.ptr + stack_size_in_bytes;
		return mn::Err{};
	}
}
