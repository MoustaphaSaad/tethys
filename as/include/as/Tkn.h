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
	is_numeric_constant(Tkn::KIND k)
	{
		return k == Tkn::KIND_INTEGER || k == Tkn::KIND_FLOAT;
	}

	inline static bool
	is_reg(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_R0 ||
				k == Tkn::KIND_KEYWORD_R1 ||
				k == Tkn::KIND_KEYWORD_R2 ||
				k == Tkn::KIND_KEYWORD_R3 ||
				k == Tkn::KIND_KEYWORD_R4 ||
				k == Tkn::KIND_KEYWORD_R5 ||
				k == Tkn::KIND_KEYWORD_R6 ||
				k == Tkn::KIND_KEYWORD_R7 ||
				k == Tkn::KIND_KEYWORD_IP ||
				k == Tkn::KIND_KEYWORD_SP);
	}

	inline static bool
	is_ctype(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_VOID ||
				k == Tkn::KIND_KEYWORD_CINT8 ||
				k == Tkn::KIND_KEYWORD_CINT16 ||
				k == Tkn::KIND_KEYWORD_CINT32 ||
				k == Tkn::KIND_KEYWORD_CINT64 ||
				k == Tkn::KIND_KEYWORD_CUINT8 ||
				k == Tkn::KIND_KEYWORD_CUINT16 ||
				k == Tkn::KIND_KEYWORD_CUINT32 ||
				k == Tkn::KIND_KEYWORD_CUINT64 ||
				k == Tkn::KIND_KEYWORD_CFLOAT32 ||
				k == Tkn::KIND_KEYWORD_CFLOAT64 ||
				k == Tkn::KIND_KEYWORD_CPTR);
	}

	inline static bool
	is_mov(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_I8_MOV ||
				k == Tkn::KIND_KEYWORD_I16_MOV ||
				k == Tkn::KIND_KEYWORD_I32_MOV ||
				k == Tkn::KIND_KEYWORD_I64_MOV ||
				k == Tkn::KIND_KEYWORD_U8_MOV ||
				k == Tkn::KIND_KEYWORD_U16_MOV ||
				k == Tkn::KIND_KEYWORD_U32_MOV ||
				k == Tkn::KIND_KEYWORD_U64_MOV);
	}

	inline static bool
	is_arithmetic(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_I8_ADD ||
				k == Tkn::KIND_KEYWORD_I16_ADD ||
				k == Tkn::KIND_KEYWORD_I32_ADD ||
				k == Tkn::KIND_KEYWORD_I64_ADD ||
				k == Tkn::KIND_KEYWORD_U8_ADD ||
				k == Tkn::KIND_KEYWORD_U16_ADD ||
				k == Tkn::KIND_KEYWORD_U32_ADD ||
				k == Tkn::KIND_KEYWORD_U64_ADD ||
				k == Tkn::KIND_KEYWORD_I8_SUB ||
				k == Tkn::KIND_KEYWORD_I16_SUB ||
				k == Tkn::KIND_KEYWORD_I32_SUB ||
				k == Tkn::KIND_KEYWORD_I64_SUB ||
				k == Tkn::KIND_KEYWORD_U8_SUB ||
				k == Tkn::KIND_KEYWORD_U16_SUB ||
				k == Tkn::KIND_KEYWORD_U32_SUB ||
				k == Tkn::KIND_KEYWORD_U64_SUB ||
				k == Tkn::KIND_KEYWORD_I8_MUL ||
				k == Tkn::KIND_KEYWORD_I16_MUL ||
				k == Tkn::KIND_KEYWORD_I32_MUL ||
				k == Tkn::KIND_KEYWORD_I64_MUL ||
				k == Tkn::KIND_KEYWORD_U8_MUL ||
				k == Tkn::KIND_KEYWORD_U16_MUL ||
				k == Tkn::KIND_KEYWORD_U32_MUL ||
				k == Tkn::KIND_KEYWORD_U64_MUL ||
				k == Tkn::KIND_KEYWORD_I8_DIV ||
				k == Tkn::KIND_KEYWORD_I16_DIV ||
				k == Tkn::KIND_KEYWORD_I32_DIV ||
				k == Tkn::KIND_KEYWORD_I64_DIV ||
				k == Tkn::KIND_KEYWORD_U8_DIV ||
				k == Tkn::KIND_KEYWORD_U16_DIV ||
				k == Tkn::KIND_KEYWORD_U32_DIV ||
				k == Tkn::KIND_KEYWORD_U64_DIV);
	}

	inline static bool
	is_cond_jump(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_I8_JE ||
				k == Tkn::KIND_KEYWORD_I16_JE ||
				k == Tkn::KIND_KEYWORD_I32_JE ||
				k == Tkn::KIND_KEYWORD_I64_JE ||
				k == Tkn::KIND_KEYWORD_U8_JE ||
				k == Tkn::KIND_KEYWORD_U16_JE ||
				k == Tkn::KIND_KEYWORD_U32_JE ||
				k == Tkn::KIND_KEYWORD_U64_JE ||
				k == Tkn::KIND_KEYWORD_I8_JNE ||
				k == Tkn::KIND_KEYWORD_I16_JNE ||
				k == Tkn::KIND_KEYWORD_I32_JNE ||
				k == Tkn::KIND_KEYWORD_I64_JNE ||
				k == Tkn::KIND_KEYWORD_U8_JNE ||
				k == Tkn::KIND_KEYWORD_U16_JNE ||
				k == Tkn::KIND_KEYWORD_U32_JNE ||
				k == Tkn::KIND_KEYWORD_U64_JNE ||
				k == Tkn::KIND_KEYWORD_I8_JL ||
				k == Tkn::KIND_KEYWORD_I16_JL ||
				k == Tkn::KIND_KEYWORD_I32_JL ||
				k == Tkn::KIND_KEYWORD_I64_JL ||
				k == Tkn::KIND_KEYWORD_U8_JL ||
				k == Tkn::KIND_KEYWORD_U16_JL ||
				k == Tkn::KIND_KEYWORD_U32_JL ||
				k == Tkn::KIND_KEYWORD_U64_JL ||
				k == Tkn::KIND_KEYWORD_I8_JLE ||
				k == Tkn::KIND_KEYWORD_I16_JLE ||
				k == Tkn::KIND_KEYWORD_I32_JLE ||
				k == Tkn::KIND_KEYWORD_I64_JLE ||
				k == Tkn::KIND_KEYWORD_U8_JLE ||
				k == Tkn::KIND_KEYWORD_U16_JLE ||
				k == Tkn::KIND_KEYWORD_U32_JLE ||
				k == Tkn::KIND_KEYWORD_U64_JLE ||
				k == Tkn::KIND_KEYWORD_I8_JG ||
				k == Tkn::KIND_KEYWORD_I16_JG ||
				k == Tkn::KIND_KEYWORD_I32_JG ||
				k == Tkn::KIND_KEYWORD_I64_JG ||
				k == Tkn::KIND_KEYWORD_U8_JG ||
				k == Tkn::KIND_KEYWORD_U16_JG ||
				k == Tkn::KIND_KEYWORD_U32_JG ||
				k == Tkn::KIND_KEYWORD_U64_JG ||
				k == Tkn::KIND_KEYWORD_I8_JGE ||
				k == Tkn::KIND_KEYWORD_I16_JGE ||
				k == Tkn::KIND_KEYWORD_I32_JGE ||
				k == Tkn::KIND_KEYWORD_I64_JGE ||
				k == Tkn::KIND_KEYWORD_U8_JGE ||
				k == Tkn::KIND_KEYWORD_U16_JGE ||
				k == Tkn::KIND_KEYWORD_U32_JGE ||
				k == Tkn::KIND_KEYWORD_U64_JGE);
	}

	inline static bool
	is_push_pop(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_PUSH ||
				k == Tkn::KIND_KEYWORD_POP);
	}

	inline static bool
	is_cmp(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_I8_CMP ||
				k == Tkn::KIND_KEYWORD_I16_CMP ||
				k == Tkn::KIND_KEYWORD_I32_CMP ||
				k == Tkn::KIND_KEYWORD_I64_CMP ||
				k == Tkn::KIND_KEYWORD_U8_CMP ||
				k == Tkn::KIND_KEYWORD_U16_CMP ||
				k == Tkn::KIND_KEYWORD_U32_CMP ||
				k == Tkn::KIND_KEYWORD_U64_CMP);
	}

	inline static bool
	is_pure_jump(Tkn::KIND k)
	{
		return (k == Tkn::KIND_KEYWORD_JE ||
				k == Tkn::KIND_KEYWORD_JNE ||
				k == Tkn::KIND_KEYWORD_JL ||
				k == Tkn::KIND_KEYWORD_JLE ||
				k == Tkn::KIND_KEYWORD_JG ||
				k == Tkn::KIND_KEYWORD_JGE ||
				k == Tkn::KIND_KEYWORD_JMP);
	}
}

#undef TOKEN_LISTING