#pragma once

#include "vm/Op.h"
#include "vm/Reg.h"
#include "vm/Util.h"

#include <mn/Buf.h>

namespace vm
{
	// EXT = 0123 4567
	// EXT[0, 1] = addressing mode, choose from [reg, imm, mem]
	// add two extension bytes before each operand, [opcode] [dst ext] [dst] [src ext] [src]

	enum ADDRESS_MODE: uint8_t
	{
		ADDRESS_MODE_REG,
		ADDRESS_MODE_IMM,
		ADDRESS_MODE_MEM,
	};

	struct Ext
	{
		ADDRESS_MODE address_mode;
	};

	constexpr inline uint8_t MASK_ADDRESS_MODE	= 0b1100'0000;

	inline static Ext
	ext_from_byte(uint8_t b)
	{
		Ext e{};
		e.address_mode = ADDRESS_MODE((b & MASK_ADDRESS_MODE) >> 6);
		return e;
	}

	inline static uint8_t
	ext_to_byte(Ext e)
	{
		uint8_t b = 0;
		b |= (uint8_t(e.address_mode) << 6) & MASK_ADDRESS_MODE;
		return b;
	}

	inline static uint8_t
	reg_ext_byte()
	{
		Ext e{};
		e.address_mode = ADDRESS_MODE_REG;
		return ext_to_byte(e);
	}

	inline static uint8_t
	imm_ext_byte()
	{
		Ext e{};
		e.address_mode = ADDRESS_MODE_IMM;
		return ext_to_byte(e);
	}

	inline static uint8_t
	mem_ext_byte()
	{
		Ext e{};
		e.address_mode = ADDRESS_MODE_MEM;
		return ext_to_byte(e);
	}

	struct Operand
	{
		enum KIND
		{
			KIND_NONE,
			KIND_REG,
			KIND_IMM8,
			KIND_IMM16,
			KIND_IMM32,
			KIND_IMM64,
			KIND_MEM,
		};

		KIND kind;
		union
		{
			Reg reg;
			uint8_t imm8;
			uint16_t imm16;
			uint32_t imm32;
			uint64_t imm64;
			Reg mem;
		};
	};

	inline static Operand
	op_reg(Reg r)
	{
		Operand op{};
		op.kind = Operand::KIND_REG;
		op.reg = r;
		return op;
	}

	template<typename T>
	inline static Operand
	op_imm(T v)
	{
		Operand op{};
		if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>)
		{
			op.kind = Operand::KIND_IMM8;
			op.imm8 = uint8_t(v);
		}
		else if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>)
		{
			op.kind = Operand::KIND_IMM16;
			op.imm16 = uint16_t(v);
		}
		else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t>)
		{
			op.kind = Operand::KIND_IMM32;
			op.imm32 = uint32_t(v);
		}
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>)
		{
			op.kind = Operand::KIND_IMM64;
			op.imm64 = uint64_t(v);
		}
		else
		{
			static_assert(sizeof(T) == 0, "unsupported immediate type");
		}
		return op;
	}

	inline static Operand
	op_mem(Reg r)
	{
		Operand op{};
		op.kind = Operand::KIND_MEM;
		op.mem = r;
		return op;
	}

	inline static Operand
	op_none()
	{
		return Operand{};
	}

	inline static uint8_t
	op_ext(Operand op)
	{
		switch(op.kind)
		{
		case Operand::KIND_NONE:
			return 0;
		case Operand::KIND_REG:
			return reg_ext_byte();
		case Operand::KIND_IMM8:
		case Operand::KIND_IMM16:
		case Operand::KIND_IMM32:
		case Operand::KIND_IMM64:
			return imm_ext_byte();
		case Operand::KIND_MEM:
			return mem_ext_byte();
		default:
			assert(false && "unreachable");
			return 0;
		}
	}

	inline static uint64_t
	op_push(mn::Buf<uint8_t>& code, Operand op)
	{
		uint64_t offset = 0;
		switch(op.kind)
		{
		case Operand::KIND_NONE:
			// do nothing
			break;
		case Operand::KIND_REG:
			offset = code.count;
			push8(code, op.reg);
			break;
		case Operand::KIND_IMM8:
			offset = code.count;
			push8(code, op.imm8);
			break;
		case Operand::KIND_IMM16:
			offset = code.count;
			push16(code, op.imm16);
			break;
		case Operand::KIND_IMM32:
			offset = code.count;
			push32(code, op.imm32);
			break;
		case Operand::KIND_IMM64:
			offset = code.count;
			push64(code, op.imm64);
			break;
		case Operand::KIND_MEM:
			offset = code.count;
			push8(code, op.mem);
			break;
		default:
			assert(false && "unreachable");
			break;
		}
		return offset;
	}

	struct Ins_Op_Offsets
	{
		uint64_t dst_offset;
		uint64_t src_offset;
	};

	inline static Ins_Op_Offsets
	ins_push(mn::Buf<uint8_t>& code, Op opcode, Operand dst, Operand src)
	{
		Ins_Op_Offsets offsets{};

		push8(code, opcode);

		if (dst.kind != Operand::KIND_NONE)
		{
			push8(code, op_ext(dst));
			offsets.dst_offset = op_push(code, dst);
		}

		if (src.kind != Operand::KIND_NONE)
		{
			push8(code, op_ext(src));
			offsets.src_offset = op_push(code, src);
		}
		return offsets;
	}
}
