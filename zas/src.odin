package zas

import "core:os"
import "core:mem"
import "core:strings"
import "core:fmt"

Pos :: struct {
	line, col: int,
}

Rng :: struct {
	begin, end: int,
}

Tkn :: struct {
	kind: Tkn_Kind,
	str: string,
	rng: Rng,
	pos: Pos,
}

Err :: struct {
	pos: Pos,
	rng: Rng,
	msg: string,
}

Src :: struct {
	path: string,
	content: []byte,
	lines: [dynamic]Rng,
	errs: [dynamic]Err,
	tkns: [dynamic]Tkn,
	decls: [dynamic]Decl,
}

src_from_file :: proc(path: string) -> (self: Src, ok: bool) {
	bytes, read_ok := os.read_entire_file(path);
	if read_ok == false {
		ok = false;
		return;
	}

	ok = true;
	self = Src {
		path = strings.clone(path),
		content = bytes,
		lines = make([dynamic]Rng),
		errs = make([dynamic]Err),
		tkns = make([dynamic]Tkn),
		decls = make([dynamic]Decl),
	};
	return;
}

src_from_string :: proc(code: string) -> Src {
	bytes := make([]byte, len(code));
	copy(bytes, code);

	return Src {
		path = strings.clone("<STRING>"),
		content = bytes,
		lines = make([dynamic]Rng),
		errs = make([dynamic]Err),
		tkns = make([dynamic]Tkn),
	};
}

src_delete :: proc(self: ^Src) {
	delete(self.path);
	delete(self.content);
	delete(self.lines);
	for err in self.errs do delete(err.msg);
	delete(self.errs);
	delete(self.tkns);
	for _, i in self.decls do decl_free(&self.decls[i]);
	delete(self.decls);
}

src_errs_dump :: proc(self: ^Src) -> string {
	b := strings.make_builder();

	for err in self.errs {
		line := self.lines[err.pos.line - 1];
		if line.begin != 0 && line.end != 0 {
			line_text := cast(string)self.content[line.begin:line.end];
			fmt.sbprintf(&b, ">>%v\n", line_text);
			fmt.sbprintf(&b, ">>");
			for i := line.begin; i < line.end; i += 1{
				if i >= err.rng.begin && i < err.rng.end {
					fmt.sbprintf(&b, "^");
				} else if self.content[i] == '\t' {
					fmt.sbprintf(&b, "\t");
				} else {
					fmt.sbprintf(&b, " ");
				}
			}
			fmt.sbprintf(&b, "\n");
		}
		fmt.sbprintf(&b, "Error[%v:%v:%v]: %v\n", self.path, err.pos.line, err.pos.col, err.msg);
	}
	return strings.to_string(b);
}

src_tkns_dump :: proc(self: ^Src) -> string {
	b := strings.make_builder();

	for tkn in self.tkns {
		fmt.sbprintf(&b, "line: %v, col: %v, kind: '%v', str: '%v'\n", tkn.pos.line, tkn.pos.col, tkn.kind, tkn.str);
	}
	return strings.to_string(b);
}

_proc_dump :: proc(self: Proc, b: ^strings.Builder) {
	fmt.sbprintf(b, "proc %v\n", self.name.str);
	for ins in self.code {
		if  tkn_kind_is_load(ins.op.kind) ||
		    tkn_kind_is_arithmetic(ins.op.kind) ||
		    tkn_kind_is_mem_transfer(ins.op.kind) ||
		    tkn_kind_is_cmp(ins.op.kind)
		{
			fmt.sbprintf(b, "  %v %v %v\n", ins.op.str, ins.dst.str, ins.src.str);
		} else if tkn_kind_is_cond_jump(ins.op.kind) {
			fmt.sbprintf(b, "  %v %v %v %v\n", ins.op.str, ins.dst.str, ins.src.str, ins.lbl.str);
		} else if tkn_kind_is_push_pop(ins.op.kind) {
			fmt.sbprintf(b, "  %v %v\n", ins.op.str, ins.dst.str);
		} else if tkn_kind_is_jump(ins.op.kind) || ins.op.kind == .Keyword_Call {
			fmt.sbprintf(b, "  %v %v\n", ins.op.str, ins.lbl.str);
		} else if ins.op.kind == .Keyword_Ret || ins.op.kind == .Keyword_Halt {
			fmt.sbprintf(b, "  %v\n", ins.op.str);
		} else if ins.op.kind == .ID {
			fmt.sbprintf(b, "%v:\n", ins.op.str);
		} else {
			fmt.sbprintf(b, "  %v:\n", ins.op.kind);
		}
	}
	fmt.sbprintf(b, "end\n");
}

_constant_dump :: proc(self: Constant, b: ^strings.Builder) {
	fmt.sbprintf(b, "constant %v \"%v\"\n", self.name.str, self.value.str);
}

_c_proc_dump :: proc(self: C_Proc, b: ^strings.Builder) {
	fmt.sbprintf(b, "proc %v(", self.name.str);
	for arg, i in self.args {
		if i != 0 do fmt.sbprintf(b, ", ");
		fmt.sbprintf(b, "%v", arg.str);
	}
	fmt.sbprintf(b, ") %v\n", self.ret.str);
}

src_decls_dump :: proc(self: ^Src) -> string {
	//this is a tmp stream you can use to construct strings into
	b := strings.make_builder();

	for decl in self.decls {
		switch v in decl {
		case Proc: _proc_dump(v, &b);
		case Constant: _constant_dump(v, &b);
		case C_Proc: _c_proc_dump(v, &b);
		}
	}
	return strings.to_string(b);
}