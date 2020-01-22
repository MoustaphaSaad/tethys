package zvm

import "core:strings"
import "core:runtime"

CType :: enum i32 {
	Void,
	Int8,
	Int16,
	Int32,
	Int64,
	Uint8,
	Uint16,
	Uint32,
	Uint64,
	Float32,
	Float64,
	Ptr,
}

CProc :: struct {
	lib, name: string,
	args: [dynamic]CType,
	ret: CType,
}

cproc_new :: proc() -> CProc {
	return CProc {
		args = make([dynamic]CType),
	};
}

cproc_free :: proc(self: ^CProc) {
	delete(self.lib);
	delete(self.name);
	delete(self.args);
}

cproc_clone :: proc(other: ^CProc) -> CProc {
	self := CProc {
		lib = strings.clone(other.lib),
		name = strings.clone(other.name),
		ret = other.ret
	};

	self.args = make([dynamic]CType, len(other.args));
	runtime.copy(self.args[:], other.args[:]);
	return self;
}