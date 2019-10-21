#pragma once

#include "as/Pos.h"
#include "as/Rng.h"
#include "as/Tkn.h"

#include <mn/Str.h>

namespace as
{
	// Error representation is the location and the message
	// we want to show at that location
	struct Err
	{
		Pos pos;
		Rng rng;
		mn::Str msg;
	};

	inline static Err
	err_str(const mn::Str& msg)
	{
		Err self{};
		self.msg = msg;
		return self;
	}

	inline static Err
	err_pos(const Pos& p, const mn::Str& m)
	{
		Err self{};
		self.pos = p;
		self.msg = m;
		return self;
	}

	inline static Err
	err_tkn(const Tkn& t, const mn::Str& m)
	{
		Err self{};
		self.pos = t.pos;
		self.rng = t.rng;
		self.msg = m;
		return self;
	}

	inline static void
	err_free(Err& self)
	{
		mn::str_free(self.msg);
	}

	inline static void
	destruct(Err& self)
	{
		err_free(self);
	}
}