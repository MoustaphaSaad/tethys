#include "vm/Core.h"
#include "vm/Op.h"
#include "vm/Util.h"

#include <mn/IO.h>

namespace vm
{
	inline static Op
	_pop_op(Core& self)
	{
		return Op(pop8(self.bytecode, self.r[Reg_IP].u64));
	}

	inline static Reg
	_pop_reg(Core& self)
	{
		return Reg(pop8(self.bytecode, self.r[Reg_IP].u64));
	}

	inline static Reg_Val&
	_load_reg(Core& self)
	{
		Reg i = _pop_reg(self);
		assert(i < Reg_COUNT);
		return self.r[i];
	}

	inline static bool
	_valid_ptr(Core& self, void* ptr)
	{
		return ptr <= end(self.stack) && ptr >= begin(self.stack);
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
		self.bytecode = mn::buf_new<uint8_t>();
		self.stack = mn::buf_new<uint8_t>();
		return self;
	}

	void
	core_free(Core& self)
	{
		mn::buf_free(self.bytecode);
		mn::buf_free(self.stack);
	}

	void
	core_ins_execute(Core& self)
	{
		auto op = _pop_op(self);
		switch(op)
		{
		case Op_LOAD8:
		{
			auto& dst = _load_reg(self);
			dst.u8 = pop8(self.bytecode, self.r[Reg_IP].u64);
			break;
		}
		case Op_LOAD16:
		{
			auto& dst = _load_reg(self);
			dst.u16 = pop16(self.bytecode, self.r[Reg_IP].u64);
			break;
		}
		case Op_LOAD32:
		{
			auto& dst = _load_reg(self);
			dst.u32 = pop32(self.bytecode, self.r[Reg_IP].u64);
			break;
		}
		case Op_LOAD64:
		{
			auto& dst = _load_reg(self);
			dst.u64 = pop64(self.bytecode, self.r[Reg_IP].u64);
			break;
		}
		case Op_ADD8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u8 += src.u8;
			break;
		}
		case Op_ADD16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u16 += src.u16;
			break;
		}
		case Op_ADD32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u32 += src.u32;
			break;
		}
		case Op_ADD64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u64 += src.u64;
			break;
		}
		case Op_IMMADD8:
		{
			auto& dst = _load_reg(self);
			auto imm = pop8(self.bytecode, self.r[Reg_IP].u64);
			dst.u8 += imm;
			break;
		}
		case Op_IMMADD16:
		{
			auto& dst = _load_reg(self);
			auto imm = pop16(self.bytecode, self.r[Reg_IP].u64);
			dst.u16 += imm;
			break;
		}
		case Op_IMMADD32:
		{
			auto& dst = _load_reg(self);
			auto imm = pop32(self.bytecode, self.r[Reg_IP].u64);
			dst.u32 += imm;
			break;
		}
		case Op_IMMADD64:
		{
			auto& dst = _load_reg(self);
			auto imm = pop64(self.bytecode, self.r[Reg_IP].u64);
			dst.u64 += imm;
			break;
		}
		case Op_SUB8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u8 -= src.u8;
			break;
		}
		case Op_SUB16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u16 -= src.u16;
			break;
		}
		case Op_SUB32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u32 -= src.u32;
			break;
		}
		case Op_SUB64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u64 -= src.u64;
			break;
		}
		case Op_IMMSUB8:
		{
			auto& dst = _load_reg(self);
			auto imm = pop8(self.bytecode, self.r[Reg_IP].u64);
			dst.u8 -= imm;
			break;
		}
		case Op_IMMSUB16:
		{
			auto& dst = _load_reg(self);
			auto imm = pop16(self.bytecode, self.r[Reg_IP].u64);
			dst.u16 -= imm;
			break;
		}
		case Op_IMMSUB32:
		{
			auto& dst = _load_reg(self);
			auto imm = pop32(self.bytecode, self.r[Reg_IP].u64);
			dst.u32 -= imm;
			break;
		}
		case Op_IMMSUB64:
		{
			auto& dst = _load_reg(self);
			auto imm = pop64(self.bytecode, self.r[Reg_IP].u64);
			dst.u64 -= imm;
			break;
		}
		case Op_MUL8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u8 *= src.u8;
			break;
		}
		case Op_MUL16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u16 *= src.u16;
			break;
		}
		case Op_MUL32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u32 *= src.u32;
			break;
		}
		case Op_MUL64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u64 *= src.u64;
			break;
		}
		case Op_IMMMUL8:
		{
			auto& dst = _load_reg(self);
			auto imm = pop8(self.bytecode, self.r[Reg_IP].u64);
			dst.u8 *= imm;
			break;
		}
		case Op_IMMMUL16:
		{
			auto& dst = _load_reg(self);
			auto imm = pop16(self.bytecode, self.r[Reg_IP].u64);
			dst.u16 *= imm;
			break;
		}
		case Op_IMMMUL32:
		{
			auto& dst = _load_reg(self);
			auto imm = pop32(self.bytecode, self.r[Reg_IP].u64);
			dst.u32 *= imm;
			break;
		}
		case Op_IMMMUL64:
		{
			auto& dst = _load_reg(self);
			auto imm = pop64(self.bytecode, self.r[Reg_IP].u64);
			dst.u64 *= imm;
			break;
		}
		case Op_IMUL8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i8 *= src.i8;
			break;
		}
		case Op_IMUL16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i16 *= src.i16;
			break;
		}
		case Op_IMUL32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i32 *= src.i32;
			break;
		}
		case Op_IMUL64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i64 *= src.i64;
			break;
		}
		case Op_IMMIMUL8:
		{
			auto& dst = _load_reg(self);
			auto imm = int8_t(pop8(self.bytecode, self.r[Reg_IP].u64));
			dst.i8 *= imm;
			break;
		}
		case Op_IMMIMUL16:
		{
			auto& dst = _load_reg(self);
			auto imm = int16_t(pop16(self.bytecode, self.r[Reg_IP].u64));
			dst.i16 *= imm;
			break;
		}
		case Op_IMMIMUL32:
		{
			auto& dst = _load_reg(self);
			auto imm = int32_t(pop32(self.bytecode, self.r[Reg_IP].u64));
			dst.i32 *= imm;
			break;
		}
		case Op_IMMIMUL64:
		{
			auto& dst = _load_reg(self);
			auto imm = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			dst.i64 *= imm;
			break;
		}
		case Op_DIV8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u8 /= src.u8;
			break;
		}
		case Op_DIV16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u16 /= src.u16;
			break;
		}
		case Op_DIV32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u32 /= src.u32;
			break;
		}
		case Op_DIV64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.u64 /= src.u64;
			break;
		}
		case Op_IMMDIV8:
		{
			auto& dst = _load_reg(self);
			auto imm = pop8(self.bytecode, self.r[Reg_IP].u64);
			dst.u8 /= imm;
			break;
		}
		case Op_IMMDIV16:
		{
			auto& dst = _load_reg(self);
			auto imm = pop16(self.bytecode, self.r[Reg_IP].u64);
			dst.u16 /= imm;
			break;
		}
		case Op_IMMDIV32:
		{
			auto& dst = _load_reg(self);
			auto imm = pop32(self.bytecode, self.r[Reg_IP].u64);
			dst.u32 /= imm;
			break;
		}
		case Op_IMMDIV64:
		{
			auto& dst = _load_reg(self);
			auto imm = pop64(self.bytecode, self.r[Reg_IP].u64);
			dst.u64 /= imm;
			break;
		}
		case Op_IDIV8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i8 /= src.i8;
			break;
		}
		case Op_IDIV16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i16 /= src.i16;
			break;
		}
		case Op_IDIV32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i32 /= src.i32;
			break;
		}
		case Op_IDIV64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			dst.i64 /= src.i64;
			break;
		}
		case Op_IMMIDIV8:
		{
			auto& dst = _load_reg(self);
			auto imm = int8_t(pop8(self.bytecode, self.r[Reg_IP].u64));
			dst.i8 /= imm;
			break;
		}
		case Op_IMMIDIV16:
		{
			auto& dst = _load_reg(self);
			auto imm = int16_t(pop16(self.bytecode, self.r[Reg_IP].u64));
			dst.i16 /= imm;
			break;
		}
		case Op_IMMIDIV32:
		{
			auto& dst = _load_reg(self);
			auto imm = int32_t(pop32(self.bytecode, self.r[Reg_IP].u64));
			dst.i32 /= imm;
			break;
		}
		case Op_IMMIDIV64:
		{
			auto& dst = _load_reg(self);
			auto imm = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			dst.i64 /= imm;
			break;
		}
		case Op_CMP8:
		{
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
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
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
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
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
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
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
			if (op1.u64 > op2.u64)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u64 < op2.u64)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMCMP8:
		{
			auto& op1 = _load_reg(self);
			auto imm = pop8(self.bytecode, self.r[Reg_IP].u64);
			if (op1.u8 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u8 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMCMP16:
		{
			auto& op1 = _load_reg(self);
			auto imm = pop16(self.bytecode, self.r[Reg_IP].u64);
			if (op1.u16 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u16 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMCMP32:
		{
			auto& op1 = _load_reg(self);
			auto imm = pop32(self.bytecode, self.r[Reg_IP].u64);
			if (op1.u32 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u32 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMCMP64:
		{
			auto& op1 = _load_reg(self);
			auto imm = pop64(self.bytecode, self.r[Reg_IP].u64);
			if (op1.u64 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.u64 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP8:
		{
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
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
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
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
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
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
			auto& op1 = _load_reg(self);
			auto& op2 = _load_reg(self);
			if (op1.i64 > op2.i64)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i64 < op2.i64)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMICMP8:
		{
			auto& op1 = _load_reg(self);
			auto imm = int8_t(pop8(self.bytecode, self.r[Reg_IP].u64));
			if (op1.i8 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i8 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMICMP16:
		{
			auto& op1 = _load_reg(self);
			auto imm = int16_t(pop16(self.bytecode, self.r[Reg_IP].u64));
			if (op1.i16 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i16 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMICMP32:
		{
			auto& op1 = _load_reg(self);
			auto imm = int32_t(pop32(self.bytecode, self.r[Reg_IP].u64));
			if (op1.i32 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i32 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_IMMICMP64:
		{
			auto& op1 = _load_reg(self);
			auto imm = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (op1.i64 > imm)
				self.cmp = Core::CMP_GREATER;
			else if (op1.i64 < imm)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_JMP:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			self.r[Reg_IP].u64 += offset;
			break;
		}
		case Op_JE:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JNE:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (self.cmp != Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JL:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_LESS)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JLE:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_LESS || self.cmp == Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JG:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_GREATER)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_JGE:
		{
			int64_t offset = int64_t(pop64(self.bytecode, self.r[Reg_IP].u64));
			if (self.cmp == Core::CMP_GREATER || self.cmp == Core::CMP_EQUAL)
			{
				self.r[Reg_IP].u64 += offset;
			}
			break;
		}
		case Op_READ8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, src.ptr, sizeof(uint8_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u8 = *(uint8_t*)src.ptr;
			break;
		}
		case Op_READ16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, src.ptr, sizeof(uint16_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u16 = *(uint16_t*)src.ptr;
			break;
		}
		case Op_READ32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, src.ptr, sizeof(uint32_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u32 = *(uint32_t*)src.ptr;
			break;
		}
		case Op_READ64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, src.ptr, sizeof(uint64_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u64 = *(uint64_t*)src.ptr;
			break;
		}
		case Op_WRITE8:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, dst.ptr, sizeof(uint8_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*(uint8_t*)dst.ptr = src.u8;
			break;
		}
		case Op_WRITE16:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, dst.ptr, sizeof(uint16_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*(uint16_t*)dst.ptr = src.u16;
			break;
		}
		case Op_WRITE32:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, dst.ptr, sizeof(uint32_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*(uint32_t*)dst.ptr = src.u32;
			break;
		}
		case Op_WRITE64:
		{
			auto& dst = _load_reg(self);
			auto& src = _load_reg(self);
			if(_valid_next_bytes(self, dst.ptr, sizeof(uint64_t)) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*(uint64_t*)dst.ptr = src.u64;
			break;
		}
		case Op_PUSH:
		{
			auto& dst = self.r[Reg_SP];
			auto& src = _load_reg(self);
			auto ptr = ((uint64_t*)dst.ptr - 1);
			if(_valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*ptr = src.u64;
			dst.ptr = ptr;
			break;
		}
		case Op_POP:
		{
			auto& dst = _load_reg(self);
			auto& src = self.r[Reg_SP];
			auto ptr = ((uint64_t*)src.ptr);
			if(_valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			dst.u64 = *ptr;
			src.ptr = ptr + 1;
			break;
		}
		case Op_CALL:
		{
			// load proc address
			auto address = pop64(self.bytecode, self.r[Reg_IP].u64);
			// load stack pointer
			auto& SP = self.r[Reg_SP];
			// allocate space for return address
			auto ptr = ((uint64_t*)SP.ptr - 1);
			if(_valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			// write the return address
			*ptr = self.r[Reg_IP].u64;
			// move the stack pointer
			SP.ptr = ptr;
			// jump to proc address
			self.r[Reg_IP].u64 = address;
			break;
		}
		case Op_RET:
		{
			// load stack pointer
			auto& SP = self.r[Reg_SP];
			auto ptr = ((uint64_t*)SP.ptr);
			if(_valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			// restore the IP
			self.r[Reg_IP].u64 = *ptr;
			// deallocate the space for return address
			SP.ptr = ptr;
			break;
		}
		case Op_DEBUGSTR:
		{
			auto& str_address = _load_reg(self);
			mn::print("DEBUG: '{}'", (const char*)str_address.ptr);
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