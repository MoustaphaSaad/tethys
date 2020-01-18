package zas

import "core:os"
import "core:mem"
import "core:strings"

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
	bytes, ok := os.read_entire_file(self.path);
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
	mem.copy(&bytes[0], strings.ptr_from_string(code), len(code));

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