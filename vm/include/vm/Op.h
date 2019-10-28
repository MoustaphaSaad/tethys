#pragma once

#include <stdint.h>

namespace vm
{
	enum Op: uint8_t
	{
		// illegal opcode
		Op_IGL,

		// LOAD [dst] [constant]
		Op_LOAD8,
		Op_LOAD16,
		Op_LOAD32,
		Op_LOAD64,

		// ADD [dst + op1] [op2]
		Op_ADD8,
		Op_ADD16,
		Op_ADD32,
		Op_ADD64,

		// SUB [dst + op1] [op2]
		Op_SUB8,
		Op_SUB16,
		Op_SUB32,
		Op_SUB64,

		// MUL [dst + op1] [op2]
		Op_MUL8,
		Op_MUL16,
		Op_MUL32,
		Op_MUL64,

		// IMUL [dst + op1] [op2]
		Op_IMUL8,
		Op_IMUL16,
		Op_IMUL32,
		Op_IMUL64,

		// DIV [dst + op1] [op2]
		Op_DIV8,
		Op_DIV16,
		Op_DIV32,
		Op_DIV64,

		// IDIV [dst + op1] [op2]
		Op_IDIV8,
		Op_IDIV16,
		Op_IDIV32,
		Op_IDIV64,

		Op_HALT,
	};
}