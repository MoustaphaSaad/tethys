package zas

Ins :: struct {
	op, dst, src, lbl: Tkn,
}


Proc :: struct {
	name: Tkn,
	code: [dynamic]Ins,
}

proc_new :: proc() -> Proc {
	return Proc {
		code = make([dynamic]Ins),
	};
}

proc_free :: proc(self: ^Proc) {
	delete(self.code);
}


Constant :: struct {
	name, value: Tkn,
}


C_Proc :: struct {
	name, ret: Tkn,
	args: [dynamic]Tkn,
}

c_proc_new :: proc() -> C_Proc {
	return C_Proc {
		args = make([dynamic]Tkn),
	};
}

c_proc_free :: proc(self: ^C_Proc) {
	delete(self.args);
}


Decl :: union {
	Proc,
	Constant,
	C_Proc
}

decl_free :: proc(self: ^Decl) {
	switch v in self {
	case Proc: proc_free(v);
	case Constant: // do nothing
	case C_Proc: c_proc_free(v);
	}
}