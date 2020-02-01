#include "as/Gen.h"
#include "as/Src.h"

#include <vm/Util.h>
#include <vm/Op.h>
#include <vm/Reg.h>
#include <vm/Asm.h>

#include <mn/IO.h>
#include <mn/Buf.h>
#include <mn/Map.h>
#include <mn/Defer.h>

#include <assert.h>

namespace as
{
	inline static void
	write64(uint8_t* ptr, uint64_t v)
	{
		ptr[0] = uint8_t(v);
		ptr[1] = uint8_t(v >> 8);
		ptr[2] = uint8_t(v >> 16);
		ptr[3] = uint8_t(v >> 24);
		ptr[4] = uint8_t(v >> 32);
		ptr[5] = uint8_t(v >> 40);
		ptr[6] = uint8_t(v >> 48);
		ptr[7] = uint8_t(v >> 56);
	}

	struct Fixup_Request
	{
		Tkn name;
		size_t bytecode_index;
	};

	struct Emitter
	{
		Src* src;
		mn::Buf<uint8_t> out;
		mn::Buf<Fixup_Request> fixups;
		mn::Map<const char*, size_t> symbols;
		// pointer to the globals symbols to check local symbols against
		mn::Map<const char*, Tkn> *globals;
	};

	inline static Emitter
	emitter_new(Src* src, mn::Map<const char*, Tkn> *globals)
	{
		Emitter self{};
		self.src = src;
		self.out = mn::buf_new<uint8_t>();
		self.fixups = mn::buf_new<Fixup_Request>();
		self.symbols = mn::map_new<const char*, uint64_t>();
		self.globals = globals;
		return self;
	}

	inline static void
	emitter_free(Emitter& self)
	{
		mn::buf_free(self.out);
		mn::buf_free(self.fixups);
		mn::map_free(self.symbols);
	}

	inline static void
	destruct(Emitter& self)
	{
		emitter_free(self);
	}

	inline static void
	emitter_register_symbol(Emitter& self, const Tkn& label)
	{
		// check global symbols
		assert(self.globals != nullptr);

		if(auto it = mn::map_lookup(*self.globals, label.str))
		{
			src_err(self.src, label, mn::strf("global symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
		}

		if (mn::map_lookup(self.symbols, label.str) == nullptr)
		{
			mn::map_insert(self.symbols, label.str, self.out.count);
		}
		else
		{
			src_err(self.src, label, mn::strf("'{}' local symbol redefinition", label.str));
		}
	}

	inline static vm::Reg
	tkn_to_reg(const Tkn& r)
	{
		switch(r.kind)
		{
		case Tkn::KIND_KEYWORD_R0: return vm::Reg_R0;
		case Tkn::KIND_KEYWORD_R1: return vm::Reg_R1;
		case Tkn::KIND_KEYWORD_R2: return vm::Reg_R2;
		case Tkn::KIND_KEYWORD_R3: return vm::Reg_R3;
		case Tkn::KIND_KEYWORD_R4: return vm::Reg_R4;
		case Tkn::KIND_KEYWORD_R5: return vm::Reg_R5;
		case Tkn::KIND_KEYWORD_R6: return vm::Reg_R6;
		case Tkn::KIND_KEYWORD_R7: return vm::Reg_R7;
		case Tkn::KIND_KEYWORD_IP: return vm::Reg_IP;
		case Tkn::KIND_KEYWORD_SP: return vm::Reg_SP;
		default:				   return vm::Reg_COUNT;
		}
	}

	inline static void
	emitter_reg_gen(Emitter& self, const Tkn& r)
	{
		switch(r.kind)
		{
		case Tkn::KIND_KEYWORD_R0:
			vm::push8(self.out, uint8_t(vm::Reg_R0));
			break;
		case Tkn::KIND_KEYWORD_R1:
			vm::push8(self.out, uint8_t(vm::Reg_R1));
			break;
		case Tkn::KIND_KEYWORD_R2:
			vm::push8(self.out, uint8_t(vm::Reg_R2));
			break;
		case Tkn::KIND_KEYWORD_R3:
			vm::push8(self.out, uint8_t(vm::Reg_R3));
			break;
		case Tkn::KIND_KEYWORD_R4:
			vm::push8(self.out, uint8_t(vm::Reg_R4));
			break;
		case Tkn::KIND_KEYWORD_R5:
			vm::push8(self.out, uint8_t(vm::Reg_R5));
			break;
		case Tkn::KIND_KEYWORD_R6:
			vm::push8(self.out, uint8_t(vm::Reg_R6));
			break;
		case Tkn::KIND_KEYWORD_R7:
			vm::push8(self.out, uint8_t(vm::Reg_R7));
			break;
		case Tkn::KIND_KEYWORD_IP:
			vm::push8(self.out, uint8_t(vm::Reg_IP));
			break;
		case Tkn::KIND_KEYWORD_SP:
			vm::push8(self.out, uint8_t(vm::Reg_SP));
			break;
		default:
			assert(false && "unreachable");
			break;
		}
	}

	template<typename T>
	inline static T
	convert_to(Tkn tkn)
	{
		// convert the string value to int8_t
		T c = 0;
		// reads returns the number of the parsed items
		[[maybe_unused]] size_t res = mn::reads(tkn.str, c);
		// assert that we parsed the only item we have
		assert(res == 1);
		return c;
	}

	inline static void
	emitter_ins_gen(Emitter& self, const Ins& ins, const Proc& proc, vm::Pkg& pkg)
	{
		switch(ins.op.kind)
		{
		case Tkn::KIND_KEYWORD_I8_LOAD:
		{
			if(ins.src.kind == Tkn::KIND_ID)
			{
				src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (8-bit) wide value"));
				break;
			}
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MOV8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U8_LOAD:
		{
			if(ins.src.kind == Tkn::KIND_ID)
			{
				src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (8-bit) wide value"));
				break;
			}
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MOV8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_LOAD:
		{
			if(ins.src.kind == Tkn::KIND_ID)
			{
				src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (16-bit) wide value"));
				break;
			}
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MOV16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U16_LOAD:
		{
			if(ins.src.kind == Tkn::KIND_ID)
			{
				src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (16-bit) wide value"));
				break;
			}
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MOV16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_LOAD:
		{
			if(ins.src.kind == Tkn::KIND_ID)
			{
				src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (32-bit) wide value"));
				break;
			}
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MOV32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U32_LOAD:
		{
			if(ins.src.kind == Tkn::KIND_ID)
			{
				src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (32-bit) wide value"));
				break;
			}
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MOV32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_LOAD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if(ins.src.kind == Tkn::KIND_ID)
			{
				vm::pkg_reloc_add(
					pkg,
					mn::str_lit(proc.name.str),
					self.out.count,
					mn::str_lit(ins.src.str)
				);
				src = vm::op_imm(int64_t(0));
			}
			else
			{
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			}
			vm::ins_push(self.out, vm::Op_MOV64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U64_LOAD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if(ins.src.kind == Tkn::KIND_ID)
			{
				vm::pkg_reloc_add(
					pkg,
					mn::str_lit(proc.name.str),
					self.out.count,
					mn::str_lit(ins.src.str)
				);
				src = vm::op_imm(uint64_t(0));
			}
			else
			{
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			}
			vm::ins_push(self.out, vm::Op_MOV64, dst, src);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U8_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U16_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U32_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U64_ADD:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ADD64, dst, src);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U8_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U16_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U32_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U64_SUB:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_SUB64, dst, src);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IMUL8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U8_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MUL8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IMUL16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U16_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MUL16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IMUL32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U32_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MUL32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IMUL64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U64_MUL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_MUL64, dst, src);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IDIV8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U8_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_DIV8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IDIV16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U16_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_DIV16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IDIV32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U32_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_DIV32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_IDIV64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_U64_DIV:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_DIV64, dst, src);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I16_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I32_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I64_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U8_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U16_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U32_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U64_JE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}


		case Tkn::KIND_KEYWORD_I8_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I16_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I32_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I64_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U8_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U16_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U32_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U64_JNE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}


		case Tkn::KIND_KEYWORD_I8_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I16_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I32_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I64_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U8_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U16_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U32_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U64_JL:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}


		case Tkn::KIND_KEYWORD_I8_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I16_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I32_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I64_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U8_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U16_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U32_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U64_JLE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}


		case Tkn::KIND_KEYWORD_I8_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I16_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I32_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I64_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U8_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U16_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U32_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U64_JG:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}


		case Tkn::KIND_KEYWORD_I8_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I16_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I32_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_I64_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U8_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U16_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U32_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}

		case Tkn::KIND_KEYWORD_U64_JGE:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);

			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;
		}


		case Tkn::KIND_KEYWORD_JMP:
			vm::ins_push(self.out, vm::Op_JMP, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_JE:
			vm::ins_push(self.out, vm::Op_JE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_JNE:
			vm::ins_push(self.out, vm::Op_JNE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_JL:
			vm::ins_push(self.out, vm::Op_JL, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_JLE:
			vm::ins_push(self.out, vm::Op_JLE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_JG:
			vm::ins_push(self.out, vm::Op_JG, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_JGE:
			vm::ins_push(self.out, vm::Op_JGE, vm::op_imm(uint64_t(0)), vm::op_none());
			mn::buf_push(self.fixups, Fixup_Request{ ins.lbl, self.out.count });
			break;

		case Tkn::KIND_KEYWORD_I8_READ:
		case Tkn::KIND_KEYWORD_U8_READ:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_mem(tkn_to_reg(ins.src), vm::SCALE_MODE_1X, 0);
			vm::ins_push(self.out, vm::Op_MOV8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_READ:
		case Tkn::KIND_KEYWORD_U16_READ:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_mem(tkn_to_reg(ins.src), vm::SCALE_MODE_1X, 0);
			vm::ins_push(self.out, vm::Op_MOV16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_READ:
		case Tkn::KIND_KEYWORD_U32_READ:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_mem(tkn_to_reg(ins.src), vm::SCALE_MODE_1X, 0);
			vm::ins_push(self.out, vm::Op_MOV32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_READ:
		case Tkn::KIND_KEYWORD_U64_READ:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::op_mem(tkn_to_reg(ins.src), vm::SCALE_MODE_1X, 0);
			vm::ins_push(self.out, vm::Op_MOV64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I8_WRITE:
		case Tkn::KIND_KEYWORD_U8_WRITE:
		{
			auto dst = vm::op_mem(tkn_to_reg(ins.dst), vm::SCALE_MODE_1X, 0);
			auto src = vm::op_reg(tkn_to_reg(ins.src));
			vm::ins_push(self.out, vm::Op_MOV8, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_WRITE:
		case Tkn::KIND_KEYWORD_U16_WRITE:
		{
			auto dst = vm::op_mem(tkn_to_reg(ins.dst), vm::SCALE_MODE_1X, 0);
			auto src = vm::op_reg(tkn_to_reg(ins.src));
			vm::ins_push(self.out, vm::Op_MOV16, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_WRITE:
		case Tkn::KIND_KEYWORD_U32_WRITE:
		{
			auto dst = vm::op_mem(tkn_to_reg(ins.dst), vm::SCALE_MODE_1X, 0);
			auto src = vm::op_reg(tkn_to_reg(ins.src));
			vm::ins_push(self.out, vm::Op_MOV32, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_WRITE:
		case Tkn::KIND_KEYWORD_U64_WRITE:
		{
			auto dst = vm::op_mem(tkn_to_reg(ins.dst), vm::SCALE_MODE_1X, 0);
			auto src = vm::op_reg(tkn_to_reg(ins.src));
			vm::ins_push(self.out, vm::Op_MOV64, dst, src);
			break;
		}

		case Tkn::KIND_KEYWORD_PUSH:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			vm::ins_push(self.out, vm::Op_PUSH, dst, vm::op_none());
			break;
		}

		case Tkn::KIND_KEYWORD_POP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			vm::ins_push(self.out, vm::Op_POP, dst, vm::op_none());
			break;
		}

		case Tkn::KIND_KEYWORD_CALL:
		{
			vm::pkg_reloc_add(pkg, mn::str_lit(proc.name.str), self.out.count, mn::str_lit(ins.lbl.str));

			if (mn::str_prefix(ins.lbl.str, "C."))
				vm::ins_push(self.out, vm::Op_C_CALL, vm::op_imm(uint64_t(0)), vm::op_none());
			else
				vm::ins_push(self.out, vm::Op_CALL, vm::op_imm(uint64_t(0)), vm::op_none());
			break;
		}
		
		case Tkn::KIND_KEYWORD_RET:
		{
			vm::ins_push(self.out, vm::Op_RET, vm::op_none(), vm::op_none());
			break;
		}

		case Tkn::KIND_KEYWORD_I8_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP8, dst, src);
		}

		case Tkn::KIND_KEYWORD_I16_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP16, dst, src);
		}

		case Tkn::KIND_KEYWORD_I32_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP32, dst, src);
		}

		case Tkn::KIND_KEYWORD_I64_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<int64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_ICMP64, dst, src);
		}

		case Tkn::KIND_KEYWORD_U8_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint8_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP8, dst, src);
		}

		case Tkn::KIND_KEYWORD_U16_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint16_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP16, dst, src);
		}

		case Tkn::KIND_KEYWORD_U32_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint32_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP32, dst, src);
		}

		case Tkn::KIND_KEYWORD_U64_CMP:
		{
			auto dst = vm::op_reg(tkn_to_reg(ins.dst));
			auto src = vm::Operand{};
			if (is_reg(ins.src.kind))
				src = vm::op_reg(tkn_to_reg(ins.src));
			else
				src = vm::op_imm(convert_to<uint64_t>(ins.src));
			vm::ins_push(self.out, vm::Op_CMP64, dst, src);
		}


		case Tkn::KIND_KEYWORD_HALT:
			vm::ins_push(self.out, vm::Op_HALT, vm::op_none(), vm::op_none());
			break;

		case Tkn::KIND_ID:
			emitter_register_symbol(self, ins.op);
			break;

		default:
			assert(false && "unreachable");
			vm::push8(self.out, uint8_t(vm::Op_IGL));
			break;
		}
	}

	inline static void
	emitter_proc_gen(Emitter& self, const Proc& proc, vm::Pkg& pkg)
	{
		// emit the proc bytecode
		for(const auto& ins: proc.ins)
			emitter_ins_gen(self, ins, proc, pkg);

		// do the fixups
		for(auto fixup: self.fixups)
		{
			auto it = mn::map_lookup(self.symbols, fixup.name.str);
			if(it == nullptr)
			{
				src_err(self.src, fixup.name, mn::strf("'{}' undefined symbol", fixup.name.str));
				continue;
			}

			int64_t offset = it->value - (fixup.bytecode_index + sizeof(int64_t));
			write64(self.out.ptr + fixup.bytecode_index, uint64_t(offset));
		}
	}

	inline static void
	_escape_string(mn::Str &str, const char* begin, const char* end)
	{
		assert(end >= begin);

		mn::str_reserve(str, end - begin);

		for(auto it = begin; it < end; ++it)
		{
			char current = *it;
			char next = 0;
			if (it + 1 < end)
				next = *(it + 1);

			if(current == '\\' && next == 'a')
			{
				mn::buf_push(str, '\a');
				++it;
				continue;
			}
			else if(current == '\\' && next == 'b')
			{
				mn::buf_push(str, '\b');
				++it;
				continue;
			}
			else if(current == '\\' && next == 'f')
			{
				mn::buf_push(str, '\f');
				++it;
				continue;
			}
			else if(current == '\\' && next == 'n')
			{
				mn::buf_push(str, '\n');
				++it;
				continue;
			}
			else if(current == '\\' && next == 'r')
			{
				mn::buf_push(str, '\r');
				++it;
				continue;
			}
			else if(current == '\\' && next == 't')
			{
				mn::buf_push(str, '\t');
				++it;
				continue;
			}
			else if(current == '\\' && next == 'v')
			{
				mn::buf_push(str, '\v');
				++it;
				continue;
			}
			else if(current == '\\' && next == '0')
			{
				mn::buf_push(str, '\0');
				++it;
				continue;
			}
			else if(current == '\\' && next == '\\')
			{
				mn::buf_push(str, '\\');
				++it;
				continue;
			}
			else
			{
				mn::buf_push(str, current);
				continue;
			}
		}

		mn::str_null_terminate(str);
	}

	inline static vm::C_TYPE
	tkn_to_ctype(const Tkn& tkn)
	{
		switch(tkn.kind)
		{
		case Tkn::KIND_KEYWORD_CINT8: return vm::C_TYPE_INT8;
		case Tkn::KIND_KEYWORD_CINT16: return vm::C_TYPE_INT16;
		case Tkn::KIND_KEYWORD_CINT32: return vm::C_TYPE_INT32;
		case Tkn::KIND_KEYWORD_CINT64: return vm::C_TYPE_INT64;
		case Tkn::KIND_KEYWORD_CUINT8: return vm::C_TYPE_UINT8;
		case Tkn::KIND_KEYWORD_CUINT16: return vm::C_TYPE_UINT16;
		case Tkn::KIND_KEYWORD_CUINT32: return vm::C_TYPE_UINT32;
		case Tkn::KIND_KEYWORD_CUINT64: return vm::C_TYPE_UINT64;
		case Tkn::KIND_KEYWORD_CFLOAT32: return vm::C_TYPE_FLOAT32;
		case Tkn::KIND_KEYWORD_CFLOAT64: return vm::C_TYPE_FLOAT64;
		case Tkn::KIND_KEYWORD_CPTR: return vm::C_TYPE_PTR;
		case Tkn::KIND_KEYWORD_VOID: return vm::C_TYPE_VOID;
		default: assert(false && "unreachable"); return vm::C_TYPE_VOID;
		}
	}

	inline static void
	_cproc_gen(C_Proc& self, Src* src, vm::Pkg *pkg)
	{
		auto parts = mn::str_split(self.name.str, ".", true);

		auto res = vm::c_proc_new();
		if(parts.count == 2)
		{
			res.lib = clone(parts[0]);
			res.name = clone(parts[1]);
		}
		else if(parts.count == 3)
		{
			res.lib = clone(parts[1]);
			res.name = clone(parts[2]);
		}
		else
		{
			src_err(src, self.name, mn::strf("unknown C proc name, name should be 'C.library_name.procedure_name'"));
			return;
		}

		mn::buf_reserve(res.arg_types, self.args.count);
		for(auto tkn: self.args)
			mn::buf_push(res.arg_types, tkn_to_ctype(tkn));

		res.ret = tkn_to_ctype(self.ret);

		mn::buf_push(pkg->c_procs, res);
	}

	// API
	vm::Pkg
	src_gen(Src* src)
	{
		// load all global symbols into globals map and try to resolve symbol redefinition erros
		auto globals = mn::map_new<const char*, Tkn>();
		mn_defer(mn::map_free(globals));

		for(auto decl: src->decls)
		{
			switch(decl->kind)
			{
			case Decl::KIND_PROC:
			{
				if(auto it = mn::map_lookup(globals, decl->proc.name.str))
					src_err(src, decl->proc.name, mn::strf("symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
				else
					mn::map_insert(globals, decl->proc.name.str, decl->proc.name);
				break;
			}

			case Decl::KIND_C_PROC:
			{
				if(auto it = mn::map_lookup(globals, decl->c_proc.name.str))
					src_err(src, decl->proc.name, mn::strf("symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
				else
					mn::map_insert(globals, decl->c_proc.name.str, decl->c_proc.name);
				break;
			}

			case Decl::KIND_CONSTANT:
			{
				if(auto it = mn::map_lookup(globals, decl->constant.name.str))
					src_err(src, decl->constant.name, mn::strf("symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
				else
					mn::map_insert(globals, decl->constant.name.str, decl->constant.name);
				break;
			}

			default:
				assert(false && "unreachable");
				break;
			}
		}

		auto pkg = vm::pkg_new();

		if (src_has_err(src))
			return pkg;

		auto tmp_str = mn::str_new();
		mn_defer(mn::str_free(tmp_str));

		for(auto decl: src->decls)
		{
			switch(decl->kind)
			{
			case Decl::KIND_PROC:
			{
				auto emitter = emitter_new(src, &globals);
				mn_defer(emitter_free(emitter));

				emitter_proc_gen(emitter, decl->proc, pkg);
				vm::pkg_proc_add(pkg, decl->proc.name.str, mn::block_from(emitter.out));
				break;
			}

			case Decl::KIND_C_PROC:
			{
				_cproc_gen(decl->c_proc, src, &pkg);
				break;
			}

			case Decl::KIND_CONSTANT:
			{
				mn::str_clear(tmp_str);
				_escape_string(tmp_str, decl->constant.value.rng.begin, decl->constant.value.rng.end);

				vm::pkg_constant_add(
					pkg,
					decl->constant.name.str,
					mn::block_from(tmp_str)
				);
				break;
			}

			default:
				assert(false && "unreachable");
				break;
			}
		}
		return pkg;
	}
}
