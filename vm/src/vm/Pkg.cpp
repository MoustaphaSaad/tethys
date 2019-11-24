#include "vm/Pkg.h"

#include <mn/File.h>
#include <mn/Path.h>
#include <mn/Defer.h>

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
		self.procs = mn::map_new<mn::Str, mn::Buf<uint8_t>>();
		self.relocs = mn::buf_new<Reloc>();
		return self;
	}

	void
	pkg_free(Pkg& self)
	{
		destruct(self.procs);
		destruct(self.relocs);
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

		return self;
	}

	mn::Buf<uint8_t>
	pkg_bytecode_main_generate(const Pkg& self, mn::Allocator allocator)
	{
		auto res = mn::buf_with_allocator<uint8_t>(allocator);

		auto loaded_procs_table = mn::map_new<mn::Str, uint64_t>();
		mn_defer(mn::map_free(loaded_procs_table));

		// append each proc bytecode to the result bytecode array
		for(auto it = mn::map_begin(self.procs);
			it != mn::map_end(self.procs);
			it = mn::map_next(self.procs, it))
		{
			// add the proc name and offset in the loaded_procs_table
			mn::map_insert(loaded_procs_table, it->key, uint64_t(res.count));
			mn::buf_concat(res, it->value);
		}

		// after loading procs we'll need to perform the relocs
		for(const auto& reloc: self.relocs)
		{
			auto source_it = mn::map_lookup(loaded_procs_table, reloc.source_name);
			auto target_it = mn::map_lookup(loaded_procs_table, reloc.target_name);
			assert(source_it && target_it);
			write64(res.ptr + source_it->value + reloc.source_offset, target_it->value);
		}

		return res;
	}
}
