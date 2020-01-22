package zvm

Section_Type :: enum { Constant, Bytecode }
Section :: struct {
	type: Section_Type,
	data: [dynamic]byte,
}