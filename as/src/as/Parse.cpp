#include "as/Parse.h"

#include <mn/IO.h>
#include <mn/Defer.h>

namespace as
{
	struct Parser
	{
		Src* src;
		mn::Buf<Tkn> tkns;
		size_t ix;
	};

	inline static bool
	tkn_is_ignore(const Tkn& tkn)
	{
		return tkn.kind == Tkn::KIND_COMMENT;
	}

	inline static Parser
	parser_new(Src* src)
	{
		Parser self{};
		self.src = src;
		self.tkns = mn::buf_clone(src->tkns);
		self.ix = 0;
		// remove ignore tokens
		mn::buf_remove_if(self.tkns, [](const Tkn& tkn) { return tkn_is_ignore(tkn); });
		return self;
	}

	inline static void
	parser_free(Parser& self)
	{
		mn::buf_free(self.tkns);
	}

	inline static void
	destruct(Parser& self)
	{
		parser_free(self);
	}

	inline static Tkn
	parser_look(Parser* self, size_t k)
	{
		if(self->ix + k >= self->tkns.count)
			return Tkn{};
		return self->tkns[self->ix + k];
	}

	inline static Tkn
	parser_look(Parser *self)
	{
		return parser_look(self, 0);
	}

	inline static Tkn
	parser_look_kind(Parser* self, Tkn::KIND k)
	{
		Tkn t = parser_look(self);
		if(t.kind == k)
			return t;
		return Tkn{};
	}

	inline static Tkn
	parser_eat(Parser* self)
	{
		if(self->ix >= self->tkns.count)
			return Tkn{};
		return self->tkns[self->ix++];
	}

	inline static Tkn
	parser_eat_kind(Parser* self, Tkn::KIND kind)
	{
		Tkn t = parser_look(self);
		if(t.kind == kind)
			return parser_eat(self);
		return Tkn{};
	}

	inline static Tkn
	parser_eat_must(Parser* self, Tkn::KIND kind)
	{
		if(self->ix >= self->tkns.count)
		{
			src_err(
				self->src,
				mn::strf("expected '{}' but found EOF", Tkn::NAMES[kind])
			);
			return Tkn{};
		}

		Tkn tkn = parser_eat(self);
		if(tkn.kind == kind)
			return tkn;

		src_err(
			self->src,
			tkn,
			mn::strf("expected '{}' but found '{}'", Tkn::NAMES[kind], tkn.str)
		);
		return Tkn{};
	}

	inline static Tkn
	parser_reg(Parser* self)
	{
		auto op = parser_look(self);
		if (is_reg(op.kind))
		{
			return parser_eat(self);
		}
		
		src_err(self->src, op, mn::strf("expected a register but found '{}'", op.str));
		return Tkn{};
	}

	inline static Tkn
	parser_imm(Parser* self, bool constant_allowed)
	{
		auto op = parser_look(self);
		if (op.kind == Tkn::KIND_INTEGER ||
			op.kind == Tkn::KIND_FLOAT)
		{
			return parser_eat(self);
		}

		if (constant_allowed && op.kind == Tkn::KIND_ID)
			return parser_eat(self);

		src_err(self->src, op, mn::strf("expected a constant but found '{}'", op.str));
		return Tkn{};
	}

	enum OPERAND_FLAG
	{
		OPERAND_FLAG_REG = 1 << 0,
		OPERAND_FLAG_MEM = 1 << 1,
		OPERAND_FLAG_IMM = 1 << 2,
		OPERAND_FLAG_ID  = 1 << 3,
	};

	inline static Operand
	parser_operand(Parser* self, int operand_flag)
	{
		auto tkn = parser_look(self);

		if((operand_flag & OPERAND_FLAG_ID) && tkn.kind == Tkn::KIND_ID)
		{
			return operand_id(parser_eat(self));
		}

		if((operand_flag & OPERAND_FLAG_IMM) && is_numeric_constant(tkn.kind))
		{
			return operand_imm(parser_eat(self));
		}

		if((operand_flag & OPERAND_FLAG_MEM) && tkn.kind == Tkn::KIND_OPEN_BRACKET)
		{
			// eat the [
			parser_eat(self);

			Tkn base{}, index{}, shift{};
			base = parser_reg(self);

			if(parser_eat_kind(self, Tkn::KIND_OPEN_BRACKET))
			{
				index = parser_reg(self);
				parser_eat_must(self, Tkn::KIND_CLOSE_BRACKET);
			}

			if(parser_eat_kind(self, Tkn::KIND_PLUS))
			{
				shift  = parser_eat_must(self, Tkn::KIND_INTEGER);
			}

			// eat the ]
			parser_eat_must(self, Tkn::KIND_CLOSE_BRACKET);
			return operand_mem(base, index, shift);
		}

		if((operand_flag & OPERAND_FLAG_REG) && is_reg(tkn.kind))
		{
			return operand_reg(parser_eat(self));
		}

		auto msg = mn::strf("Expected");
		if(operand_flag & OPERAND_FLAG_REG)
			msg = mn::strf(msg, " register");
		if(operand_flag & OPERAND_FLAG_MEM)
			msg = mn::strf(msg, (operand_flag & OPERAND_FLAG_REG) ? ", memory" : " memory");
		if((operand_flag & OPERAND_FLAG_IMM) || (operand_flag & OPERAND_FLAG_ID))
			msg = mn::strf(msg, (operand_flag & OPERAND_FLAG_MEM) ? ", or immediate value" : " immediate value");
		msg = mn::strf(msg, " but found '{}'", tkn.str);

		src_err(self->src, parser_eat(self), msg);

		return Operand{};
	}

	inline static Ins
	parser_ins(Parser* self)
	{
		Ins ins{};

		Tkn op = parser_look(self);
		if (is_mov(op.kind))
		{
			ins.op = parser_eat(self);
			ins.dst = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM);
			auto op_flag = OPERAND_FLAG_REG | OPERAND_FLAG_MEM | OPERAND_FLAG_IMM;
			if(op.kind == Tkn::KIND_KEYWORD_I64_MOV || op.kind == Tkn::KIND_KEYWORD_U64_MOV)
				op_flag |= OPERAND_FLAG_ID;
			ins.src = parser_operand(self, op_flag);
		}
		else if (is_arithmetic(op.kind))
		{
			ins.op = parser_eat(self);
			ins.dst = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM);
			ins.src = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM | OPERAND_FLAG_IMM);
		}
		else if (is_cond_jump(op.kind))
		{
			ins.op = parser_eat(self);
			ins.dst = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM | OPERAND_FLAG_IMM);
			ins.src = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM | OPERAND_FLAG_IMM);
			ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
		}
		else if(is_push_pop(op.kind))
		{
			ins.op = parser_eat(self);
			ins.dst = parser_operand(self, OPERAND_FLAG_REG);
		}
		else if (is_pure_jump(op.kind))
		{
			ins.op = parser_eat(self);
			ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
		}
		else if(is_cmp(op.kind))
		{
			ins.op = parser_eat(self);
			ins.dst = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM | OPERAND_FLAG_IMM);
			ins.src = parser_operand(self, OPERAND_FLAG_REG | OPERAND_FLAG_MEM | OPERAND_FLAG_IMM);
		}
		else if (op.kind == Tkn::KIND_KEYWORD_CALL)
		{
			ins.op = parser_eat(self);
			ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
		}
		else if(op.kind == Tkn::KIND_KEYWORD_RET)
		{
			ins.op = parser_eat(self);
		}
		// label
		else if (op.kind == Tkn::KIND_ID)
		{
			ins.op = parser_eat(self);
			parser_eat_must(self, Tkn::KIND_COLON);
		}
		else if(op.kind == Tkn::KIND_KEYWORD_HALT)
		{
			ins.op = parser_eat(self);
		}

		return ins;
	}

	inline static Proc
	parser_proc(Parser* self)
	{
		parser_eat_must(self, Tkn::KIND_KEYWORD_PROC);
		auto proc = proc_new();
		proc.name = parser_eat_must(self, Tkn::KIND_ID);

		while (parser_look_kind(self, Tkn::KIND_KEYWORD_END) == false)
		{
			auto ins = parser_ins(self);
			if (ins.op)
				mn::buf_push(proc.ins, ins);
			else
				break;
		}

		parser_eat_must(self, Tkn::KIND_KEYWORD_END);

		return proc;
	}

	inline static C_Proc
	parser_c_proc(Parser* self)
	{
		parser_eat_must(self, Tkn::KIND_KEYWORD_PROC);
		auto proc = c_proc_new();
		proc.name = parser_eat_must(self, Tkn::KIND_ID);

		parser_eat_must(self, Tkn::KIND_OPEN_PAREN);
		while(parser_look_kind(self, Tkn::KIND_CLOSE_PAREN) == false)
		{
			auto tkn = parser_eat(self);
			if (is_ctype(tkn.kind))
				mn::buf_push(proc.args, tkn);
			else
				src_err(self->src, tkn, mn::strf("expected a type token"));
			parser_eat_kind(self, Tkn::KIND_COMMA);
		}
		parser_eat_must(self, Tkn::KIND_CLOSE_PAREN);

		auto tkn = parser_eat(self);
		if(is_ctype(tkn.kind))
			proc.ret = tkn;
		else
			src_err(self->src, proc.name, mn::strf("expected a return type for C proc, but found '{}'", tkn.str));

		return proc;
	}

	inline static Constant
	parser_constant(Parser* self)
	{
		parser_eat_must(self, Tkn::KIND_KEYWORD_CONSTANT);
		Constant constant{};
		constant.name = parser_eat_must(self, Tkn::KIND_ID);
		constant.value = parser_eat_must(self, Tkn::KIND_STRING);
		return constant;
	}

	inline static void
	proc_dump(Proc* proc, mn::Stream out)
	{
		mn::print_to(out, "PROC {}\n", proc->name.str);
		for(const auto& ins: proc->ins)
		{
			if(ins.op.kind == Tkn::KIND_ID)
				mn::print_to(out, "{}\n", ins);
			else
				mn::print_to(out, "  {}\n", ins);
		}
		mn::print_to(out, "END\n");
	}

	inline static void
	constant_dump(Constant *constant, mn::Stream out)
	{
		mn::print_to(out, "constant {} \"{}\"\n", constant->name.str, constant->value.str);
	}

	inline static void
	c_proc_dump(C_Proc* proc, mn::Stream out)
	{
		mn::print_to(out, "proc {}(", proc->name.str);
		for(size_t i = 0; i < proc->args.count; ++i)
		{
			if(i != 0)
				mn::print_to(out, ", ");
			mn::print_to(out, "{}", proc->args[i].str);
		}
		mn::print_to(out, ") {}\n", proc->ret.str);
	}

	// API
	bool
	parse(Src* src)
	{
		auto parser = parser_new(src);
		mn_defer(parser_free(parser));

		while(parser.ix < parser.tkns.count)
		{
			auto tkn = parser_look(&parser);
			if (tkn.kind == Tkn::KIND_KEYWORD_PROC)
			{
				// check if the proc is C proc
				auto proc_name = parser_look(&parser, 1);
				if(mn::str_prefix(proc_name.str, "C."))
				{
					auto c_proc = parser_c_proc(&parser);
					if(src_has_err(src))
					{
						c_proc_free(c_proc);
						break;
					}
					mn::buf_push(src->decls, decl_c_proc_new(c_proc));
				}
				else
				{
					auto proc = parser_proc(&parser);
					if (src_has_err(src))
					{
						proc_free(proc);
						break;
					}
					mn::buf_push(src->decls, decl_proc_new(proc));
				}
			}
			else if(tkn.kind == Tkn::KIND_KEYWORD_CONSTANT)
			{
				auto constant = parser_constant(&parser);
				if (src_has_err(src))
					break;
				mn::buf_push(src->decls, decl_constant_new(constant));
			}
		}

		return src_has_err(src) == false;
	}

	mn::Str
	decl_dump(Src* self, mn::Allocator allocator)
	{
		auto out = mn::memory_stream_new(allocator);
		mn_defer(mn::memory_stream_free(out));

		for(auto decl: self->decls)
		{
			switch(decl->kind)
			{
			case Decl::KIND_PROC:
				proc_dump(&decl->proc, out);
				break;

			case Decl::KIND_CONSTANT:
				constant_dump(&decl->constant, out);
				break;

			case Decl::KIND_C_PROC:
				c_proc_dump(&decl->c_proc, out);
				break;

			default:
				assert(false && "unreachable");
				break;
			}
		}

		return memory_stream_str(out);
	}
}