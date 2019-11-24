#include "as/Gen.h"
#include "as/Src.h"

#include <vm/Util.h>
#include <vm/Op.h>
#include <vm/Reg.h>

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
	};

	inline static Emitter
	emitter_new(Src* src)
	{
		Emitter self{};
		self.src = src;
		self.fixups = mn::buf_new<Fixup_Request>();
		self.symbols = mn::map_new<const char*, uint64_t>();
		return self;
	}

	inline static void
	emitter_free(Emitter& self)
	{
		mn::buf_free(self.fixups);
		mn::map_free(self.symbols);
	}

	inline static void
	destruct(Emitter& self)
	{
		emitter_free(self);
	}

	inline static void
	emitter_label_fixup_request(Emitter& self, const Tkn& label)
	{
		mn::buf_push(self.fixups, Fixup_Request{ label, self.out.count });
		vm::push64(self.out, 0);
	}

	inline static void
	emitter_register_symbol(Emitter& self, const Tkn& label)
	{
		if (mn::map_lookup(self.symbols, label.str) == nullptr)
		{
			mn::map_insert(self.symbols, label.str, self.out.count);
		}
		else
		{
			src_err(self.src, label, mn::strf("'{}' symbol redefinition", label.str));
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

	inline static void
	emitter_ins_gen(Emitter& self, const Ins& ins, const Proc& proc, vm::Pkg& pkg)
	{
		switch(ins.op.kind)
		{
		case Tkn::KIND_KEYWORD_I8_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD8));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to int8_t
			int8_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push8(self.out, uint8_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U8_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD8));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to uint8_t
			uint8_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push8(self.out, c);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD16));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to int16_t
			int16_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push16(self.out, uint16_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U16_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD16));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to uint16_t
			uint16_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push16(self.out, c);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD32));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to int32_t
			int32_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push32(self.out, uint32_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U32_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD32));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to uint32_t
			uint32_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push32(self.out, c);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD64));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to int64_t
			int64_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push64(self.out, uint64_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U64_LOAD:
		{
			vm::push8(self.out, uint8_t(vm::Op_LOAD64));
			emitter_reg_gen(self, ins.dst);

			// convert the string value to uint64_t
			uint64_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push64(self.out, c);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_ADD:
		case Tkn::KIND_KEYWORD_U8_ADD:
			vm::push8(self.out, uint8_t(vm::Op_ADD8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I16_ADD:
		case Tkn::KIND_KEYWORD_U16_ADD:
			vm::push8(self.out, uint8_t(vm::Op_ADD16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I32_ADD:
		case Tkn::KIND_KEYWORD_U32_ADD:
			vm::push8(self.out, uint8_t(vm::Op_ADD32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I64_ADD:
		case Tkn::KIND_KEYWORD_U64_ADD:
			vm::push8(self.out, uint8_t(vm::Op_ADD64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I8_SUB:
		case Tkn::KIND_KEYWORD_U8_SUB:
			vm::push8(self.out, uint8_t(vm::Op_SUB8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I16_SUB:
		case Tkn::KIND_KEYWORD_U16_SUB:
			vm::push8(self.out, uint8_t(vm::Op_SUB16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I32_SUB:
		case Tkn::KIND_KEYWORD_U32_SUB:
			vm::push8(self.out, uint8_t(vm::Op_SUB32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I64_SUB:
		case Tkn::KIND_KEYWORD_U64_SUB:
			vm::push8(self.out, uint8_t(vm::Op_SUB64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I8_MUL:
			vm::push8(self.out, uint8_t(vm::Op_IMUL8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U8_MUL:
			vm::push8(self.out, uint8_t(vm::Op_MUL8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I16_MUL:
			vm::push8(self.out, uint8_t(vm::Op_IMUL16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U16_MUL:
			vm::push8(self.out, uint8_t(vm::Op_MUL16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I32_MUL:
			vm::push8(self.out, uint8_t(vm::Op_IMUL32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U32_MUL:
			vm::push8(self.out, uint8_t(vm::Op_MUL32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I64_MUL:
			vm::push8(self.out, uint8_t(vm::Op_IMUL64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U64_MUL:
			vm::push8(self.out, uint8_t(vm::Op_MUL64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I8_DIV:
			vm::push8(self.out, uint8_t(vm::Op_IDIV8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U8_DIV:
			vm::push8(self.out, uint8_t(vm::Op_DIV8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I16_DIV:
			vm::push8(self.out, uint8_t(vm::Op_IDIV16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U16_DIV:
			vm::push8(self.out, uint8_t(vm::Op_DIV16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I32_DIV:
			vm::push8(self.out, uint8_t(vm::Op_IDIV32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U32_DIV:
			vm::push8(self.out, uint8_t(vm::Op_DIV32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I64_DIV:
			vm::push8(self.out, uint8_t(vm::Op_IDIV64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_U64_DIV:
			vm::push8(self.out, uint8_t(vm::Op_DIV64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I8_JE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I16_JE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I32_JE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I64_JE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U8_JE:
			vm::push8(self.out, uint8_t(vm::Op_CMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U16_JE:
			vm::push8(self.out, uint8_t(vm::Op_CMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U32_JE:
			vm::push8(self.out, uint8_t(vm::Op_CMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U64_JE:
			vm::push8(self.out, uint8_t(vm::Op_CMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I8_JNE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I16_JNE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I32_JNE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I64_JNE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U8_JNE:
			vm::push8(self.out, uint8_t(vm::Op_CMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U16_JNE:
			vm::push8(self.out, uint8_t(vm::Op_CMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U32_JNE:
			vm::push8(self.out, uint8_t(vm::Op_CMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U64_JNE:
			vm::push8(self.out, uint8_t(vm::Op_CMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JNE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I8_JL:
			vm::push8(self.out, uint8_t(vm::Op_ICMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I16_JL:
			vm::push8(self.out, uint8_t(vm::Op_ICMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I32_JL:
			vm::push8(self.out, uint8_t(vm::Op_ICMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I64_JL:
			vm::push8(self.out, uint8_t(vm::Op_ICMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U8_JL:
			vm::push8(self.out, uint8_t(vm::Op_CMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U16_JL:
			vm::push8(self.out, uint8_t(vm::Op_CMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U32_JL:
			vm::push8(self.out, uint8_t(vm::Op_CMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U64_JL:
			vm::push8(self.out, uint8_t(vm::Op_CMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JL));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I8_JLE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I16_JLE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I32_JLE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I64_JLE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U8_JLE:
			vm::push8(self.out, uint8_t(vm::Op_CMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U16_JLE:
			vm::push8(self.out, uint8_t(vm::Op_CMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U32_JLE:
			vm::push8(self.out, uint8_t(vm::Op_CMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U64_JLE:
			vm::push8(self.out, uint8_t(vm::Op_CMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JLE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I8_JG:
			vm::push8(self.out, uint8_t(vm::Op_ICMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I16_JG:
			vm::push8(self.out, uint8_t(vm::Op_ICMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I32_JG:
			vm::push8(self.out, uint8_t(vm::Op_ICMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I64_JG:
			vm::push8(self.out, uint8_t(vm::Op_ICMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U8_JG:
			vm::push8(self.out, uint8_t(vm::Op_CMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U16_JG:
			vm::push8(self.out, uint8_t(vm::Op_CMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U32_JG:
			vm::push8(self.out, uint8_t(vm::Op_CMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U64_JG:
			vm::push8(self.out, uint8_t(vm::Op_CMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JG));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I8_JGE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I16_JGE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I32_JGE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I64_JGE:
			vm::push8(self.out, uint8_t(vm::Op_ICMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U8_JGE:
			vm::push8(self.out, uint8_t(vm::Op_CMP8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U16_JGE:
			vm::push8(self.out, uint8_t(vm::Op_CMP16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U32_JGE:
			vm::push8(self.out, uint8_t(vm::Op_CMP32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_U64_JGE:
			vm::push8(self.out, uint8_t(vm::Op_CMP64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			vm::push8(self.out, uint8_t(vm::Op_JGE));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_JMP:
			vm::push8(self.out, uint8_t(vm::Op_JMP));
			emitter_label_fixup_request(self, ins.lbl);
			break;

		case Tkn::KIND_KEYWORD_I8_READ:
		case Tkn::KIND_KEYWORD_U8_READ:
			vm::push8(self.out, uint8_t(vm::Op_READ8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I16_READ:
		case Tkn::KIND_KEYWORD_U16_READ:
			vm::push8(self.out, uint8_t(vm::Op_READ16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I32_READ:
		case Tkn::KIND_KEYWORD_U32_READ:
			vm::push8(self.out, uint8_t(vm::Op_READ32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I64_READ:
		case Tkn::KIND_KEYWORD_U64_READ:
			vm::push8(self.out, uint8_t(vm::Op_READ64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I8_WRITE:
		case Tkn::KIND_KEYWORD_U8_WRITE:
			vm::push8(self.out, uint8_t(vm::Op_WRITE8));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I16_WRITE:
		case Tkn::KIND_KEYWORD_U16_WRITE:
			vm::push8(self.out, uint8_t(vm::Op_WRITE16));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I32_WRITE:
		case Tkn::KIND_KEYWORD_U32_WRITE:
			vm::push8(self.out, uint8_t(vm::Op_WRITE32));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_I64_WRITE:
		case Tkn::KIND_KEYWORD_U64_WRITE:
			vm::push8(self.out, uint8_t(vm::Op_WRITE64));
			emitter_reg_gen(self, ins.dst);
			emitter_reg_gen(self, ins.src);
			break;

		case Tkn::KIND_KEYWORD_PUSH:
			vm::push8(self.out, uint8_t(vm::Op_PUSH));
			emitter_reg_gen(self, ins.dst);
			break;

		case Tkn::KIND_KEYWORD_POP:
			vm::push8(self.out, uint8_t(vm::Op_POP));
			emitter_reg_gen(self, ins.dst);
			break;

		case Tkn::KIND_KEYWORD_CALL:
			vm::push8(self.out, uint8_t(vm::Op_CALL));
			vm::pkg_reloc_add(pkg, mn::str_from_c(proc.name.str), self.out.count, mn::str_from_c(ins.lbl.str));
			vm::push64(self.out, 0);
			break;
		
		case Tkn::KIND_KEYWORD_RET:
			vm::push8(self.out, uint8_t(vm::Op_RET));
			break;

		case Tkn::KIND_KEYWORD_HALT:
			vm::push8(self.out, uint8_t(vm::Op_HALT));
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

	inline static mn::Buf<uint8_t>
	emitter_proc_gen(Emitter& self, const Proc& proc, vm::Pkg& pkg)
	{
		self.out = mn::buf_new<uint8_t>();

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

		auto res = self.out;
		self.out = {};
		return res;
	}

	// API
	vm::Pkg
	src_gen(Src* src)
	{
		auto pkg = vm::pkg_new();
		for(size_t i = 0; i < src->procs.count; ++i)
		{
			auto name = src->procs[i].name.str;

			auto emitter = emitter_new(src);
			mn_defer(emitter_free(emitter));

			auto code = emitter_proc_gen(emitter, src->procs[i], pkg);
			vm::pkg_proc_add(pkg, name, code);
		}
		return pkg;
	}
}
