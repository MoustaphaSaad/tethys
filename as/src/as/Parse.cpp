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
	_tkn_is_ignore(const Tkn& tkn)
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
		mn::buf_remove_if(self.tkns, [](const Tkn& tkn) { return _tkn_is_ignore(tkn); });
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
	_parser_look(Parser* self, size_t k)
	{
		if(self->ix + k >= self->tkns.count)
			return Tkn{};
		return self->tkns[self->ix + k];
	}

	inline static Tkn
	_parser_look(Parser *self)
	{
		return _parser_look(self, 0);
	}

	inline static Tkn
	_parser_look_kind(Parser* self, Tkn::KIND k)
	{
		Tkn t = _parser_look(self);
		if(t.kind == k)
			return t;
		return Tkn{};
	}

	inline static Tkn
	_parser_eat(Parser* self)
	{
		if(self->ix >= self->tkns.count)
			return Tkn{};
		return self->tkns[self->ix++];
	}

	inline static Tkn
	_parser_eat_kind(Parser* self, Tkn::KIND kind)
	{
		Tkn t = _parser_look(self);
		if(t.kind == kind)
			return _parser_eat(self);
		return Tkn{};
	}

	inline static Tkn
	_parser_eat_must(Parser* self, Tkn::KIND kind)
	{
		if(self->ix >= self->tkns.count)
		{
			src_err(
				self->src,
				mn::strf("expected '{}' but found EOF", Tkn::NAMES[kind])
			);
			return Tkn{};
		}

		Tkn tkn = _parser_eat(self);
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
	_parser_reg(Parser* self)
	{
		auto op = _parser_look(self);
		if (is_reg(op))
		{
			return _parser_eat(self);
		}
		
		src_err(self->src, op, mn::strf("expected a register but found '{}'", op.str));
		return Tkn{};
	}

	inline static Tkn
	_parser_imm(Parser* self, bool constant_allowed)
	{
		auto op = _parser_look(self);
		if (op.kind == Tkn::KIND_INTEGER ||
			op.kind == Tkn::KIND_FLOAT)
		{
			return _parser_eat(self);
		}

		if (constant_allowed && op.kind == Tkn::KIND_ID)
			return _parser_eat(self);

		src_err(self->src, op, mn::strf("expected a constant but found '{}'", op.str));
		return Tkn{};
	}

	inline static bool
	_is_load(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_I8_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_LOAD ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_LOAD);
	}

	inline static bool
	_is_arithmetic(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_I8_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_ADD ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_SUB ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_MUL ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_DIV ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_DIV);
	}

	inline static bool
	_is_cond_jump(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_I8_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_JGE);
	}

	inline static bool
	_is_mem_transfer(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_I8_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_READ ||
				tkn.kind == Tkn::KIND_KEYWORD_I8_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_WRITE ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_WRITE);
	}

	inline static bool
	_is_push_pop(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_PUSH ||
				tkn.kind == Tkn::KIND_KEYWORD_POP);
	}

	inline static bool
	_is_cmp(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_I8_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_I16_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_I32_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_I64_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_U8_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_U16_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_U32_CMP ||
				tkn.kind == Tkn::KIND_KEYWORD_U64_CMP);
	}

	inline static bool
	_is_pure_jump(const Tkn& tkn)
	{
		return (tkn.kind == Tkn::KIND_KEYWORD_JE ||
				tkn.kind == Tkn::KIND_KEYWORD_JNE ||
				tkn.kind == Tkn::KIND_KEYWORD_JL ||
				tkn.kind == Tkn::KIND_KEYWORD_JLE ||
				tkn.kind == Tkn::KIND_KEYWORD_JG ||
				tkn.kind == Tkn::KIND_KEYWORD_JGE ||
				tkn.kind == Tkn::KIND_KEYWORD_JMP);
	}

	inline static Ins
	_parser_ins(Parser* self)
	{
		Ins ins{};

		Tkn op = _parser_look(self);
		if (_is_load(op))
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
			ins.src = _parser_imm(self, op.kind == Tkn::KIND_KEYWORD_U64_LOAD || op.kind == Tkn::KIND_KEYWORD_I64_LOAD);
		}
		else if (_is_arithmetic(op))
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
			auto src = _parser_look(self);
			if (src.kind == Tkn::KIND_INTEGER || is_reg(src))
				ins.src = _parser_eat(self);
			else
				src_err(self->src, src, mn::strf("expected an integer or a register"));
		}
		else if (_is_cond_jump(op))
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
			auto src = _parser_look(self);
			if(src.kind == Tkn::KIND_INTEGER || is_reg(src))
				ins.src = _parser_eat(self);
			else
				src_err(self->src, src, mn::strf("expected an integer or a register"));
			ins.lbl = _parser_eat_must(self, Tkn::KIND_ID);
		}
		else if(_is_mem_transfer(op))
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
			ins.src = _parser_reg(self);
		}
		else if(_is_push_pop(op))
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
		}
		else if (_is_pure_jump(op))
		{
			ins.op = _parser_eat(self);
			ins.lbl = _parser_eat_must(self, Tkn::KIND_ID);
		}
		else if(_is_cmp(op))
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
			auto src = _parser_look(self);
			if(src.kind == Tkn::KIND_INTEGER || is_reg(src))
				ins.src = _parser_eat(self);
			else
				src_err(self->src, src, mn::strf("expected an integer or a register"));
		}
		else if (op.kind == Tkn::KIND_KEYWORD_CALL)
		{
			ins.op = _parser_eat(self);
			ins.lbl = _parser_eat_must(self, Tkn::KIND_ID);
		}
		else if(op.kind == Tkn::KIND_KEYWORD_RET)
		{
			ins.op = _parser_eat(self);
		}
		// label
		else if (op.kind == Tkn::KIND_ID)
		{
			ins.op = _parser_eat(self);
			_parser_eat_must(self, Tkn::KIND_COLON);
		}
		else if(op.kind == Tkn::KIND_KEYWORD_HALT)
		{
			ins.op = _parser_eat(self);
		}
		else if(op.kind == Tkn::KIND_KEYWORD_DEBUGSTR)
		{
			ins.op = _parser_eat(self);
			ins.dst = _parser_reg(self);
		}

		return ins;
	}

	inline static Proc
	_parser_proc(Parser* self)
	{
		_parser_eat_must(self, Tkn::KIND_KEYWORD_PROC);
		auto proc = proc_new();
		proc.name = _parser_eat_must(self, Tkn::KIND_ID);

		while (_parser_look_kind(self, Tkn::KIND_KEYWORD_END) == false)
		{
			auto ins = _parser_ins(self);
			if (ins.op)
				mn::buf_push(proc.ins, ins);
			else
				break;
		}

		_parser_eat_kind(self, Tkn::KIND_KEYWORD_END);

		return proc;
	}

	inline static Constant
	_parser_constant(Parser* self)
	{
		_parser_eat_must(self, Tkn::KIND_KEYWORD_CONSTANT);
		Constant constant{};
		constant.name = _parser_eat_must(self, Tkn::KIND_ID);
		constant.value = _parser_eat_must(self, Tkn::KIND_STRING);
		return constant;
	}

	inline static void
	_proc_dump(Proc* proc, mn::Stream out)
	{
		mn::print_to(out, "PROC {}\n", proc->name.str);
		for(const auto& ins: proc->ins)
		{
			if (_is_load(ins.op) ||
				_is_arithmetic(ins.op))
			{
				mn::print_to(out, "  {} {} {}\n", ins.op.str, ins.dst.str, ins.src.str);
			}
			else if(_is_cond_jump(ins.op))
			{
				mn::print_to(out, "  {} {} {} {}\n", ins.op.str, ins.dst.str, ins.src.str, ins.lbl.str);
			}
			else if(_is_mem_transfer(ins.op))
			{
				mn::print_to(out, "  {} {} {}\n", ins.op.str, ins.dst.str, ins.src.str);
			}
			else if(_is_push_pop(ins.op))
			{
				mn::print_to(out, "  {} {}\n", ins.op.str, ins.dst.str);
			}
			else if(_is_pure_jump(ins.op))
			{
				mn::print_to(out, "  {} {}\n", ins.op.str, ins.lbl.str);
			}
			else if(_is_cmp(ins.op))
			{
				mn::print_to(out, "  {} {} {}\n", ins.op.str, ins.dst.str, ins.src.str);
			}
			else if(ins.op.kind == Tkn::KIND_KEYWORD_CALL)
			{
				mn::print_to(out, "  {} {}\n", ins.op.str, ins.lbl.str);
			}
			else if(ins.op.kind == Tkn::KIND_KEYWORD_RET)
			{
				mn::print_to(out, "  {}\n", ins.op.str);
			}
			else if(ins.op.kind == Tkn::KIND_ID)
			{
				mn::print_to(out, "{}:\n", ins.op.str);
			}
			else if(ins.op.kind == Tkn::KIND_KEYWORD_HALT)
			{
				mn::print_to(out, "  {}\n", ins.op.str);
			}
			else
			{
				mn::print_to(out, "  INVALID OP\n");
			}
		}
		mn::print_to(out, "END\n");
	}

	inline static void
	_constant_dump(Constant *constant, mn::Stream out)
	{
		mn::print_to(out, "constant {} \"{}\"\n", constant->name.str, constant->value.str);
	}

	// API
	bool
	parse(Src* src)
	{
		auto parser = parser_new(src);
		mn_defer(parser_free(parser));

		while(parser.ix < parser.tkns.count)
		{
			auto tkn = _parser_look(&parser);
			if (tkn.kind == Tkn::KIND_KEYWORD_PROC)
			{
				auto proc = _parser_proc(&parser);
				if (src_has_err(src))
				{
					proc_free(proc);
					break;
				}
				mn::buf_push(src->decls, decl_proc_new(proc));
			}
			else if(tkn.kind == Tkn::KIND_KEYWORD_CONSTANT)
			{
				auto constant = _parser_constant(&parser);
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
				_proc_dump(&decl->proc, out);
				break;

			case Decl::KIND_CONSTANT:
				_constant_dump(&decl->constant, out);
				break;

			default:
				assert(false && "unreachable");
				break;
			}
		}

		return memory_stream_str(out);
	}
}