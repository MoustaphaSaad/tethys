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
	lines: [dynamic]string,
	errs: [dynamic]Err,
	tkns: [dynamic]Tkn,
}

src_from_file :: proc(self: ^Src, path: string) -> bool {
	bytes, ok := os.read_entire_file(path);
	if ok == false do return false;

	self^ = Src {
		path = strings.clone(path),
		content = bytes,
		lines = make([dynamic]string),
		errs = make([dynamic]Err),
		tkns = make([dynamic]Tkn),
	};

	return true;
}

src_from_string :: proc(self: ^Src, code: string) {
	bytes := make([]byte, len(code));
	copy(bytes, code);

	self^ = Src {
		path = strings.clone("<STRING>"),
		content = bytes,
		lines = make([dynamic]string),
		errs = make([dynamic]Err),
		tkns = make([dynamic]Tkn),
	};
}

src_delete :: proc(self: ^Src) {
	delete(self.path);
	delete(self.content);
	delete(self.lines);
	for err in self.errs {
		delete(err.msg);
	}
	delete(self.errs);
	delete(self.tkns);
}

src_tkns_dump :: proc(self: ^Src) -> string {
	//this is a tmp stream you can use to construct strings into
	b := strings.make_builder();

	for tkn in self.tkns {
		fmt.sbprintf(&b, "line: %v, col: %v, kind: '%v', str: '%v'\n", tkn.pos.line, tkn.pos.col, tkn.kind, tkn.str);
	}
	return strings.to_string(b);
}