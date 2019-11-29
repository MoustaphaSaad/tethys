#pragma once

#include "as/Pos.h"
#include "as/Rng.h"
#include "as/Token_Listing.h"

namespace as
{
	// This is token representation
	struct Tkn
	{
		enum KIND
		{
			#define TOKEN(k, s) KIND_##k
				TOKEN_LISTING
			#undef TOKEN
		};

		inline static const char* NAMES[] = {
			#define TOKEN(k, s) s
				TOKEN_LISTING
			#undef TOKEN
		};

		KIND kind;
		const char* str;
		Rng rng;
		Pos pos;

		inline operator bool() const { return kind != KIND_NONE; }
	};

	inline static bool
	is_reg(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_R0 ||
				tkn.kind == Tkn::KIND_KEYWORD_R1 ||
				tkn.kind == Tkn::KIND_KEYWORD_R2 ||
				tkn.kind == Tkn::KIND_KEYWORD_R3 ||
				tkn.kind == Tkn::KIND_KEYWORD_R4 ||
				tkn.kind == Tkn::KIND_KEYWORD_R5 ||
				tkn.kind == Tkn::KIND_KEYWORD_R6 ||
				tkn.kind == Tkn::KIND_KEYWORD_R7 ||
				tkn.kind == Tkn::KIND_KEYWORD_IP ||
				tkn.kind == Tkn::KIND_KEYWORD_SP);
	}
}

#undef TOKEN_LISTING