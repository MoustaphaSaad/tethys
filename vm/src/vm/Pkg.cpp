#include "vm/Pkg.h"

#include <mn/File.h>
#include <mn/Path.h>
#include <mn/Defer.h>

namespace vm
{
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
	Pkg
	pkg_new()
	{
		Pkg self{};
		self.procs = mn::map_new<mn::Str, mn::Buf<uint8_t>>();
		return self;
	}

	void
	pkg_free(Pkg& self)
	{
		destruct(self.procs);
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

		return self;
	}

	mn::Buf<uint8_t>
	pkg_load_proc(const Pkg& self, const mn::Str& name)
	{
		// this function could do other stuff but for now we just search the procs for main proc
		for(auto it = mn::map_begin(self.procs);
			it != mn::map_end(self.procs);
			it = mn::map_next(self.procs, it))
		{
			if (it->key == name)
				return mn::buf_clone(it->value);
		}
		return mn::buf_new<uint8_t>();
	}
}
