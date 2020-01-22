package zvm

import "core:mem"

Op :: enum u8 {
	// illegal opcode
	Igl,

	// Load [dst] [constant]
	Load8,
	Load16,
	Load32,
	Load64,

	// Add [dst + op1] [op2]
	Add8,
	Add16,
	Add32,
	Add64,

	// ImmAdd [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmAdd8,
	ImmAdd16,
	ImmAdd32,
	ImmAdd64,

	// Sub [dst + op1] [op2]
	Sub8,
	Sub16,
	Sub32,
	Sub64,

	// ImmSub [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmSub8,
	ImmSub16,
	ImmSub32,
	ImmSub64,

	// Mul [dst + op1] [op2]
	Mul8,
	Mul16,
	Mul32,
	Mul64,

	// ImmMul [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmMul8,
	ImmMul16,
	ImmMul32,
	ImmMul64,

	// IMul [dst + op1] [op2]
	IMul8,
	IMul16,
	IMul32,
	IMul64,

	// ImmIMul [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmIMul8,
	ImmIMul16,
	ImmIMul32,
	ImmIMul64,

	// Div [dst + op1] [op2]
	Div8,
	Div16,
	Div32,
	Div64,

	// ImmDiv [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmDiv8,
	ImmDiv16,
	ImmDiv32,
	ImmDiv64,

	// IDiv [dst + op1] [op2]
	IDiv8,
	IDiv16,
	IDiv32,
	IDiv64,

	// ImmIDiv [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmIDiv8,
	ImmIDiv16,
	ImmIDiv32,
	ImmIDiv64,

	// unsigned compare
	// Cmp [op1] [op2]
	Cmp8,
	Cmp16,
	Cmp32,
	Cmp64,

	// ImmCmp [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmCmp8,
	ImmCmp16,
	ImmCmp32,
	ImmCmp64,

	// signed compare
	// ICmp [op1] [op2]
	ICmp8,
	ICmp16,
	ICmp32,
	ICmp64,

	// ImmICmp [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
	ImmICmp8,
	ImmICmp16,
	ImmICmp32,
	ImmICmp64,

	// jump unconditionall
	// Jmp [offset 64-bit]
	Jmp,

	// jump if equal
	// Je [offset 64-bit]
	Je,

	// jump if not equal
	// Jne [offset 64-bit]
	Jne,

	// jump if less than
	// Jl [offset 64-bit]
	Jl,

	// jump if less than or equal
	// Jle [offset 64-bit]
	Jle,

	// jump if greater than
	// Jg [offset 64-bit]
	Jg,

	// jump if greater than or equal
	// Jge [offset 64-bit]
	Jge,

	// reads data from the src address register into the specified dst register
	// Read [dst] [src]
	Read8,
	Read16,
	Read32,
	Read64,

	// writes data from the src register into the specified dst address register
	// Write [dst] [src]
	Write8,
	Write16,
	Write32,
	Write64,

	// pushes the register into the stack and increment it
	// Push [register]
	Push,

	// pops the register into the stack and decrement it
	// Pop [register]
	Pop,

	// performs a call instruction
	// Call [address unsigned 64-bit]
	Call,

	// returns from proc calls
	// Ret
	Ret,

	// calls a C function
	// CCal [unsigned 64-bit index into c_proc array in core]
	CCall,

	Halt,
}

Reg :: enum u8 {
	// General purpose registers
	R0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,

	// instruction pointer
	IP,
	// stack pointer
	SP,
}

Reg_Value :: struct #raw_union {
	sint8:  i8,
	sint16: i16,
	sint32: i32,
	sint64: i64,
	uint8:  u8,
	uint16: u16,
	uint32: u32,
	uint64: u64,
	ptr:	uintptr,
}

pop8 :: proc(bytes: [dynamic]byte, ix: ^u64) -> u8 {
	r := (transmute(^u8)&bytes[ix^])^;
	ix^ += size_of(r);
	return r;
}

pop16 :: proc(bytes: [dynamic]byte, ix: ^u64) -> u16 {
	r := (transmute(^u16)&bytes[ix^])^;
	ix^ += size_of(r);
	return r;
}

pop32 :: proc(bytes: [dynamic]byte, ix: ^u64) -> u32 {
	r := (transmute(^u32)&bytes[ix^])^;
	ix^ += size_of(r);
	return r;
}

pop64 :: proc(bytes: [dynamic]byte, ix: ^u64) -> u64 {
	r := (transmute(^u64)&bytes[ix^])^;
	ix^ += size_of(r);
	return r;
}

push8 :: proc(bytes: ^[dynamic]byte, v: u8) {
	append(bytes, byte(v));
}

push16 :: proc(bytes: ^[dynamic]byte, v: u16) {
	append(bytes, byte(v));
	append(bytes, byte(v >> 8));
}

push32 :: proc(bytes: ^[dynamic]byte, v: u32) {
	append(bytes, byte(v));
	append(bytes, byte(v >> 8));
	append(bytes, byte(v >> 16));
	append(bytes, byte(v >> 24));
}

push64 :: proc(bytes: ^[dynamic]byte, v: u64) {
	append(bytes, byte(v));
	append(bytes, byte(v >> 8));
	append(bytes, byte(v >> 16));
	append(bytes, byte(v >> 24));
	append(bytes, byte(v >> 32));
	append(bytes, byte(v >> 40));
	append(bytes, byte(v >> 48));
	append(bytes, byte(v >> 56));
}

Core_State :: enum { Ok, Halt, Err }

Core_Cmp :: enum { None, Less, Equal, Greater }

Core :: struct {
	state: Core_State,
	cmp: Core_Cmp,
	r: [Reg]Reg_Value,
	bytecode: [dynamic]byte,
	stack: [dynamic]byte,
}

core_new :: proc() -> Core {
	return Core {
		bytecode = make([dynamic]byte),
		stack = make([dynamic]byte),
	};
}

core_free :: proc(self: ^Core) {
	delete(self.bytecode);
	delete(self.stack);
}

_pop_op :: proc(self: ^Core) -> Op {
	return transmute(Op)pop8(self.bytecode, &self.r[.IP].uint64);
}

_pop_reg :: proc(self: ^Core) -> Reg {
	return transmute(Reg)pop8(self.bytecode, &self.r[.IP].uint64);
}

_load_reg :: proc(self: ^Core) -> ^Reg_Value {
	return &self.r[_pop_reg(self)];
}

_valid_stack_ptr :: proc(self: ^Core, ptr: uintptr) -> bool {
	return ptr >= cast(uintptr)&self.stack[0] && ptr <= cast(uintptr)&self.stack[len(self.stack)];
}

_valid_stack_next_bytes :: proc(self: ^Core, ptr: uintptr, size: uint) -> bool {
	return _valid_stack_ptr(self, ptr) && _valid_stack_ptr(self, ptr + cast(uintptr)size);
}

core_ins_execute :: proc(self: ^Core) {
	op := _pop_op(self);
	switch op {
	case .Load8:
		dst := _load_reg(self);
		dst.uint8 = pop8(self.bytecode, &self.r[.IP].uint64);
	case .Load16:
		dst := _load_reg(self);
		dst.uint16 = pop16(self.bytecode, &self.r[.IP].uint64);
	case .Load32:
		dst := _load_reg(self);
		dst.uint32 = pop32(self.bytecode, &self.r[.IP].uint64);
	case .Load64:
		dst := _load_reg(self);
		dst.uint64 = pop64(self.bytecode, &self.r[.IP].uint64);
	case .Add8:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint8 += src.uint8;
	case .Add16:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint16 += src.uint16;
	case .Add32:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint32 += src.uint32;
	case .Add64:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint64 += src.uint64;
	case .ImmAdd8:
		dst := _load_reg(self);
		imm := pop8(self.bytecode, &self.r[.IP].uint64);
		dst.uint8 += imm;
	case .ImmAdd16:
		dst := _load_reg(self);
		imm := pop16(self.bytecode, &self.r[.IP].uint64);
		dst.uint16 += imm;
	case .ImmAdd32:
		dst := _load_reg(self);
		imm := pop32(self.bytecode, &self.r[.IP].uint64);
		dst.uint32 += imm;
	case .ImmAdd64:
		dst := _load_reg(self);
		imm := pop64(self.bytecode, &self.r[.IP].uint64);
		dst.uint64 += imm;
	case .Sub8:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint8 -= src.uint8;
	case .Sub16:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint16 -= src.uint16;
	case .Sub32:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint32 -= src.uint32;
	case .Sub64:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint64 -= src.uint64;
	case .ImmSub8:
		dst := _load_reg(self);
		imm := pop8(self.bytecode, &self.r[.IP].uint64);
		dst.uint8 -= imm;
	case .ImmSub16:
		dst := _load_reg(self);
		imm := pop16(self.bytecode, &self.r[.IP].uint64);
		dst.uint16 -= imm;
	case .ImmSub32:
		dst := _load_reg(self);
		imm := pop32(self.bytecode, &self.r[.IP].uint64);
		dst.uint32 -= imm;
	case .ImmSub64:
		dst := _load_reg(self);
		imm := pop64(self.bytecode, &self.r[.IP].uint64);
		dst.uint64 -= imm;
	case .Mul8:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint8 *= src.uint8;
	case .Mul16:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint16 *= src.uint16;
	case .Mul32:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint32 *= src.uint32;
	case .Mul64:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint64 *= src.uint64;
	case .ImmMul8:
		dst := _load_reg(self);
		imm := pop8(self.bytecode, &self.r[.IP].uint64);
		dst.uint8 *= imm;
	case .ImmMul16:
		dst := _load_reg(self);
		imm := pop16(self.bytecode, &self.r[.IP].uint64);
		dst.uint16 *= imm;
	case .ImmMul32:
		dst := _load_reg(self);
		imm := pop32(self.bytecode, &self.r[.IP].uint64);
		dst.uint32 *= imm;
	case .ImmMul64:
		dst := _load_reg(self);
		imm := pop64(self.bytecode, &self.r[.IP].uint64);
		dst.uint64 *= imm;
	case .IMul8:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint8 *= src.sint8;
	case .IMul16:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint16 *= src.sint16;
	case .IMul32:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint32 *= src.sint32;
	case .IMul64:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint64 *= src.sint64;
	case .ImmIMul8:
		dst := _load_reg(self);
		imm := transmute(i8)pop8(self.bytecode, &self.r[.IP].uint64);
		dst.sint8 *= imm;
	case .ImmIMul16:
		dst := _load_reg(self);
		imm := transmute(i16)pop16(self.bytecode, &self.r[.IP].uint64);
		dst.sint16 *= imm;
	case .ImmIMul32:
		dst := _load_reg(self);
		imm := transmute(i32)pop32(self.bytecode, &self.r[.IP].uint64);
		dst.sint32 *= imm;
	case .ImmIMul64:
		dst := _load_reg(self);
		imm := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		dst.sint64 *= imm;
	case .Div8:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint8 /= src.uint8;
	case .Div16:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint16 /= src.uint16;
	case .Div32:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint32 /= src.uint32;
	case .Div64:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.uint64 /= src.uint64;
	case .ImmDiv8:
		dst := _load_reg(self);
		imm := pop8(self.bytecode, &self.r[.IP].uint64);
		dst.uint8 /= imm;
	case .ImmDiv16:
		dst := _load_reg(self);
		imm := pop16(self.bytecode, &self.r[.IP].uint64);
		dst.uint16 /= imm;
	case .ImmDiv32:
		dst := _load_reg(self);
		imm := pop32(self.bytecode, &self.r[.IP].uint64);
		dst.uint32 /= imm;
	case .ImmDiv64:
		dst := _load_reg(self);
		imm := pop64(self.bytecode, &self.r[.IP].uint64);
		dst.uint64 /= imm;
	case .IDiv8:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint8 /= src.sint8;
	case .IDiv16:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint16 /= src.sint16;
	case .IDiv32:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint32 /= src.sint32;
	case .IDiv64:
		dst := _load_reg(self);
		src := _load_reg(self);
		dst.sint64 /= src.sint64;
	case .ImmIDiv8:
		dst := _load_reg(self);
		imm := transmute(i8)pop8(self.bytecode, &self.r[.IP].uint64);
		dst.sint8 /= imm;
	case .ImmIDiv16:
		dst := _load_reg(self);
		imm := transmute(i16)pop16(self.bytecode, &self.r[.IP].uint64);
		dst.sint16 /= imm;
	case .ImmIDiv32:
		dst := _load_reg(self);
		imm := transmute(i32)pop32(self.bytecode, &self.r[.IP].uint64);
		dst.sint32 /= imm;
	case .ImmIDiv64:
		dst := _load_reg(self);
		imm := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		dst.sint64 /= imm;
	case .Cmp8:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.uint8 > op2.uint8 {
			self.cmp = .Greater;
		} else if op1.uint8 < op2.uint8 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .Cmp16:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.uint16 > op2.uint16 {
			self.cmp = .Greater;
		} else if op1.uint16 < op2.uint16 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .Cmp32:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.uint32 > op2.uint32 {
			self.cmp = .Greater;
		} else if op1.uint32 < op2.uint32 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .Cmp64:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.uint64 > op2.uint64 {
			self.cmp = .Greater;
		} else if op1.uint64 < op2.uint64 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmCmp8:
		op1 := _load_reg(self);
		imm := pop8(self.bytecode, &self.r[.IP].uint64);
		if op1.uint8 > imm {
			self.cmp = .Greater;
		} else if op1.uint8 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmCmp16:
		op1 := _load_reg(self);
		imm := pop16(self.bytecode, &self.r[.IP].uint64);
		if op1.uint16 > imm {
			self.cmp = .Greater;
		} else if op1.uint16 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmCmp32:
		op1 := _load_reg(self);
		imm := pop32(self.bytecode, &self.r[.IP].uint64);
		if op1.uint32 > imm {
			self.cmp = .Greater;
		} else if op1.uint32 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmCmp64:
		op1 := _load_reg(self);
		imm := pop64(self.bytecode, &self.r[.IP].uint64);
		if op1.uint64 > imm {
			self.cmp = .Greater;
		} else if op1.uint64 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ICmp8:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.sint8 > op2.sint8 {
			self.cmp = .Greater;
		} else if op1.sint8 < op2.sint8 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ICmp16:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.sint16 > op2.sint16 {
			self.cmp = .Greater;
		} else if op1.sint16 < op2.sint16 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ICmp32:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.sint32 > op2.sint32 {
			self.cmp = .Greater;
		} else if op1.sint32 < op2.sint32 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ICmp64:
		op1 := _load_reg(self);
		op2 := _load_reg(self);
		if op1.sint64 > op2.sint64 {
			self.cmp = .Greater;
		} else if op1.sint64 < op2.sint64 {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmICmp8:
		op1 := _load_reg(self);
		imm := transmute(i8)pop8(self.bytecode, &self.r[.IP].uint64);
		if op1.sint8 > imm {
			self.cmp = .Greater;
		} else if op1.sint8 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmICmp16:
		op1 := _load_reg(self);
		imm := transmute(i16)pop16(self.bytecode, &self.r[.IP].uint64);
		if op1.sint16 > imm {
			self.cmp = .Greater;
		} else if op1.sint16 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmICmp32:
		op1 := _load_reg(self);
		imm := transmute(i32)pop32(self.bytecode, &self.r[.IP].uint64);
		if op1.sint32 > imm {
			self.cmp = .Greater;
		} else if op1.sint32 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .ImmICmp64:
		op1 := _load_reg(self);
		imm := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if op1.sint64 > imm {
			self.cmp = .Greater;
		} else if op1.sint64 < imm {
			self.cmp = .Less;
		} else {
			self.cmp = .Equal;
		}
	case .Jmp:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		self.r[.IP].sint64 += offset;
	case .Je:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if self.cmp == .Equal do self.r[.IP].sint64 += offset;
	case .Jne:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if self.cmp != .Equal do self.r[.IP].sint64 += offset;
	case .Jl:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if self.cmp == .Less do self.r[.IP].sint64 += offset;
	case .Jle:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if self.cmp == .Less || self.cmp == .Equal do self.r[.IP].sint64 += offset;
	case .Jg:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if self.cmp == .Greater do self.r[.IP].sint64 += offset;
	case .Jge:
		offset := transmute(i64)pop64(self.bytecode, &self.r[.IP].uint64);
		if self.cmp == .Greater || self.cmp == .Equal do self.r[.IP].sint64 += offset;
	case .Read8:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u8)) == false {
			self.state = .Err;
			break;
		}
		dst.uint8 = (transmute(^u8)src.ptr)^;
	case .Read16:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u16)) == false {
			self.state = .Err;
			break;
		}
		dst.uint16 = (transmute(^u16)src.ptr)^;
	case .Read32:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u32)) == false {
			self.state = .Err;
			break;
		}
		dst.uint32 = (transmute(^u32)src.ptr)^;
	case .Read64:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u64)) == false {
			self.state = .Err;
			break;
		}
		dst.uint64 = (transmute(^u64)src.ptr)^;
	case .Write8:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u8)) == false {
			self.state = .Err;
			break;
		}
		(transmute(^u8)dst.ptr)^ = src.uint8;
	case .Write16:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u16)) == false {
			self.state = .Err;
			break;
		}
		(transmute(^u16)dst.ptr)^ = src.uint16;
	case .Write32:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u32)) == false {
			self.state = .Err;
			break;
		}
		(transmute(^u32)dst.ptr)^ = src.uint32;
	case .Write64:
		dst := _load_reg(self);
		src := _load_reg(self);
		if _valid_stack_next_bytes(self, src.ptr, size_of(u64)) == false {
			self.state = .Err;
			break;
		}
		(transmute(^u64)dst.ptr)^ = src.uint64;
	case .Push:
		dst := &self.r[.SP];
		src := _load_reg(self);
		ptr := dst.ptr - 8;
		if _valid_stack_next_bytes(self, ptr, 8) == false {
			self.state = .Err;
			break;
		}
		(transmute(^u64)ptr)^ = src.uint64;
		dst.ptr = ptr;
	case .Pop:
		dst := _load_reg(self);
		src := &self.r[.SP];
		ptr := dst.ptr;
		if _valid_stack_next_bytes(self, ptr, 8) == false {
			self.state = .Err;
			break;
		}
		dst.uint64 = (transmute(^u64)ptr)^;
		src.ptr = ptr + 8;
	case .Call:
		// load proc address
		address := pop64(self.bytecode, &self.r[.IP].uint64);
		// load stack pointer
		SP := &self.r[.SP];
		// allocate space for return address
		ptr := SP.ptr - 8;
		if _valid_stack_next_bytes(self, ptr, 8) == false {
			self.state = .Err;
			break;
		}
		// write the return address
		(transmute(^u64)ptr)^ = self.r[.IP].uint64;
		// move the stack pointer
		SP.ptr = ptr;
		// jump to proc address
		self.r[.IP].uint64 = address;
	case .Ret:
		// load stack pointer
		SP := &self.r[.SP];
		ptr := SP.ptr;
		if _valid_stack_next_bytes(self, ptr, 8) == false {
			self.state = .Err;
			break;
		}
		// restore the IP
		self.r[.IP].uint64 = (transmute(^u64)ptr)^;
		// deallocate the space for return address
		SP.ptr = ptr;
	case .CCall:
		assert(false);
	case .Halt:
		self.state = .Halt;
	case .Igl: self.state = .Err;
	case: self.state = .Err;
	}
}
