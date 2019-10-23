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

	inline static Parser
	parser_new(Src* src)
	{
		Parser self{};
		self.src = src;
		self.tkns = mn::buf_clone(src->tkns);
		self.ix = 0;
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
		if (op.kind == Tkn::KIND_KEYWORD_R0 ||
			op.kind == Tkn::KIND_KEYWORD_R1 ||
			op.kind == Tkn::KIND_KEYWORD_R2 ||
			op.kind == Tkn::KIND_KEYWORD_R3 ||
			op.kind == Tkn::KIND_KEYWORD_R4 ||
			op.kind == Tkn::KIND_KEYWORD_R5 ||
			op.kind == Tkn::KIND_KEYWORD_R6 ||
			op.kind == Tkn::KIND_KEYWORD_R7 ||
			op.kind == Tkn::KIND_KEYWORD_IP)
		{
			return parser_eat(self);
		}
		
		src_err(self->src, op, mn::strf("expected a register but found '{}'", op.str));
		return Tkn{};
	}

	inline static Tkn
	parser_const(Parser* self)
	{
		auto op = parser_look(self);
		if (op.kind == Tkn::KIND_INTEGER ||
			op.kind == Tkn::KIND_FLOAT)
		{
			return parser_eat(self);
		}
		
		src_err(self->src, op, mn::strf("expected a constant but found '{}'", op.str));
		return Tkn{};
	}

	inline static Ins
	parser_ins(Parser* self)
	{
		Ins ins{};

		Tkn op = parser_look(self);
		if (op.kind == Tkn::KIND_KEYWORD_I8_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_I16_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_I32_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_I64_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_U8_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_U16_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_U32_LOAD ||
			op.kind == Tkn::KIND_KEYWORD_U64_LOAD)
		{
			ins.op = parser_eat(self);
			ins.dst = parser_reg(self);
			ins.src = parser_const(self);
		}
		else if (op.kind == Tkn::KIND_KEYWORD_I8_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_I16_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_I32_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_I64_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_U8_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_U16_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_U32_ADD ||
				 op.kind == Tkn::KIND_KEYWORD_U64_ADD)
		{
			ins.op = parser_eat(self);
			ins.dst = parser_reg(self);
			ins.src = parser_reg(self);
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

		parser_eat_kind(self, Tkn::KIND_KEYWORD_END);

		return proc;
	}

	// API
	bool
	parse(Src* src)
	{
		auto parser = parser_new(src);
		mn_defer(parser_free(parser));

		while(parser.ix < parser.tkns.count)
		{
			auto proc = parser_proc(&parser);
			if (src_has_err(src)) break;
			mn::buf_push(src->procs, proc);
		}

		return src_has_err(src) == false;
	}

	mn::Str
	proc_dump(Src* self, mn::Allocator allocator)
	{
		auto out = mn::memory_stream_new(allocator);
		mn_defer(mn::memory_stream_free(out));

		for(const auto& proc: self->procs)
		{
			mn::print_to(out, "PROC {}\n", proc.name.str);
			for(const auto& ins: proc.ins)
			{
				mn::print_to(
					out,
					"{} {} {}\n",
					ins.op ? ins.op.str : "INVALID_OP",
					ins.dst ? ins.dst.str : "",
					ins.src ? ins.src.str : ""
				);
			}
			mn::print_to(out, "END\n");
		}

		return memory_stream_str(out);
	}
}