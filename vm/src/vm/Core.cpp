#include "vm/Core.h"
#include "vm/Op.h"
#include "vm/Util.h"

namespace vm
{
	inline static Op
	pop_op(Core& self, const mn::Buf<uint8_t>& code)
	{
		return Op(pop8(code, self.r[Reg_IP].u64));
	}

	inline static Reg
	pop_reg(Core& self, const mn::Buf<uint8_t>& code)
	{
		return Reg(pop8(code, self.r[Reg_IP].u64));
	}

	inline static Reg_Val&
	load_reg(Core& self, const mn::Buf<uint8_t>& code)
	{
		Reg i = pop_reg(self, code);
		assert(i < Reg_COUNT);
		return self.r[i];
	}

	// API
	void
	core_ins_execute(Core& self, const mn::Buf<uint8_t>& code)
	{
		auto op = pop_op(self, code);
		switch(op)
		{
		case Op_LOAD8:
		{
			auto& dst = load_reg(self, code);
			dst.u8 = pop8(code, self.r[Reg_IP].u64);
			break;
		}
		case Op_LOAD16:
		{
			auto& dst = load_reg(self, code);
			dst.u16 = pop16(code, self.r[Reg_IP].u64);
			break;
		}
		case Op_LOAD32:
		{
			auto& dst = load_reg(self, code);
			dst.u32 = pop32(code, self.r[Reg_IP].u64);
			break;
		}
		case Op_LOAD64:
		{
			auto& dst = load_reg(self, code);
			dst.u64 = pop64(code, self.r[Reg_IP].u64);
			break;
		}
		case Op_ADD8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u8 += src.u8;
			break;
		}
		case Op_ADD16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u16 += src.u16;
			break;
		}
		case Op_ADD32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u32 += src.u32;
			break;
		}
		case Op_ADD64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u64 += src.u64;
			break;
		}
		case Op_SUB8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u8 -= src.u8;
			break;
		}
		case Op_SUB16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u16 -= src.u16;
			break;
		}
		case Op_SUB32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u32 -= src.u32;
			break;
		}
		case Op_SUB64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u64 -= src.u64;
			break;
		}
		case Op_MUL8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u8 *= src.u8;
			break;
		}
		case Op_MUL16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u16 *= src.u16;
			break;
		}
		case Op_MUL32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u32 *= src.u32;
			break;
		}
		case Op_MUL64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u64 *= src.u64;
			break;
		}
		case Op_IMUL8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i8 *= src.i8;
			break;
		}
		case Op_IMUL16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i16 *= src.i16;
			break;
		}
		case Op_IMUL32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i32 *= src.i32;
			break;
		}
		case Op_IMUL64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i64 *= src.i64;
			break;
		}
		case Op_DIV8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u8 /= src.u8;
			break;
		}
		case Op_DIV16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u16 /= src.u16;
			break;
		}
		case Op_DIV32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u32 /= src.u32;
			break;
		}
		case Op_DIV64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.u64 /= src.u64;
			break;
		}
		case Op_IDIV8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i8 /= src.i8;
			break;
		}
		case Op_IDIV16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i16 /= src.i16;
			break;
		}
		case Op_IDIV32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i32 /= src.i32;
			break;
		}
		case Op_IDIV64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			dst.i64 /= src.i64;
			break;
		}
		case Op_HALT:
			self.state = Core::STATE_HALT;
			break;
		case Op_IGL:
		default:
			self.state = Core::STATE_ERR;
			break;
		}
	}
}