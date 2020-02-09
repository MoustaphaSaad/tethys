#include "vm/Core.h"
#include "vm/Op.h"
#include "vm/Util.h"

#include <mn/IO.h>
#include <mn/Defer.h>

#include <ffi.h>

namespace vm
{
	inline static Op
	pop_op(Core& self)
	{
		return Op(pop8(self.bytecode, self.r[Reg_IP].u64));
	}

	inline static Reg
	pop_reg(Core& self)
	{
		return Reg(pop8(self.bytecode, self.r[Reg_IP].u64));
	}

	inline static Ext
	pop_ext(Core& self)
	{
		return ext_from_byte(pop8(self.bytecode, self.r[Reg_IP].u64));
	}

	inline static Reg_Val&
	load_reg(Core& self)
	{
		Reg i = pop_reg(self);
		assert(i < Reg_COUNT);
		return self.r[i];
	}

	inline static bool
	valid_ptr(Core& self, void* ptr)
	{
		return ptr <= end(self.stack) && ptr >= begin(self.stack);
	}

	inline static bool
	valid_next_bytes(Core& self, void* ptr, size_t size)
	{
		return valid_ptr(self, ptr) && valid_ptr(self, (uint8_t*)ptr + size);
	}

	inline static ffi_type*
	ffi_type_from_c(C_TYPE t)
	{
		switch(t)
		{
		case C_TYPE_VOID: return &ffi_type_void;
		case C_TYPE_INT8: return &ffi_type_sint8;
		case C_TYPE_INT16: return &ffi_type_sint16;
		case C_TYPE_INT32: return &ffi_type_sint32;
		case C_TYPE_INT64: return &ffi_type_sint64;
		case C_TYPE_UINT8: return &ffi_type_uint8;
		case C_TYPE_UINT16: return &ffi_type_uint16;
		case C_TYPE_UINT32: return &ffi_type_uint32;
		case C_TYPE_UINT64: return &ffi_type_uint64;
		case C_TYPE_FLOAT32: return &ffi_type_float;
		case C_TYPE_FLOAT64 : return &ffi_type_double;
		case C_TYPE_PTR:  return &ffi_type_pointer;
		default: assert(false && "unreachable"); return &ffi_type_void;
		}
	}

	// API
	Core
	core_new()
	{
		Core self{};
		self.bytecode = mn::buf_new<uint8_t>();
		self.stack = mn::buf_new<uint8_t>();
		self.c_libraries = mn::buf_new<mn::Library>();
		self.c_procs_address = mn::buf_new<void*>();
		self.c_procs_desc = mn::buf_new<C_Proc>();
		return self;
	}

	void
	core_free(Core& self)
	{
		mn::buf_free(self.bytecode);
		mn::buf_free(self.stack);
		destruct(self.c_libraries);
		mn::buf_free(self.c_procs_address);
		destruct(self.c_procs_desc);
	}

	inline static uintptr_t
	load_operand_uintptr(Core& self, size_t imm_size)
	{
		auto ext = pop_ext(self);

		uintptr_t ptr = 0;
		switch(ext.address_mode)
		{
		case ADDRESS_MODE_REG:
		{
			auto R = Reg(pop8(self.bytecode, self.r[Reg_IP].u64));
			ptr = (uintptr_t)&self.r[R].u8;
			break;
		}
		case ADDRESS_MODE_MEM:
		{
			auto R = Reg(pop8(self.bytecode, self.r[Reg_IP].u64));
			ptr = uintptr_t(self.r[R].ptr);
			switch(ext.scale_mode)
			{
			case SCALE_MODE_1X: ptr *= 1; break;
			case SCALE_MODE_2X: ptr *= 2; break;
			case SCALE_MODE_4X: ptr *= 4; break;
			case SCALE_MODE_8X: ptr *= 8; break;
			default: assert(false && "unreachable"); break;
			}

			if(ext.is_shifted)
			{
				auto shift = pop64(self.bytecode, self.r[Reg_IP].u64);
				ptr += shift;
			}
			break;
		}
		case ADDRESS_MODE_IMM:
		{
			ptr = uintptr_t(self.bytecode.ptr);
			ptr += self.r[Reg_IP].u64;
			self.r[Reg_IP].u64 += imm_size;
			break;
		}
		default: assert(false && "unreachable"); break;
		}
		return ptr;
	}

	template<typename T>
	inline static T*
	load_operand(Core& self)
	{
		if constexpr (std::is_same_v<T, uint8_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, int8_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, uint16_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, int16_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, uint32_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, int32_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, uint64_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else if constexpr (std::is_same_v<T, int64_t>)
			return (T*)load_operand_uintptr(self, sizeof(T));
		else
			static_assert(sizeof(T) == 0, "unsupported operand type");
	}

	void
	core_ins_execute(Core& self)
	{
		Op op = pop_op(self);

		switch(op)
		{
		case Op_MOV8:
		{
			auto dst = load_operand<uint8_t>(self);
			auto src = load_operand<uint8_t>(self);
			*dst = *src;
			break;
		}
		case Op_MOV16:
		{
			auto dst = load_operand<uint16_t>(self);
			auto src = load_operand<uint16_t>(self);
			*dst = *src;
			break;
		}
		case Op_MOV32:
		{
			auto dst = load_operand<uint32_t>(self);
			auto src = load_operand<uint32_t>(self);
			*dst = *src;
			break;
		}
		case Op_MOV64:
		{
			auto dst = load_operand<uint64_t>(self);
			auto src = load_operand<uint64_t>(self);
			*dst = *src;
			break;
		}
		case Op_ADD8:
		{
			auto dst = load_operand<uint8_t>(self);
			auto src = load_operand<uint8_t>(self);
			*dst += *src;
			break;
		}
		case Op_ADD16:
		{
			auto dst = load_operand<uint16_t>(self);
			auto src = load_operand<uint16_t>(self);
			*dst += *src;
			break;
		}
		case Op_ADD32:
		{
			auto dst = load_operand<uint32_t>(self);
			auto src = load_operand<uint32_t>(self);
			*dst += *src;
			break;
		}
		case Op_ADD64:
		{
			auto dst = load_operand<uint64_t>(self);
			auto src = load_operand<uint64_t>(self);
			*dst += *src;
			break;
		}
		case Op_SUB8:
		{
			auto dst = load_operand<uint8_t>(self);
			auto src = load_operand<uint8_t>(self);
			*dst -= *src;
			break;
		}
		case Op_SUB16:
		{
			auto dst = load_operand<uint16_t>(self);
			auto src = load_operand<uint16_t>(self);
			*dst -= *src;
			break;
		}
		case Op_SUB32:
		{
			auto dst = load_operand<uint32_t>(self);
			auto src = load_operand<uint32_t>(self);
			*dst -= *src;
			break;
		}
		case Op_SUB64:
		{
			auto dst = load_operand<uint64_t>(self);
			auto src = load_operand<uint64_t>(self);
			*dst -= *src;
			break;
		}
		case Op_MUL8:
		{
			auto dst = load_operand<uint8_t>(self);
			auto src = load_operand<uint8_t>(self);
			*dst *= *src;
			break;
		}
		case Op_MUL16:
		{
			auto dst = load_operand<uint16_t>(self);
			auto src = load_operand<uint16_t>(self);
			*dst *= *src;
			break;
		}
		case Op_MUL32:
		{
			auto dst = load_operand<uint32_t>(self);
			auto src = load_operand<uint32_t>(self);
			*dst *= *src;
			break;
		}
		case Op_MUL64:
		{
			auto dst = load_operand<uint64_t>(self);
			auto src = load_operand<uint64_t>(self);
			*dst *= *src;
			break;
		}
		case Op_IMUL8:
		{
			auto dst = load_operand<int8_t>(self);
			auto src = load_operand<int8_t>(self);
			*dst *= *src;
			break;
		}
		case Op_IMUL16:
		{
			auto dst = load_operand<int16_t>(self);
			auto src = load_operand<int16_t>(self);
			*dst *= *src;
			break;
		}
		case Op_IMUL32:
		{
			auto dst = load_operand<int32_t>(self);
			auto src = load_operand<int32_t>(self);
			*dst *= *src;
			break;
		}
		case Op_IMUL64:
		{
			auto dst = load_operand<int64_t>(self);
			auto src = load_operand<int64_t>(self);
			*dst *= *src;
			break;
		}
		case Op_DIV8:
		{
			auto dst = load_operand<uint8_t>(self);
			auto src = load_operand<uint8_t>(self);
			*dst /= *src;
			break;
		}
		case Op_DIV16:
		{
			auto dst = load_operand<uint16_t>(self);
			auto src = load_operand<uint16_t>(self);
			*dst /= *src;
			break;
		}
		case Op_DIV32:
		{
			auto dst = load_operand<uint32_t>(self);
			auto src = load_operand<uint32_t>(self);
			*dst /= *src;
			break;
		}
		case Op_DIV64:
		{
			auto dst = load_operand<uint64_t>(self);
			auto src = load_operand<uint64_t>(self);
			*dst /= *src;
			break;
		}
		case Op_IDIV8:
		{
			auto dst = load_operand<int8_t>(self);
			auto src = load_operand<int8_t>(self);
			*dst /= *src;
			break;
		}
		case Op_IDIV16:
		{
			auto dst = load_operand<int16_t>(self);
			auto src = load_operand<int16_t>(self);
			*dst /= *src;
			break;
		}
		case Op_IDIV32:
		{
			auto dst = load_operand<int32_t>(self);
			auto src = load_operand<int32_t>(self);
			*dst /= *src;
			break;
		}
		case Op_IDIV64:
		{
			auto dst = load_operand<int64_t>(self);
			auto src = load_operand<int64_t>(self);
			*dst /= *src;
			break;
		}
		case Op_CMP8:
		{
			auto op1 = load_operand<uint8_t>(self);
			auto op2 = load_operand<uint8_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_CMP16:
		{
			auto op1 = load_operand<uint16_t>(self);
			auto op2 = load_operand<uint16_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_CMP32:
		{
			auto op1 = load_operand<uint32_t>(self);
			auto op2 = load_operand<uint32_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_CMP64:
		{
			auto op1 = load_operand<uint64_t>(self);
			auto op2 = load_operand<uint64_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP8:
		{
			auto op1 = load_operand<int8_t>(self);
			auto op2 = load_operand<int8_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP16:
		{
			auto op1 = load_operand<int16_t>(self);
			auto op2 = load_operand<int16_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP32:
		{
			auto op1 = load_operand<int32_t>(self);
			auto op2 = load_operand<int32_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_ICMP64:
		{
			auto op1 = load_operand<int64_t>(self);
			auto op2 = load_operand<int64_t>(self);
			if (*op1 > *op2)
				self.cmp = Core::CMP_GREATER;
			else if (*op1 < *op2)
				self.cmp = Core::CMP_LESS;
			else
				self.cmp = Core::CMP_EQUAL;
			break;
		}
		case Op_JMP:
		{
			auto offset = load_operand<uint64_t>(self);
			self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_JE:
		{
			auto offset = load_operand<uint64_t>(self);
			if (self.cmp == Core::CMP_EQUAL)
				self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_JNE:
		{
			auto offset = load_operand<uint64_t>(self);
			if (self.cmp != Core::CMP_EQUAL)
				self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_JL:
		{
			auto offset = load_operand<uint64_t>(self);
			if (self.cmp == Core::CMP_LESS)
				self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_JLE:
		{
			auto offset = load_operand<uint64_t>(self);
			if (self.cmp == Core::CMP_LESS || self.cmp == Core::CMP_EQUAL)
				self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_JG:
		{
			auto offset = load_operand<uint64_t>(self);
			if (self.cmp == Core::CMP_GREATER)
				self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_JGE:
		{
			auto offset = load_operand<uint64_t>(self);
			if (self.cmp == Core::CMP_GREATER || self.cmp == Core::CMP_EQUAL)
				self.r[Reg_IP].u64 += *offset;
			break;
		}
		case Op_PUSH:
		{
			auto& dst = self.r[Reg_SP];
			auto  src = load_operand<uint64_t>(self);
			auto ptr = ((uint64_t*)dst.ptr - 1);
			if(valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*ptr = *src;
			dst.ptr = ptr;
			break;
		}
		case Op_POP:
		{
			auto  dst = load_operand<uint64_t>(self);
			auto& src = self.r[Reg_SP];
			auto ptr = ((uint64_t*)src.ptr);
			if(valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			*dst = *ptr;
			src.ptr = ptr + 1;
			break;
		}
		case Op_CALL:
		{
			// load proc address
			auto  address = load_operand<uint64_t>(self);
			// load stack pointer
			auto& SP = self.r[Reg_SP];
			// allocate space for return address
			auto ptr = ((uint64_t*)SP.ptr - 1);
			if(valid_next_bytes(self, ptr, 8) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			// write the return address
			*ptr = self.r[Reg_IP].u64;
			// move the stack pointer
			SP.ptr = ptr;
			// jump to proc address
			self.r[Reg_IP].u64 = *address;
			break;
		}
		case Op_C_CALL:
		{
			// load c proc index
			auto ix = *load_operand<uint64_t>(self);
			if(ix >= self.c_procs_desc.count)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			auto& cproc = self.c_procs_desc[ix];
			auto cproc_ptr= self.c_procs_address[ix];
			
			ffi_cif cif;
			auto arg_types = mn::buf_with_count<ffi_type*>(cproc.arg_types.count);
			auto arg_values = mn::buf_with_count<void*>(cproc.arg_types.count);
			auto ret_type = ffi_type_from_c(cproc.ret);
			ffi_arg ret_value;
			mn_defer({
				mn::buf_free(arg_types);
				mn::buf_free(arg_values);
			});

			char* it = (char*)self.r[Reg_SP].ptr;

			// get return value address from the stack
			if(valid_next_bytes(self, it, ret_type->size) == false)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			it += ret_type->size;

			// get args from the stack
			for(size_t i = 0; i < cproc.arg_types.count; ++i)
			{
				auto ffi_arg_type = ffi_type_from_c(cproc.arg_types[i]);
				if(valid_next_bytes(self, it, ffi_arg_type->size) == false)
				{
					self.state = Core::STATE_ERR;
					break;
				}
				arg_types[i] = ffi_arg_type;
				arg_values[i] = it;
				it += ffi_arg_type->size;
			}

			// exit if there's an error
			if (self.state == Core::STATE_ERR)
				break;

			auto res = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, uint32_t(cproc.arg_types.count), ret_type, arg_types.ptr);
			if(res != FFI_OK)
			{
				self.state = Core::STATE_ERR;
				break;
			}
			ffi_call(&cif, FFI_FN(cproc_ptr), &ret_value, arg_values.ptr);
			// write c proc name for now
			mn::print("C CALL: {}.{} @ {}\n", cproc.lib, cproc.name, self.c_procs_address[ix]);
			break;
		}
		case Op_RET:
		{
			// load stack pointer
			auto& SP = self.r[Reg_SP];
			auto ptr = ((uint64_t*)SP.ptr);
			if(valid_next_bytes(self, ptr, 8) == false)
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