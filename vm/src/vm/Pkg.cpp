#include "vm/Pkg.h"
#include "vm/Core.h"

#include <mn/File.h>
#include <mn/Path.h>
#include <mn/Defer.h>
#include <mn/Library.h>

namespace vm
{
	inline static void
	write64(uint8_t* ptr, uint64_t v)
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
	write_string(mn::File f, const mn::Str& str)
	{
		uint32_t len = uint32_t(str.count);
		mn::stream_write(f, mn::block_from(len));
		mn::stream_write(f, mn::block_from(str));
	}

	inline static void
	write_bytes(mn::File f, const mn::Buf<uint8_t>& bytes)
	{
		uint32_t len = uint32_t(bytes.count);
		mn::stream_write(f, mn::block_from(len));
		mn::stream_write(f, mn::block_from(bytes));
	}

	inline static mn::Str
	read_string(mn::File f)
	{
		uint32_t len = 0;
		mn::stream_read(f, mn::block_from(len));

		auto v = mn::str_new();
		mn::str_resize(v, len);
		mn::stream_read(f, mn::block_from(v));

		return v;
	}

	inline static mn::Buf<uint8_t>
	read_bytes(mn::File f)
	{
		uint32_t len = 0;
		mn::stream_read(f, mn::block_from(len));

		auto v = mn::buf_with_count<uint8_t>(len);
		mn::stream_read(f, mn::block_from(v));

		return v;
	}

	// API
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

	Pkg
	pkg_new()
	{
		Pkg self{};
		self.constants = mn::map_new<mn::Str, mn::Buf<uint8_t>>();
		self.procs = mn::map_new<mn::Str, mn::Buf<uint8_t>>();
		self.relocs = mn::buf_new<Reloc>();
		self.constant_relocs = mn::buf_new<Reloc>();
		self.c_procs = mn::buf_new<C_Proc>();
		return self;
	}

	void
	pkg_free(Pkg& self)
	{
		destruct(self.constants);
		destruct(self.procs);
		destruct(self.relocs);
		destruct(self.constant_relocs);
		destruct(self.c_procs);
	}

	bool
	pkg_proc_add(Pkg& self, const mn::Str& name, const mn::Buf<uint8_t>& bytes)
	{
		if (mn::map_lookup(self.procs, name) != nullptr)
			return false;

		mn::map_insert(self.procs, name, bytes);
		return true;
	}

	void
	pkg_reloc_add(Pkg& self, mn::Str source_name, uint64_t source_offset, mn::Str target_name)
	{
		mn::buf_push(self.relocs, Reloc{ source_name, target_name, source_offset });
	}

	bool
	pkg_constant_add(Pkg& self, mn::Str constant_name, mn::Block bytes)
	{
		auto it = mn::map_lookup(self.constants, constant_name);
		assert(it == nullptr);
		if (it != nullptr)
			return false;

		auto buf = mn::buf_with_count<uint8_t>(bytes.size);
		::memcpy(buf.ptr, bytes.ptr, bytes.size);
		mn::map_insert(self.constants, constant_name, buf);
		return true;
	}

	void
	pkg_constant_reloc_add(Pkg& self, mn::Str source_name, uint64_t source_offset, mn::Str target_name)
	{
		mn::buf_push(self.constant_relocs, Reloc{ source_name, target_name, source_offset });
	}

	void
	pkg_save(const Pkg& self, const mn::Str& filename)
	{
		auto f = mn::file_open(filename, mn::IO_MODE::WRITE, mn::OPEN_MODE::CREATE_OVERWRITE);
		assert(f != nullptr);
		mn_defer(mn::file_close(f));

		// write procs count
		uint32_t len = uint32_t(self.procs.count);
		mn::stream_write(f, mn::block_from(len));

		// write each proc
		for(auto it = mn::map_begin(self.procs);
			it != mn::map_end(self.procs);
			it = mn::map_next(self.procs, it))
		{
			write_string(f, it->key);
			write_bytes(f, it->value);
		}

		// write relocs count
		len = uint32_t(self.relocs.count);
		mn::stream_write(f, mn::block_from(len));

		// write each reloc
		for(const auto& reloc: self.relocs)
		{
			write_string(f, reloc.source_name);
			write_string(f, reloc.target_name);
			mn::stream_write(f, mn::block_from(reloc.source_offset));
		}

		// write constants count
		len = uint32_t(self.constants.count);
		mn::stream_write(f, mn::block_from(len));

		// write each constant
		for(auto it = mn::map_begin(self.constants);
			it != mn::map_end(self.constants);
			it = mn::map_next(self.constants, it))
		{
			write_string(f, it->key);
			write_bytes(f, it->value);
		}

		// write relocs count
		len = uint32_t(self.constant_relocs.count);
		mn::stream_write(f, mn::block_from(len));

		// write each reloc
		for(const auto& reloc: self.constant_relocs)
		{
			write_string(f, reloc.source_name);
			write_string(f, reloc.target_name);
			mn::stream_write(f, mn::block_from(reloc.source_offset));
		}

		// write c procs count
		len = uint32_t(self.c_procs.count);
		mn::stream_write(f, mn::block_from(len));

		// write each proc
		for(const auto &proc: self.c_procs)
		{
			write_string(f, proc.lib);
			write_string(f, proc.name);
			
			// write arg_types count
			len = uint32_t(proc.arg_types.count);
			mn::stream_write(f, mn::block_from(len));
			mn::stream_write(f, mn::block_from(proc.arg_types));
		}
	}

	Pkg
	pkg_load(const mn::Str& filename)
	{
		auto self = pkg_new();

		auto f = mn::file_open(filename, mn::IO_MODE::READ, mn::OPEN_MODE::OPEN_ONLY);
		assert(f != nullptr);
		mn_defer(mn::file_close(f));

		// read procs count
		uint32_t len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::map_reserve(self.procs, len);

		// read each proc
		for(size_t i = 0; i < len; ++i)
		{
			auto name = read_string(f);
			auto bytes = read_bytes(f);
			pkg_proc_add(self, name, bytes);
		}

		// read relocs count
		len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::buf_reserve(self.relocs, len);

		// read each reloc
		for(size_t i = 0; i < len; ++i)
		{
			Reloc reloc{};
			reloc.source_name = read_string(f);
			reloc.target_name = read_string(f);
			mn::stream_read(f, mn::block_from(reloc.source_offset));
			mn::buf_push(self.relocs, reloc);
		}

		// read constants count
		len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::map_reserve(self.constants, len);

		// read each constant
		for(size_t i = 0; i < len; ++i)
		{
			auto name = read_string(f);
			auto bytes = read_bytes(f);
			mn::map_insert(self.constants, name, bytes);
		}

		// read relocs count
		len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::buf_reserve(self.constant_relocs, len);

		// read each reloc
		for(size_t i = 0; i < len; ++i)
		{
			Reloc reloc{};
			reloc.source_name = read_string(f);
			reloc.target_name = read_string(f);
			mn::stream_read(f, mn::block_from(reloc.source_offset));
			mn::buf_push(self.constant_relocs, reloc);
		}

		// read c procs count
		len = 0;
		mn::stream_read(f, mn::block_from(len));
		mn::buf_reserve(self.c_procs, len);

		// read each c proc
		for(size_t i = 0; i < len; ++i)
		{
			auto proc = c_proc_new();
			proc.lib = read_string(f);
			proc.name = read_string(f);

			// read args count
			uint32_t arg_len = 0;
			mn::stream_read(f, mn::block_from(arg_len));
			// read arg_types
			mn::buf_resize(proc.arg_types, arg_len);
			mn::stream_read(f, mn::block_from(proc.arg_types));

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

		auto loaded_procs_table = mn::map_new<mn::Str, uint64_t>();
		mn_defer(mn::map_free(loaded_procs_table));

		// append each proc bytecode to the result bytecode array
		for(auto it = mn::map_begin(self.procs);
			it != mn::map_end(self.procs);
			it = mn::map_next(self.procs, it))
		{
			// add the proc name and offset in the loaded_procs_table
			mn::map_insert(loaded_procs_table, it->key, uint64_t(core.bytecode.count));
			mn::buf_concat(core.bytecode, it->value);
		}

		// after loading procs we'll need to perform the relocs
		for(const auto& reloc: self.relocs)
		{
			auto source_it = mn::map_lookup(loaded_procs_table, reloc.source_name);
			if (source_it == nullptr)
				return mn::Err{ "relocation source procedure '{}' not found", reloc.source_name };

			if(mn::str_prefix(reloc.target_name, "C."))
			{
				auto target_it = mn::map_lookup(loaded_c_procs_table, reloc.target_name);
				if (target_it == nullptr)
					return mn::Err{ "relocation target procedure '{}' not found", reloc.target_name };

				write64(core.bytecode.ptr + source_it->value + reloc.source_offset, target_it->value);
			}
			else
			{
				auto target_it = mn::map_lookup(loaded_procs_table, reloc.target_name);
				if (target_it == nullptr)
					return mn::Err{ "relocation target procedure '{}' not found", reloc.target_name };

				write64(core.bytecode.ptr + source_it->value + reloc.source_offset, target_it->value);
			}
		}

		auto loaded_constants_table = mn::map_new<mn::Str, uint64_t>();
		mn_defer(mn::map_free(loaded_constants_table));

		// append each constant
		for(auto it = mn::map_begin(self.constants);
			it != mn::map_end(self.constants);
			it = mn::map_next(self.constants, it))
		{
			mn::map_insert(loaded_constants_table, it->key, uint64_t(core.stack.count));
			mn::buf_concat(core.stack, it->value);
		}

		mn::buf_resize(core.stack, stack_size_in_bytes);

		// after loading constants we'll need to perform the relocs
		for(const auto& reloc: self.constant_relocs)
		{
			auto source_it = mn::map_lookup(loaded_procs_table, reloc.source_name);
			if (source_it == nullptr)
				return mn::Err{ "relocation source procedure '{}' not found", reloc.source_name };

			auto target_it = mn::map_lookup(loaded_constants_table, reloc.target_name);
			if (target_it == nullptr)
				return mn::Err{ "relocation target constant '{}' not found", reloc.target_name };

			write64(core.bytecode.ptr + source_it->value + reloc.source_offset, uint64_t(core.stack.ptr + target_it->value));
		}

		auto main_it = mn::map_lookup(loaded_procs_table, mn::str_lit("main"));
		if(main_it == nullptr)
			return mn::Err{ "undefined main proc" };

		core.r[Reg_IP].u64 = main_it->value;
		core.r[Reg_SP].ptr = core.stack.ptr + stack_size_in_bytes;
		return mn::Err{};
	}
}
