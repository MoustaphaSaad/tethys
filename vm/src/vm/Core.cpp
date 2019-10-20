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