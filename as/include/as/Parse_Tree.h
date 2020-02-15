#pragma once

#include "as/Exports.h"
#include "as/Tkn.h"

#include <mn/Buf.h>
#include <mn/Fmt.h>

namespace as
{
	struct Operand
	{
		enum KIND
		{
			KIND_NONE,
			KIND_REG,
			KIND_MEM,
			KIND_IMM,
			KIND_ID
		};

		KIND kind;
		union
		{
			Tkn reg;
			struct
			{
				Tkn base;
				Tkn index;
				Tkn shift;
			} mem;
			Tkn imm;
			Tkn id;
		};
	};

	inline static Operand
	operand_reg(Tkn reg)
	{
		Operand self{};
		self.kind = Operand::KIND_REG;
		self.reg = reg;
		return self;
	}

	inline static Operand
	operand_mem(Tkn base, Tkn index, Tkn shift)
	{
		Operand self{};
		self.kind = Operand::KIND_MEM;
		self.mem.base = base;
		self.mem.index = index;
		self.mem.shift = shift;
		return self;
	}

	inline static Operand
	operand_imm(Tkn imm)
	{
		Operand self{};
		self.kind = Operand::KIND_IMM;
		self.imm = imm;
		return self;
	}

	inline static Operand
	operand_id(Tkn id)
	{
		Operand self{};
		self.kind = Operand::KIND_ID;
		self.id = id;
		return self;
	}

	struct Ins
	{
		Tkn op;  // operation
		Operand dst; // destination
		Operand src; // source
		Tkn lbl; // label
	};

	struct Proc
	{
		Tkn name;
		mn::Buf<Ins> ins;
	};

	inline static Proc
	proc_new()
	{
		Proc self{};
		self.ins = mn::buf_new<Ins>();
		return self;
	}

	inline static void
	proc_free(Proc& self)
	{
		mn::buf_free(self.ins);
	}

	inline static void
	destruct(Proc& self)
	{
		proc_free(self);
	}


	struct Constant
	{
		Tkn name;
		Tkn value;
	};


	struct C_Proc
	{
		Tkn name;
		mn::Buf<Tkn> args;
		Tkn ret;
	};

	inline static C_Proc
	c_proc_new()
	{
		C_Proc self{};
		self.args = mn::buf_new<Tkn>();
		return self;
	}

	inline static void
	c_proc_free(C_Proc& self)
	{
		mn::buf_free(self.args);
	}

	inline static void
	destruct(C_Proc& self)
	{
		c_proc_free(self);
	}


	struct Decl
	{
		enum KIND
		{
			KIND_NONE,
			KIND_PROC,
			KIND_CONSTANT,
			KIND_C_PROC,
		};

		KIND kind;
		union
		{
			Proc proc;
			Constant constant;
			C_Proc c_proc;
		};
	};

	AS_EXPORT Decl*
	decl_proc_new(Proc proc);

	AS_EXPORT Decl*
	decl_constant_new(Constant constant);

	AS_EXPORT Decl*
	decl_c_proc_new(C_Proc c_proc);

	AS_EXPORT void
	decl_free(Decl* self);

	inline static void
	destruct(Decl* self)
	{
		decl_free(self);
	}
}

namespace fmt
{
	template<>
	struct formatter<as::Tkn> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const as::Tkn &tkn, FormatContext &ctx) {
			if (tkn.kind == as::Tkn::KIND_NONE)
				return format_to(ctx.out(), "Tkn::KIND_NONE");
			return format_to(ctx.out(), "{}", tkn.str);
		}
	};

	template<>
	struct formatter<as::Operand> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const as::Operand &operand, FormatContext &ctx) {
			switch(operand.kind)
			{
			case as::Operand::KIND_REG:
				return format_to(ctx.out(), "{}", operand.reg);
			case as::Operand::KIND_MEM:
				format_to(ctx.out(), "[{}", operand.mem.base);
				if(operand.mem.index)
					format_to(ctx.out(), "[{}]", operand.mem.index);
				if(operand.mem.shift)
					format_to(ctx.out(), " + {}", operand.mem.shift);
				return format_to(ctx.out(), "]");
			case as::Operand::KIND_IMM:
				return format_to(ctx.out(), "{}", operand.imm);
			case as::Operand::KIND_ID:
				return format_to(ctx.out(), "{}", operand.id);
			case as::Operand::KIND_NONE:
				return format_to(ctx.out(), "Operand::KIND_NONE");
			default:
				assert(false && "unreachable");
				return ctx.out();
			}
		}
	};

	template<>
	struct formatter<as::Ins> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const as::Ins &ins, FormatContext &ctx) {
			format_to(ctx.out(), "{}", ins.op);
			if(ins.op.kind == as::Tkn::KIND_ID)
				format_to(ctx.out(), ":");

			if(ins.dst.kind != as::Operand::KIND_NONE)
				format_to(ctx.out(), " {}", ins.dst);
			if(ins.src.kind != as::Operand::KIND_NONE)
				format_to(ctx.out(), " {}", ins.src);
			if(ins.lbl)
				format_to(ctx.out(), " {}", ins.lbl);
			return ctx.out();
		}
	};
}