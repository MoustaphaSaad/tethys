#include "as/Src.h"

#include <mn/Memory.h>
#include <mn/Path.h>
#include <mn/Defer.h>
#include <mn/IO.h>

namespace as
{
	inline static Src*
	src_new(const mn::Str& path, const mn::Str& code)
	{
		auto self = mn::alloc<Src>();
		self->path = path;
		self->content = code;
		self->lines = mn::buf_new<Line>();
		self->str_table = mn::str_intern_new();
		self->errs = mn::buf_new<Err>();
		self->tkns = mn::buf_new<Tkn>();
		return self;
	}

	// API
	Src*
	src_from_file(const char* path)
	{
		return src_new(mn::str_from_c(path), mn::file_content_str(path));
	}

	Src*
	src_from_str(const char* code)
	{
		return src_new(mn::str_from_c("<STRING>"), mn::str_from_c(code));
	}

	void
	src_free(Src* self)
	{
		mn::str_free(self->path);
		mn::str_free(self->content);
		destruct(self->lines);
		mn::str_intern_free(self->str_table);
		destruct(self->errs);
		mn::buf_free(self->tkns);
		mn::free(self);
	}

	mn::Str
	src_tkns_dump(Src* self, mn::Allocator allocator)
	{
		//this is a tmp stream you can use to construct strings into
		auto out = mn::memory_stream_new(allocator);
		mn_defer(mn::memory_stream_free(out));

		for(const auto& tkn: self->tkns)
		{
			mn::print_to(
				out,
				"line: {}, col: {}, kind: \"{}\" str: \"{}\"\n",
				tkn.pos.line,
				tkn.pos.col,
				Tkn::NAMES[tkn.kind],
				tkn.str
			);
		}

		return mn::memory_stream_str(out);
	}

	mn::Str
	src_errs_dump(Src* self, mn::Allocator allocator)
	{
		auto out = mn::memory_stream_new(allocator);
		mn_defer(mn::memory_stream_free(out));

		for(const Err& e: self->errs)
		{
			Line l = self->lines[e.pos.line - 1];
			//we need to put ^^^ under the word the compiler means by the error
			if(e.rng.begin && e.rng.end)
			{
				mn::print_to(out, ">> {:{}}\n", l.end - l.begin, l.begin);
				mn::print_to(out, ">> ");
				for(const char* it = l.begin; it != l.end; it = mn::rune_next(it))
				{
					auto c = mn::rune_read(it);
					if(it >= e.rng.begin && it < e.rng.end)
					{
						mn::print_to(out, "^");
					}
					else if(c == '\t')
					{
						mn::print_to(out, "\t");
					}
					else
					{
						mn::print_to(out, " ");
					}
				}
				mn::print_to(out, "\n");
			}
			mn::print_to(out, "Error[{}:{}:{}]: {}\n\n", self->path, e.pos.line, e.pos.col, e.msg);
		}
		return mn::memory_stream_str(out);
	}
}