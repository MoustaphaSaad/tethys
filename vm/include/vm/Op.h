#pragma once

#include <stdint.h>

namespace vm
{
	enum Op: int8_t
	{
		// illegal opcode
		Op_IGL,

		// MOV [dst] [src]
		Op_MOV8,
		Op_MOV16,
		Op_MOV32,
		Op_MOV64,

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

		// calls a C function
		// C_CALL [unsigned 64-bit index into c_proc array in core]
		Op_C_CALL,

		Op_HALT,
	};

	// EXT = 0123 4567
	// EXT[0, 1] = addressing mode, choose from [reg, imm, mem]
	// EXT[2] = shifted addressing mode
	// EXT[3, 4, 5] = reserved bits -> always 0
	// EXT[6, 7] = scale mode, choose from [1, 2, 4, 8] scaling
	// add two extension bytes before each operand, [opcode] [dst ext] [dst] [src ext] [src]

	enum ADDRESS_MODE: uint8_t
	{
		ADDRESS_MODE_REG,
		ADDRESS_MODE_IMM,
		ADDRESS_MODE_MEM,
	};

	enum SCALE_MODE: uint8_t
	{
		SCALE_MODE_1X,
		SCALE_MODE_2X,
		SCALE_MODE_4X,
		SCALE_MODE_8X,
	};

	struct Ext
	{
		ADDRESS_MODE address_mode;
		bool is_shifted;
		uint8_t reserved;
		SCALE_MODE scale_mode;
	};

	constexpr inline uint8_t MASK_ADDRESS_MODE	= 0b1100'0000;
	constexpr inline uint8_t MASK_IS_SHIFTED	= 0b0010'0000;
	constexpr inline uint8_t MASK_RESERVED		= 0b0001'1100;
	constexpr inline uint8_t MASK_SCALE_MODE	= 0b0000'0011;

	inline static Ext
	ext_from_byte(uint8_t b)
	{
		Ext e{};
		e.address_mode = ADDRESS_MODE((b & MASK_ADDRESS_MODE) >> 6);
		e.is_shifted   = bool(b & MASK_IS_SHIFTED);
		e.reserved	   = uint8_t(b & MASK_RESERVED) >> 2;
		e.scale_mode   = SCALE_MODE(b & MASK_SCALE_MODE);
		return e;
	}

	inline static uint8_t
	ext_to_byte(Ext e)
	{
		uint8_t b = 0;
		b |= (uint8_t(e.address_mode) << 6) & MASK_ADDRESS_MODE;
		b |= e.is_shifted ? MASK_IS_SHIFTED : 0;
		b |= (uint8_t(e.scale_mode)) & MASK_SCALE_MODE;
		return b;
	}
}