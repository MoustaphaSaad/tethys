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
}

#undef TOKEN_LISTING