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

	inline static bool
	_valid_ptr(Core& self, void* ptr)
	{
		return ptr <= end(self.stack_memory) && ptr >= begin(self.stack_memory);
	}

	inline static bool
	_valid_next_bytes(Core& self, void* ptr, size_t size)
	{
		return _valid_ptr(self, ptr) && _valid_ptr(self, (uint8_t*)ptr + size);
	}

	// API
	Core
	core_new()
	{
		Core self{};
		self.stack_memory = mn::buf_with_count<uint8_t>(8ULL * 1024ULL * 1024ULL);
		self.r[Reg_SP].ptr = end(self.stack_memory);
		return self;
	}

	void
	core_free(Core& self)
	{
		mn::buf_free(self.stack_memory);
	}

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
		case Op_CMP8:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.u8 > op2.u8)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u8 < op2.u8)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_CMP16:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.u16 > op2.u16)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u16 < op2.u16)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_CMP32:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.u32 > op2.u32)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u32 < op2.u32)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_CMP64:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.u64 > op2.u64)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u64 < op2.u64)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP8:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.i8 > op2.i8)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i8 < op2.i8)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP16:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.i16 > op2.i16)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i16 < op2.i16)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP32:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.i32 > op2.i32)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i32 < op2.i32)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP64:
		{
			auto& op1 = load_reg(self, code);
			auto& op2 = load_reg(self, code);
			if (op1.i64 > op2.i64)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i64 < op2.i64)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_JMP:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			self.r[Reg_IP].u64 += offset;
			break;
		}
		case Op_JE:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JNE:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			if (self.cmp != Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JL:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_LESS)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JLE:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_LESS || self.cmp == Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JG:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_GREATER)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JGE:
		{
			int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_GREATER || self.cmp == Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_READ8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint8_t*)src.ptr - 1);
			if(_valid_next_bytes(self, ptr, 1) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u8 = *ptr;
			break;
		}
		case Op_READ16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint16_t*)src.ptr - 1);
			if(_valid_next_bytes(self, ptr, 2) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u16 = *ptr;
			break;
		}
		case Op_READ32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint32_t*)src.ptr - 1);
			if(_valid_next_bytes(self, ptr, 4) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u32 = *ptr;
			break;
		}
		case Op_READ64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint64_t*)src.ptr - 1);
			if(_valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u64 = *ptr;
			break;
		}
		case Op_WRITE8:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint8_t*)dst.ptr - 1);
			if(_valid_next_bytes(self, ptr, 1) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*ptr = src.u8;
			break;
		}
		case Op_WRITE16:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint16_t*)dst.ptr - 1);
			if(_valid_next_bytes(self, ptr, 2) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*ptr = src.u16;
			break;
		}
		case Op_WRITE32:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint32_t*)dst.ptr - 1);
			if(_valid_next_bytes(self, ptr, 4) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*ptr = src.u32;
			break;
		}
		case Op_WRITE64:
		{
			auto& dst = load_reg(self, code);
			auto& src = load_reg(self, code);
			auto ptr = ((uint64_t*)dst.ptr - 1);
			if(_valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*ptr = src.u64;
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