#pragma once

#include "vm/Op.h"
#include "vm/Reg.h"
#include "vm/Util.h"

#include <mn/Buf.h>

namespace vm
{
	inline static uint8_t
	imm_ext_byte()
	{
		Ext e{};
		e.is_ext = true;
		e.address_mode = ADDRESS_MODE_IMM;
		return *(uint8_t*)&e;
	}

	inline static uint8_t
	mem_ext_byte(SCALE_MODE scale, uint64_t shift)
	{
		Ext e{};
		e.is_ext = true;
		e.address_mode = ADDRESS_MODE_MEM;
		e.is_shifted = shift == 0;
		e.scale_mode = scale;
		return *(uint8_t*)&e;
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
			struct
			{
				Reg reg;
				SCALE_MODE scale;
				uint64_t shift;
			} mem;
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
	op_mem(Reg r, SCALE_MODE scale, uint64_t shift)
	{
		Operand op{};
		op.kind = Operand::KIND_MEM;
		op.mem.reg = r;
		op.mem.scale = scale;
		op.mem.shift = shift;
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
		case Operand::KIND_REG:
			return 0;
		case Operand::KIND_IMM8:
		case Operand::KIND_IMM16:
		case Operand::KIND_IMM32:
		case Operand::KIND_IMM64:
			return imm_ext_byte();
		case Operand::KIND_MEM:
			return mem_ext_byte(op.mem.scale, op.mem.shift);
		default:
			assert(false && "unreachable");
			return 0;
		}
	}

	inline static void
	op_push(mn::Buf<uint8_t>& code, Operand op)
	{
		switch(op.kind)
		{
		case Operand::KIND_NONE:
			// do nothing
			break;
		case Operand::KIND_REG:
			push8(code, op.reg);
			break;
		case Operand::KIND_IMM8:
			push8(code, op.imm8);
			break;
		case Operand::KIND_IMM16:
			push16(code, op.imm16);
			break;
		case Operand::KIND_IMM32:
			push32(code, op.imm32);
			break;
		case Operand::KIND_IMM64:
			push64(code, op.imm64);
			break;
		case Operand::KIND_MEM:
			push8(code, op.mem.reg);
			push64(code, op.mem.shift);
			break;
		default:
			assert(false && "unreachable");
			break;
		}
	}

	inline static void
	ins_push(mn::Buf<uint8_t>& code, Op opcode, Operand dst, Operand src)
	{
		if (auto ext = op_ext(dst); ext != 0)
			push8(code, ext);

		if (auto ext = op_ext(src); ext != 0)
			push8(code, ext);

		push8(code, opcode);
		op_push(code, dst);
		op_push(code, src);
	}
}
