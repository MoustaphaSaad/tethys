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

		Op_HALT,
	};
}