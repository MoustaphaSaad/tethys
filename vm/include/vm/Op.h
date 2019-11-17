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

		// unsigned compare
		// CMP [op1] [op2]
		Op_CMP8,
		Op_CMP16,
		Op_CMP32,
		Op_CMP64,

		// signed compare
		// ICMP [op1] [op2]
		Op_ICMP8,
		Op_ICMP16,
		Op_ICMP32,
		Op_ICMP64,

		// jump unconditionall
		// JMP [offset 64-bit]
		Op_JMP,

		// jump if equal
		// JE [offset 64-bit]
		Op_JE,

		// jump if not equal
		// JNE [offset 64-bit]
		Op_JNE,

		// jump if less than
		// JL [offset 64-bit]
		Op_JL,

		// jump if less than or equal
		// JLE [offset 64-bit]
		Op_JLE,

		// jump if greater than
		// JG [offset 64-bit]
		Op_JG,

		// jump if greater than or equal
		// JGE [offset 64-bit]
		Op_JGE,

		// reads data from the src address register into the specified dst register
		// READ [dst] [src]
		Op_READ8,
		Op_READ16,
		Op_READ32,
		Op_READ64,

		// writes data from the src register into the specified dst address register
		// WRITE [dst] [src]
		Op_WRITE8,
		Op_WRITE16,
		Op_WRITE32,
		Op_WRITE64,

		// pushes the register into the stack and increment it
		// PUSH [register]
		Op_PUSH,

		// pops the register into the stack and decrement it
		// POP [register]
		Op_POP,


		Op_HALT,
	};
}