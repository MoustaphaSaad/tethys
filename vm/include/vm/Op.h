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

		// IMMADD [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMADD8,
		Op_IMMADD16,
		Op_IMMADD32,
		Op_IMMADD64,

		// SUB [dst + op1] [op2]
		Op_SUB8,
		Op_SUB16,
		Op_SUB32,
		Op_SUB64,

		// IMMSUB [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMSUB8,
		Op_IMMSUB16,
		Op_IMMSUB32,
		Op_IMMSUB64,

		// MUL [dst + op1] [op2]
		Op_MUL8,
		Op_MUL16,
		Op_MUL32,
		Op_MUL64,

		// IMMMUL [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMMUL8,
		Op_IMMMUL16,
		Op_IMMMUL32,
		Op_IMMMUL64,

		// IMUL [dst + op1] [op2]
		Op_IMUL8,
		Op_IMUL16,
		Op_IMUL32,
		Op_IMUL64,

		// IMMIMUL [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMIMUL8,
		Op_IMMIMUL16,
		Op_IMMIMUL32,
		Op_IMMIMUL64,

		// DIV [dst + op1] [op2]
		Op_DIV8,
		Op_DIV16,
		Op_DIV32,
		Op_DIV64,

		// IMMDIV [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMDIV8,
		Op_IMMDIV16,
		Op_IMMDIV32,
		Op_IMMDIV64,

		// IDIV [dst + op1] [op2]
		Op_IDIV8,
		Op_IDIV16,
		Op_IDIV32,
		Op_IDIV64,

		// IMMIDIV [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMIDIV8,
		Op_IMMIDIV16,
		Op_IMMIDIV32,
		Op_IMMIDIV64,

		// unsigned compare
		// CMP [op1] [op2]
		Op_CMP8,
		Op_CMP16,
		Op_CMP32,
		Op_CMP64,

		// IMMCMP [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMCMP8,
		Op_IMMCMP16,
		Op_IMMCMP32,
		Op_IMMCMP64,

		// signed compare
		// ICMP [op1] [op2]
		Op_ICMP8,
		Op_ICMP16,
		Op_ICMP32,
		Op_ICMP64,

		// IMMICMP [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
		Op_IMMICMP8,
		Op_IMMICMP16,
		Op_IMMICMP32,
		Op_IMMICMP64,

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

		// performs a call instruction
		// CALL [address unsigned 64-bit]
		Op_CALL,

		// returns from proc calls
		// RET
		Op_RET,

		// TEMPORARY Op
		// prints a debug string
		// DEBUGSTR [register = address unsigned 64-bit]
		Op_DEBUGSTR,

		Op_HALT,
	};
}