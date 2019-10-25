#include "as/Gen.h"

#include <vm/Util.h>
#include <vm/Op.h>
#include <vm/Reg.h>

#include <mn/IO.h>

#include <assert.h>

namespace as
{
	inline static void
	reg_gen(const Tkn& r, mn::Buf<uint8_t>& out)
	{
		switch(r.kind)
		{
		case Tkn::KIND_KEYWORD_R0:
			vm::push8(out, uint8_t(vm::Reg_R0));
			break;
		case Tkn::KIND_KEYWORD_R1:
			vm::push8(out, uint8_t(vm::Reg_R1));
			break;
		case Tkn::KIND_KEYWORD_R2:
			vm::push8(out, uint8_t(vm::Reg_R2));
			break;
		case Tkn::KIND_KEYWORD_R3:
			vm::push8(out, uint8_t(vm::Reg_R3));
			break;
		case Tkn::KIND_KEYWORD_R4:
			vm::push8(out, uint8_t(vm::Reg_R4));
			break;
		case Tkn::KIND_KEYWORD_R5:
			vm::push8(out, uint8_t(vm::Reg_R5));
			break;
		case Tkn::KIND_KEYWORD_R6:
			vm::push8(out, uint8_t(vm::Reg_R6));
			break;
		case Tkn::KIND_KEYWORD_R7:
			vm::push8(out, uint8_t(vm::Reg_R7));
			break;
		case Tkn::KIND_KEYWORD_IP:
			vm::push8(out, uint8_t(vm::Reg_IP));
			break;
		default:
			assert(false && "unreachable");
			break;
		}
	}

	inline static void
	ins_gen(const Ins& ins, mn::Buf<uint8_t>& out)
	{
		switch(ins.op.kind)
		{
		case Tkn::KIND_KEYWORD_I8_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD8));
			reg_gen(ins.dst, out);

			// convert the string value to int8_t
			int8_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push8(out, uint8_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U8_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD8));
			reg_gen(ins.dst, out);

			// convert the string value to uint8_t
			uint8_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push8(out, c);
			break;
		}

		case Tkn::KIND_KEYWORD_I16_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD16));
			reg_gen(ins.dst, out);

			// convert the string value to int16_t
			int16_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push16(out, uint16_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U16_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD16));
			reg_gen(ins.dst, out);

			// convert the string value to uint16_t
			uint16_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push16(out, c);
			break;
		}

		case Tkn::KIND_KEYWORD_I32_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD32));
			reg_gen(ins.dst, out);

			// convert the string value to int32_t
			int32_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push32(out, uint32_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U32_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD32));
			reg_gen(ins.dst, out);

			// convert the string value to uint32_t
			uint32_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push32(out, c);
			break;
		}

		case Tkn::KIND_KEYWORD_I64_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD64));
			reg_gen(ins.dst, out);

			// convert the string value to int64_t
			int64_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push64(out, uint64_t(c));
			break;
		}

		case Tkn::KIND_KEYWORD_U64_LOAD:
		{
			vm::push8(out, uint8_t(vm::Op_LOAD64));
			reg_gen(ins.dst, out);

			// convert the string value to uint64_t
			uint64_t c = 0;
			// reads returns the number of the parsed items
			size_t res = mn::reads(ins.src.str, c);
			// assert that we parsed the only item we have
			assert(res == 1);
			vm::push64(out, c);
			break;
		}


		case Tkn::KIND_KEYWORD_I8_ADD:
		case Tkn::KIND_KEYWORD_U8_ADD:
			vm::push8(out, uint8_t(vm::Op_ADD8));
			reg_gen(ins.dst, out);
			reg_gen(ins.src, out);
			break;

		case Tkn::KIND_KEYWORD_I16_ADD:
		case Tkn::KIND_KEYWORD_U16_ADD:
			vm::push8(out, uint8_t(vm::Op_ADD16));
			reg_gen(ins.dst, out);
			reg_gen(ins.src, out);
			break;

		case Tkn::KIND_KEYWORD_I32_ADD:
		case Tkn::KIND_KEYWORD_U32_ADD:
			vm::push8(out, uint8_t(vm::Op_ADD32));
			reg_gen(ins.dst, out);
			reg_gen(ins.src, out);
			break;

		case Tkn::KIND_KEYWORD_I64_ADD:
		case Tkn::KIND_KEYWORD_U64_ADD:
			vm::push8(out, uint8_t(vm::Op_ADD64));
			reg_gen(ins.dst, out);
			reg_gen(ins.src, out);
			break;

		case Tkn::KIND_KEYWORD_HALT:
			vm::push8(out, uint8_t(vm::Op_HALT));
			break;

		default:
			assert(false && "unreachable");
			vm::push8(out, uint8_t(vm::Op_IGL));
			break;
		}
	}

	// API
	mn::Buf<uint8_t>
	proc_gen(const Proc& proc, mn::Allocator allocator)
	{
		auto out = mn::buf_with_allocator<uint8_t>(allocator);

		for(const auto& ins: proc.ins)
			ins_gen(ins, out);

		return out;
	}
}
