package zas

import "core:fmt"
import "core:strings"

@(private)
Parser :: struct {
	src: ^Src,
	tkns: [dynamic]Tkn,
	ix: int,
}

_parser_new :: proc(src: ^Src) -> Parser {
	self := Parser {
		src = src,
		tkns = make([dynamic]Tkn),
		ix = 0,
	};

	for tkn in src.tkns {
		if tkn.kind != .Comment {
			append(&self.tkns, tkn);
		}
	}

	return self;
}

_parser_free :: proc(self: ^Parser) {
	delete(self.tkns);
}

_parser_look_indexed :: proc(self: ^Parser, k: int) -> Tkn {
	if self.ix + k >= len(self.tkns) do return Tkn{};
	return self.tkns[self.ix + k];
}

_parser_look :: proc(self: ^Parser) -> Tkn do return _parser_look_indexed(self, 0);

_parser_look_kind :: proc(self: ^Parser, kind: Tkn_Kind) -> Tkn {
	if tkn := _parser_look(self); tkn.kind == kind do return tkn;
	return Tkn{};
}

_parser_eat :: proc(self: ^Parser) -> Tkn {
	if self.ix >= len(self.tkns) do return Tkn{};
	tkn := self.tkns[self.ix];
	self.ix += 1;
	return tkn;
}

_parser_eat_must :: proc(self: ^Parser, kind: Tkn_Kind) -> Tkn {
	if self.ix >= len(self.tkns) {
		append(&self.src.errs, Err{
			msg = fmt.aprintf("expected '%v' but found EOF", kind),
		});
		return Tkn{};
	}

	tkn := _parser_eat(self);
	if tkn.kind == kind do return tkn;

	append(&self.src.errs, Err{
		pos = tkn.pos,
		rng = tkn.rng,
		msg = fmt.aprintf("expected '%v' but found '%v'", kind, tkn.str),
	});
	return Tkn{};
}

_parser_reg :: proc(self: ^Parser) -> Tkn {
	tkn := _parser_eat(self);
	if tkn_kind_is_reg(tkn.kind) do return tkn;
	append(&self.src.errs, Err{
		pos = tkn.pos,
		rng = tkn.rng,
		msg = fmt.aprintf("expected a register but found '%v'", tkn.str),
	});
	return Tkn{};
}

_parser_imm :: proc(self: ^Parser, const_allowed: bool) -> Tkn {
	tkn := _parser_eat(self);
	if tkn.kind == .Const_Integer || tkn.kind == .Const_Float do return tkn;
	if const_allowed && tkn.kind == .ID do return tkn;
	append(&self.src.errs, Err{
		pos = tkn.pos,
		rng = tkn.rng,
		msg = fmt.aprintf("expected a constant but found '%v'", tkn.str),
	});
	return Tkn{};
}

_parser_ins :: proc (self: ^Parser) -> (ins: Ins) {
	op := _parser_look(self);
	if tkn_kind_is_load(op.kind) {
		ins.op = _parser_eat(self);
		ins.dst = _parser_reg(self);
		ins.src = _parser_imm(self, op.kind == .Keyword_U64_Load || op.kind == .Keyword_I64_Load);
	} else if tkn_kind_is_arithmetic(op.kind) {
		ins.op = _parser_eat(self);
		ins.dst = _parser_reg(self);
		src := _parser_look(self);
		if src.kind == .Const_Integer || tkn_kind_is_reg(src.kind){
			ins.src = _parser_eat(self);
		} else {
			append(&self.src.errs, Err{
				msg = fmt.aprintf("expected an integer or a register"),
			});
		}
	} else if tkn_kind_is_cond_jump(op.kind) {
		ins.op = _parser_eat(self);
		ins.dst = _parser_reg(self);
		src := _parser_look(self);
		if src.kind == .Const_Integer || tkn_kind_is_reg(src.kind){
			ins.src = _parser_eat(self);
		} else {
			append(&self.src.errs, Err{
				msg = fmt.aprintf("expected an integer or a register"),
			});
		}
		ins.lbl = _parser_eat_must(self, .ID);
	} else if tkn_kind_is_mem_transfer(op.kind) {
		ins.op = _parser_eat(self);
		ins.dst = _parser_reg(self);
		ins.src = _parser_reg(self);
	} else if tkn_kind_is_push_pop(op.kind) {
		ins.op = _parser_eat(self);
		ins.dst = _parser_reg(self);
	} else if tkn_kind_is_jump(op.kind) {
		ins.op = _parser_eat(self);
		ins.lbl = _parser_eat_must(self, .ID);
	} else if tkn_kind_is_cmp(op.kind) {
		ins.op = _parser_eat(self);
		ins.dst = _parser_reg(self);
		src := _parser_look(self);
		if src.kind == .Const_Integer || tkn_kind_is_reg(src.kind) {
			ins.src = _parser_eat(self);
		} else {
			append(&self.src.errs, Err{
				msg = fmt.aprintf("expected an integer or a register"),
			});
		}
	} else if op.kind == .Keyword_Call {
		ins.op = _parser_eat(self);
		ins.lbl = _parser_eat_must(self, .ID);
	} else if op.kind == .Keyword_Ret {
		ins.op = _parser_eat(self);
	} else if op.kind == .ID { // label
		ins.op = _parser_eat(self);
		_parser_eat_must(self, .Colon);
	} else if op.kind == .Keyword_Halt {
		ins.op = _parser_eat(self);
	}

	return;
}

_parser_proc :: proc(self: ^Parser) -> Proc {
	_parser_eat_must(self, .Keyword_Proc);
	p := proc_new();
	p.name = _parser_eat_must(self, .ID);

	for _parser_look_kind(self, .Keyword_End).kind == .None {
		if ins := _parser_ins(self); ins.op.kind != .None {
			append(&p.code, ins);
		} else {
			break;
		}
	}

	_parser_eat_must(self, .Keyword_End);

	return p;
}

_parser_c_proc :: proc(self: ^Parser) -> C_Proc {
	_parser_eat_must(self, .Keyword_Proc);
	p := c_proc_new();
	p.name = _parser_eat_must(self, .ID);

	_parser_eat_must(self, .Open_Paren);
	for _parser_look_kind(self, .Close_Paren).kind == .None {
		tkn := _parser_eat(self);
		if tkn_kind_is_ctype(tkn.kind) {
			append(&p.args, tkn);
		} else{
			append(&self.src.errs, Err{
				pos = tkn.pos,
				rng = tkn.rng,
				msg = fmt.aprintf("expected a type token"),
			});
		}
		if _parser_look_kind(self, .Comma).kind != .None do _parser_eat(self);
	}
	_parser_eat_must(self, .Close_Paren);

	tkn := _parser_eat(self);
	if tkn_kind_is_ctype(tkn.kind) {
		p.ret = tkn;
	} else {
		append(&self.src.errs, Err{
			pos = p.name.pos,
			rng = p.name.rng,
			msg = fmt.aprintf("expected a return type for C proc, but found '%v'", tkn.str),
		});
	}

	return p;
}

_parser_constant :: proc(self: ^Parser) -> Constant {
	_parser_eat_must(self, .Keyword_Constant);
	return Constant {
		name = _parser_eat_must(self, .ID),
		value = _parser_eat_must(self, .Const_String),
	};
}



src_parse :: proc(src: ^Src) -> bool {
	parser := _parser_new(src);
	defer _parser_free(&parser);

	for parser.ix < len(parser.tkns) {
		tkn := _parser_look(&parser);
		if tkn.kind == .Keyword_Proc {
			// check if the proc is C proc
			proc_name := _parser_look_indexed(&parser, 1).str;
			if strings.has_prefix(proc_name, "C.") {
				p := _parser_c_proc(&parser);
				if len(src.errs) > 0 {
					c_proc_free(&p);
					break;
				}
				append(&src.decls, p);
			} else {
				p := _parser_proc(&parser);
				if len(src.errs) > 0 {
					proc_free(&p);
					break;
				}
				append(&src.decls, p);
			}
		} else if tkn.kind == .Keyword_Constant {
			c := _parser_constant(&parser);
			if len(src.errs) > 0 do break;
			append(&src.decls, c);
		}
	}

	return len(src.errs) == 0;
}