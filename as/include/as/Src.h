#pragma once

#include "as/Exports.h"
#include "as/Err.h"
#include "as/Rng.h"
#include "as/Tkn.h"
#include "as/Parse_Tree.h"

#include <mn/Str.h>
#include <mn/Buf.h>
#include <mn/Str_Intern.h>

namespace as
{
	struct Src
	{
		mn::Str path;
		mn::Str content;
		mn::Buf<Line> lines;
		mn::Str_Intern str_table;
		mn::Buf<Err> errs;
		mn::Buf<Tkn> tkns;
		mn::Buf<Decl*> decls;
	};

	AS_EXPORT Src*
	src_from_file(const char* path);

	AS_EXPORT Src*
	src_from_str(const char* code);

	AS_EXPORT void
	src_free(Src* self);

	inline static void
	destruct(Src* self)
	{
		src_free(self);
	}

	inline static void
	src_line_begin(Src* self, const char* begin)
	{
		mn::buf_push(self->lines, Line{begin, begin});
	}

	inline static void
	src_line_end(Src* self, const char* end)
	{
		mn::buf_top(self->lines).end = end;
	}

	inline static void
	src_err(Src* self, const Err& e)
	{
		mn::buf_push(self->errs, e);
	}

	inline static void
	src_err(Src* self, const mn::Str& msg)
	{
		mn::buf_push(self->errs, err_str(msg));
	}

	inline static void
	src_err(Src* self, const Pos& pos, const mn::Str& msg)
	{
		mn::buf_push(self->errs, err_pos(pos, msg));
	}

	inline static void
	src_err(Src* self, const Tkn& tkn, const mn::Str& msg)
	{
		mn::buf_push(self->errs, err_tkn(tkn, msg));
	}

	inline static bool
	src_has_err(Src* self)
	{
		return self->errs.count > 0;
	}

	inline static void
	src_tkn(Src* self, const Tkn& tkn)
	{
		mn::buf_push(self->tkns, tkn);
	}

	AS_EXPORT mn::Str
	src_tkns_dump(Src* self, mn::Allocator allocator = mn::allocator_top());

	AS_EXPORT mn::Str
	src_errs_dump(Src* self, mn::Allocator allocator = mn::allocator_top());
}