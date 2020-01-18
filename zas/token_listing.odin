package zas

Tkn_Desc :: struct {
	kind: Tkn_Kind,
	mnemonic: string,
}

Tkn_Kind :: enum {
	None,
	Comment,
	Colon,
	Open_Paren,
	Close_Paren,
	Comma,
	ID,
	Const_String,
	Const_Integer,
	Const_Float,
	Keyword__Begin,

	// C Types
	Keyword_C_Void = Keyword__Begin,
	Keyword_C_Int8,
	Keyword_C_Int16,
	Keyword_C_Int32,
	Keyword_C_Int64,
	Keyword_C_Uint8,
	Keyword_C_Uint16,
	Keyword_C_Uint32,
	Keyword_C_Uint64,
	Keyword_C_Float32,
	Keyword_C_Float64,
	Keyword_C_Ptr,

	// Registers
	Keyword_R0,
	Keyword_R1,
	Keyword_R2,
	Keyword_R3,
	Keyword_R4,
	Keyword_R5,
	Keyword_R6,
	Keyword_R7,
	Keyword_IP,
	Keyword_SP,

	// Directives
	Keyword_Proc,
	Keyword_End,
	Keyword_Constant,

	// Load Instructions
	Keyword_I8_Load,
	Keyword_I16_Load,
	Keyword_I32_Load,
	Keyword_I64_Load,
	Keyword_U8_Load,
	Keyword_U16_Load,
	Keyword_U32_Load,
	Keyword_U64_Load,

	// Add Instructions
	Keyword_I8_Add,
	Keyword_I16_Add,
	Keyword_I32_Add,
	Keyword_I64_Add,
	Keyword_U8_Add,
	Keyword_U16_Add,
	Keyword_U32_Add,
	Keyword_U64_Add,

	// Sub Instructions
	Keyword_I8_Sub,
	Keyword_I16_Sub,
	Keyword_I32_Sub,
	Keyword_I64_Sub,
	Keyword_U8_Sub,
	Keyword_U16_Sub,
	Keyword_U32_Sub,
	Keyword_U64_Sub,

	// Mul Instructions
	Keyword_I8_Mul,
	Keyword_I16_Mul,
	Keyword_I32_Mul,
	Keyword_I64_Mul,
	Keyword_U8_Mul,
	Keyword_U16_Mul,
	Keyword_U32_Mul,
	Keyword_U64_Mul,

	// Div Instructions
	Keyword_I8_Div,
	Keyword_I16_Div,
	Keyword_I32_Div,
	Keyword_I64_Div,
	Keyword_U8_Div,
	Keyword_U16_Div,
	Keyword_U32_Div,
	Keyword_U64_Div,

	// Cmp Instructions
	Keyword_I8_Cmp,
	Keyword_I16_Cmp,
	Keyword_I32_Cmp,
	Keyword_I64_Cmp,
	Keyword_U8_Cmp,
	Keyword_U16_Cmp,
	Keyword_U32_Cmp,
	Keyword_U64_Cmp,

	// Conditional Jump If Equal Instructions
	Keyword_I8_Je,
	Keyword_I16_Je,
	Keyword_I32_Je,
	Keyword_I64_Je,
	Keyword_U8_Je,
	Keyword_U16_Je,
	Keyword_U32_Je,
	Keyword_U64_Je,

	// Conditional Jump If Not Equal Instructions
	Keyword_I8_Jne,
	Keyword_I16_Jne,
	Keyword_I32_Jne,
	Keyword_I64_Jne,
	Keyword_U8_Jne,
	Keyword_U16_Jne,
	Keyword_U32_Jne,
	Keyword_U64_Jne,

	// Conditional Jump If Less Instructions
	Keyword_I8_Jl,
	Keyword_I16_Jl,
	Keyword_I32_Jl,
	Keyword_I64_Jl,
	Keyword_U8_Jl,
	Keyword_U16_Jl,
	Keyword_U32_Jl,
	Keyword_U64_Jl,

	// Conditional Jump If Less or Equal Instructions
	Keyword_I8_Jle,
	Keyword_I16_Jle,
	Keyword_I32_Jle,
	Keyword_I64_Jle,
	Keyword_U8_Jle,
	Keyword_U16_Jle,
	Keyword_U32_Jle,
	Keyword_U64_Jle,

	// Conditional Jump If Greater Instructions
	Keyword_I8_Jg,
	Keyword_I16_Jg,
	Keyword_I32_Jg,
	Keyword_I64_Jg,
	Keyword_U8_Jg,
	Keyword_U16_Jg,
	Keyword_U32_Jg,
	Keyword_U64_Jg,

	// Conditional Jump If Greater or Equal Instructions
	Keyword_I8_Jge,
	Keyword_I16_Jge,
	Keyword_I32_Jge,
	Keyword_I64_Jge,
	Keyword_U8_Jge,
	Keyword_U16_Jge,
	Keyword_U32_Jge,
	Keyword_U64_Jge,

	// Read Instructions
	Keyword_I8_Read,
	Keyword_I16_Read,
	Keyword_I32_Read,
	Keyword_I64_Read,
	Keyword_U8_Read,
	Keyword_U16_Read,
	Keyword_U32_Read,
	Keyword_U64_Read,

	// Write Instructions
	Keyword_I8_Write,
	Keyword_I16_Write,
	Keyword_I32_Write,
	Keyword_I64_Write,
	Keyword_U8_Write,
	Keyword_U16_Write,
	Keyword_U32_Write,
	Keyword_U64_Write,

	// Jump Instructions
	Keyword_Je,
	Keyword_Jne,
	Keyword_Jl,
	Keyword_Jle,
	Keyword_Jg,
	Keyword_Jge,
	Keyword_Jmp,

	// Misc Instructions
	Keyword_Push,
	Keyword_Pop,
	Keyword_Call,
	Keyword_Ret,
	Keyword_Halt,

	Keyword__End,
}

TKN_DESC := [Tkn_Kind]Tkn_Desc {
	Tkn_Kind.None          = { kind = .None,          mnemonic = "", },
	Tkn_Kind.Comment       = { kind = .Comment,       mnemonic = "", },
	Tkn_Kind.Colon         = { kind = .Colon,         mnemonic = "", },
	Tkn_Kind.Open_Paren    = { kind = .Open_Paren,    mnemonic = "", },
	Tkn_Kind.Close_Paren   = { kind = .Close_Paren,   mnemonic = "", },
	Tkn_Kind.Comma         = { kind = .Comma,         mnemonic = "", },
	Tkn_Kind.ID            = { kind = .ID,            mnemonic = "", },
	Tkn_Kind.Const_String  = { kind = .Const_String,  mnemonic = "", },
	Tkn_Kind.Const_Integer = { kind = .Const_Integer, mnemonic = "", },
	Tkn_Kind.Const_Float   = { kind = .Const_Float,   mnemonic = "", },

	// C Types
	Tkn_Kind.Keyword_C_Void    = { kind = .Keyword_C_Void,    mnemonic = "C.void"    },
	Tkn_Kind.Keyword_C_Int8    = { kind = .Keyword_C_Int8,    mnemonic = "C.int8"    },
	Tkn_Kind.Keyword_C_Int16   = { kind = .Keyword_C_Int16,   mnemonic = "C.int16"   },
	Tkn_Kind.Keyword_C_Int32   = { kind = .Keyword_C_Int32,   mnemonic = "C.int32"   },
	Tkn_Kind.Keyword_C_Int64   = { kind = .Keyword_C_Int64,   mnemonic = "C.int64"   },
	Tkn_Kind.Keyword_C_Uint8   = { kind = .Keyword_C_Uint8,   mnemonic = "C.uint8"   },
	Tkn_Kind.Keyword_C_Uint16  = { kind = .Keyword_C_Uint16,  mnemonic = "C.uint16"  },
	Tkn_Kind.Keyword_C_Uint32  = { kind = .Keyword_C_Uint32,  mnemonic = "C.uint32"  },
	Tkn_Kind.Keyword_C_Uint64  = { kind = .Keyword_C_Uint64,  mnemonic = "C.uint64"  },
	Tkn_Kind.Keyword_C_Float32 = { kind = .Keyword_C_Float32, mnemonic = "C.float32" },
	Tkn_Kind.Keyword_C_Float64 = { kind = .Keyword_C_Float64, mnemonic = "C.float64" },

	// Registers
	Tkn_Kind.Keyword_R0 = { kind = .Keyword_R0, mnemonic = "R0" },
	Tkn_Kind.Keyword_R1 = { kind = .Keyword_R1, mnemonic = "R1" },
	Tkn_Kind.Keyword_R2 = { kind = .Keyword_R2, mnemonic = "R2" },
	Tkn_Kind.Keyword_R3 = { kind = .Keyword_R3, mnemonic = "R3" },
	Tkn_Kind.Keyword_R4 = { kind = .Keyword_R4, mnemonic = "R4" },
	Tkn_Kind.Keyword_R5 = { kind = .Keyword_R5, mnemonic = "R5" },
	Tkn_Kind.Keyword_R6 = { kind = .Keyword_R6, mnemonic = "R6" },
	Tkn_Kind.Keyword_R7 = { kind = .Keyword_R7, mnemonic = "R7" },
	Tkn_Kind.Keyword_IP = { kind = .Keyword_IP, mnemonic = "IP" },
	Tkn_Kind.Keyword_SP = { kind = .Keyword_SP, mnemonic = "SP" },

	// Directives
	Tkn_Kind.Keyword_Proc     = { kind = .Keyword_Proc,     mnemonic = "PROC"     },
	Tkn_Kind.Keyword_End      = { kind = .Keyword_End,      mnemonic = "END"      },
	Tkn_Kind.Keyword_Constant = { kind = .Keyword_Constant, mnemonic = "CONSTANT" },

	// Load Instructions
	Tkn_Kind.Keyword_I8_Load  = { kind = .Keyword_I8_Load,  mnemonic = "i8.load"  },
	Tkn_Kind.Keyword_I16_Load = { kind = .Keyword_I16_Load, mnemonic = "i16.load" },
	Tkn_Kind.Keyword_I32_Load = { kind = .Keyword_I32_Load, mnemonic = "i32.load" },
	Tkn_Kind.Keyword_I64_Load = { kind = .Keyword_I64_Load, mnemonic = "i64.load" },
	Tkn_Kind.Keyword_U8_Load  = { kind = .Keyword_U8_Load,  mnemonic = "u8.load"  },
	Tkn_Kind.Keyword_U16_Load = { kind = .Keyword_U16_Load, mnemonic = "u16.load" },
	Tkn_Kind.Keyword_U32_Load = { kind = .Keyword_U32_Load, mnemonic = "u32.load" },
	Tkn_Kind.Keyword_U64_Load = { kind = .Keyword_U64_Load, mnemonic = "u64.load" },

	// Add Instructions
	Tkn_Kind.Keyword_I8_Add  = { kind = .Keyword_I8_Add,  mnemonic = "i8.add"  },
	Tkn_Kind.Keyword_I16_Add = { kind = .Keyword_I16_Add, mnemonic = "i16.add" },
	Tkn_Kind.Keyword_I32_Add = { kind = .Keyword_I32_Add, mnemonic = "i32.add" },
	Tkn_Kind.Keyword_I64_Add = { kind = .Keyword_I64_Add, mnemonic = "i64.add" },
	Tkn_Kind.Keyword_U8_Add  = { kind = .Keyword_U8_Add,  mnemonic = "u8.add"  },
	Tkn_Kind.Keyword_U16_Add = { kind = .Keyword_U16_Add, mnemonic = "u16.add" },
	Tkn_Kind.Keyword_U32_Add = { kind = .Keyword_U32_Add, mnemonic = "u32.add" },
	Tkn_Kind.Keyword_U64_Add = { kind = .Keyword_U64_Add, mnemonic = "u64.add" },

	// Sub Instructions
	Tkn_Kind.Keyword_I8_Sub  = { kind = .Keyword_I8_Sub,  mnemonic = "i8.sub"  },
	Tkn_Kind.Keyword_I16_Sub = { kind = .Keyword_I16_Sub, mnemonic = "i16.sub" },
	Tkn_Kind.Keyword_I32_Sub = { kind = .Keyword_I32_Sub, mnemonic = "i32.sub" },
	Tkn_Kind.Keyword_I64_Sub = { kind = .Keyword_I64_Sub, mnemonic = "i64.sub" },
	Tkn_Kind.Keyword_U8_Sub  = { kind = .Keyword_U8_Sub,  mnemonic = "u8.sub"  },
	Tkn_Kind.Keyword_U16_Sub = { kind = .Keyword_U16_Sub, mnemonic = "u16.sub" },
	Tkn_Kind.Keyword_U32_Sub = { kind = .Keyword_U32_Sub, mnemonic = "u32.sub" },
	Tkn_Kind.Keyword_U64_Sub = { kind = .Keyword_U64_Sub, mnemonic = "u64.sub" },

	// Mul Instructions
	Tkn_Kind.Keyword_I8_Mul  = { kind = .Keyword_I8_Mul,  mnemonic = "i8.mul"  },
	Tkn_Kind.Keyword_I16_Mul = { kind = .Keyword_I16_Mul, mnemonic = "i16.mul" },
	Tkn_Kind.Keyword_I32_Mul = { kind = .Keyword_I32_Mul, mnemonic = "i32.mul" },
	Tkn_Kind.Keyword_I64_Mul = { kind = .Keyword_I64_Mul, mnemonic = "i64.mul" },
	Tkn_Kind.Keyword_U8_Mul  = { kind = .Keyword_U8_Mul,  mnemonic = "u8.mul"  },
	Tkn_Kind.Keyword_U16_Mul = { kind = .Keyword_U16_Mul, mnemonic = "u16.mul" },
	Tkn_Kind.Keyword_U32_Mul = { kind = .Keyword_U32_Mul, mnemonic = "u32.mul" },
	Tkn_Kind.Keyword_U64_Mul = { kind = .Keyword_U64_Mul, mnemonic = "u64.mul" },

	// Div Instructions
	Tkn_Kind.Keyword_I8_Div  = { kind = .Keyword_I8_Div,  mnemonic = "i8.div"  },
	Tkn_Kind.Keyword_I16_Div = { kind = .Keyword_I16_Div, mnemonic = "i16.div" },
	Tkn_Kind.Keyword_I32_Div = { kind = .Keyword_I32_Div, mnemonic = "i32.div" },
	Tkn_Kind.Keyword_I64_Div = { kind = .Keyword_I64_Div, mnemonic = "i64.div" },
	Tkn_Kind.Keyword_U8_Div  = { kind = .Keyword_U8_Div,  mnemonic = "u8.div"  },
	Tkn_Kind.Keyword_U16_Div = { kind = .Keyword_U16_Div, mnemonic = "u16.div" },
	Tkn_Kind.Keyword_U32_Div = { kind = .Keyword_U32_Div, mnemonic = "u32.div" },
	Tkn_Kind.Keyword_U64_Div = { kind = .Keyword_U64_Div, mnemonic = "u64.div" },

	// Cmp Instructions
	Tkn_Kind.Keyword_I8_Cmp  = { kind = .Keyword_I8_Cmp,  mnemonic = "i8.cmp"  },
	Tkn_Kind.Keyword_I16_Cmp = { kind = .Keyword_I16_Cmp, mnemonic = "i16.cmp" },
	Tkn_Kind.Keyword_I32_Cmp = { kind = .Keyword_I32_Cmp, mnemonic = "i32.cmp" },
	Tkn_Kind.Keyword_I64_Cmp = { kind = .Keyword_I64_Cmp, mnemonic = "i64.cmp" },
	Tkn_Kind.Keyword_U8_Cmp  = { kind = .Keyword_U8_Cmp,  mnemonic = "u8.cmp"  },
	Tkn_Kind.Keyword_U16_Cmp = { kind = .Keyword_U16_Cmp, mnemonic = "u16.cmp" },
	Tkn_Kind.Keyword_U32_Cmp = { kind = .Keyword_U32_Cmp, mnemonic = "u32.cmp" },
	Tkn_Kind.Keyword_U64_Cmp = { kind = .Keyword_U64_Cmp, mnemonic = "u64.cmp" },

	// Condition Jump If Equal Instructions
	Tkn_Kind.Keyword_I8_Je  = { kind = .Keyword_I8_Je,  mnemonic = "i8.je"  },
	Tkn_Kind.Keyword_I16_Je = { kind = .Keyword_I16_Je, mnemonic = "i16.je" },
	Tkn_Kind.Keyword_I32_Je = { kind = .Keyword_I32_Je, mnemonic = "i32.je" },
	Tkn_Kind.Keyword_I64_Je = { kind = .Keyword_I64_Je, mnemonic = "i64.je" },
	Tkn_Kind.Keyword_U8_Je  = { kind = .Keyword_U8_Je,  mnemonic = "u8.je"  },
	Tkn_Kind.Keyword_U16_Je = { kind = .Keyword_U16_Je, mnemonic = "u16.je" },
	Tkn_Kind.Keyword_U32_Je = { kind = .Keyword_U32_Je, mnemonic = "u32.je" },
	Tkn_Kind.Keyword_U64_Je = { kind = .Keyword_U64_Je, mnemonic = "u64.je" },

	// Condition Jump If Not Equal Instructions
	Tkn_Kind.Keyword_I8_Jne  = { kind = .Keyword_I8_Jne,  mnemonic = "i8.jne"  },
	Tkn_Kind.Keyword_I16_Jne = { kind = .Keyword_I16_Jne, mnemonic = "i16.jne" },
	Tkn_Kind.Keyword_I32_Jne = { kind = .Keyword_I32_Jne, mnemonic = "i32.jne" },
	Tkn_Kind.Keyword_I64_Jne = { kind = .Keyword_I64_Jne, mnemonic = "i64.jne" },
	Tkn_Kind.Keyword_U8_Jne  = { kind = .Keyword_U8_Jne,  mnemonic = "u8.jne"  },
	Tkn_Kind.Keyword_U16_Jne = { kind = .Keyword_U16_Jne, mnemonic = "u16.jne" },
	Tkn_Kind.Keyword_U32_Jne = { kind = .Keyword_U32_Jne, mnemonic = "u32.jne" },
	Tkn_Kind.Keyword_U64_Jne = { kind = .Keyword_U64_Jne, mnemonic = "u64.jne" },

	// Condition Jump If Less Instructions
	Tkn_Kind.Keyword_I8_Jl  = { kind = .Keyword_I8_Jl,  mnemonic = "i8.jl"  },
	Tkn_Kind.Keyword_I16_Jl = { kind = .Keyword_I16_Jl, mnemonic = "i16.jl" },
	Tkn_Kind.Keyword_I32_Jl = { kind = .Keyword_I32_Jl, mnemonic = "i32.jl" },
	Tkn_Kind.Keyword_I64_Jl = { kind = .Keyword_I64_Jl, mnemonic = "i64.jl" },
	Tkn_Kind.Keyword_U8_Jl  = { kind = .Keyword_U8_Jl,  mnemonic = "u8.jl"  },
	Tkn_Kind.Keyword_U16_Jl = { kind = .Keyword_U16_Jl, mnemonic = "u16.jl" },
	Tkn_Kind.Keyword_U32_Jl = { kind = .Keyword_U32_Jl, mnemonic = "u32.jl" },
	Tkn_Kind.Keyword_U64_Jl = { kind = .Keyword_U64_Jl, mnemonic = "u64.jl" },

	// Condition Jump If Less or Equal Instructions
	Tkn_Kind.Keyword_I8_Jle  = { kind = .Keyword_I8_Jle,  mnemonic = "i8.jle"  },
	Tkn_Kind.Keyword_I16_Jle = { kind = .Keyword_I16_Jle, mnemonic = "i16.jle" },
	Tkn_Kind.Keyword_I32_Jle = { kind = .Keyword_I32_Jle, mnemonic = "i32.jle" },
	Tkn_Kind.Keyword_I64_Jle = { kind = .Keyword_I64_Jle, mnemonic = "i64.jle" },
	Tkn_Kind.Keyword_U8_Jle  = { kind = .Keyword_U8_Jle,  mnemonic = "u8.jle"  },
	Tkn_Kind.Keyword_U16_Jle = { kind = .Keyword_U16_Jle, mnemonic = "u16.jle" },
	Tkn_Kind.Keyword_U32_Jle = { kind = .Keyword_U32_Jle, mnemonic = "u32.jle" },
	Tkn_Kind.Keyword_U64_Jle = { kind = .Keyword_U64_Jle, mnemonic = "u64.jle" },

	// Condition Jump If Greater Instructions
	Tkn_Kind.Keyword_I8_Jg  = { kind = .Keyword_I8_Jg,  mnemonic = "i8.jg"  },
	Tkn_Kind.Keyword_I16_Jg = { kind = .Keyword_I16_Jg, mnemonic = "i16.jg" },
	Tkn_Kind.Keyword_I32_Jg = { kind = .Keyword_I32_Jg, mnemonic = "i32.jg" },
	Tkn_Kind.Keyword_I64_Jg = { kind = .Keyword_I64_Jg, mnemonic = "i64.jg" },
	Tkn_Kind.Keyword_U8_Jg  = { kind = .Keyword_U8_Jg,  mnemonic = "u8.jg"  },
	Tkn_Kind.Keyword_U16_Jg = { kind = .Keyword_U16_Jg, mnemonic = "u16.jg" },
	Tkn_Kind.Keyword_U32_Jg = { kind = .Keyword_U32_Jg, mnemonic = "u32.jg" },
	Tkn_Kind.Keyword_U64_Jg = { kind = .Keyword_U64_Jg, mnemonic = "u64.jg" },

	// Condition Jump If Greater or Equal Instructions
	Tkn_Kind.Keyword_I8_Jge  = { kind = .Keyword_I8_Jge,  mnemonic = "i8.jge"  },
	Tkn_Kind.Keyword_I16_Jge = { kind = .Keyword_I16_Jge, mnemonic = "i16.jge" },
	Tkn_Kind.Keyword_I32_Jge = { kind = .Keyword_I32_Jge, mnemonic = "i32.jge" },
	Tkn_Kind.Keyword_I64_Jge = { kind = .Keyword_I64_Jge, mnemonic = "i64.jge" },
	Tkn_Kind.Keyword_U8_Jge  = { kind = .Keyword_U8_Jge,  mnemonic = "u8.jge"  },
	Tkn_Kind.Keyword_U16_Jge = { kind = .Keyword_U16_Jge, mnemonic = "u16.jge" },
	Tkn_Kind.Keyword_U32_Jge = { kind = .Keyword_U32_Jge, mnemonic = "u32.jge" },
	Tkn_Kind.Keyword_U64_Jge = { kind = .Keyword_U64_Jge, mnemonic = "u64.jge" },

	// Read Instructions
	Tkn_Kind.Keyword_I8_Read  = { kind = .Keyword_I8_Read,  mnemonic = "i8.read"  },
	Tkn_Kind.Keyword_I16_Read = { kind = .Keyword_I16_Read, mnemonic = "i16.read" },
	Tkn_Kind.Keyword_I32_Read = { kind = .Keyword_I32_Read, mnemonic = "i32.read" },
	Tkn_Kind.Keyword_I64_Read = { kind = .Keyword_I64_Read, mnemonic = "i64.read" },
	Tkn_Kind.Keyword_U8_Read  = { kind = .Keyword_U8_Read,  mnemonic = "u8.read"  },
	Tkn_Kind.Keyword_U16_Read = { kind = .Keyword_U16_Read, mnemonic = "u16.read" },
	Tkn_Kind.Keyword_U32_Read = { kind = .Keyword_U32_Read, mnemonic = "u32.read" },
	Tkn_Kind.Keyword_U64_Read = { kind = .Keyword_U64_Read, mnemonic = "u64.read" },

	// Write Instructions
	Tkn_Kind.Keyword_I8_Write  = { kind = .Keyword_I8_Write,  mnemonic = "i8.write"  },
	Tkn_Kind.Keyword_I16_Write = { kind = .Keyword_I16_Write, mnemonic = "i16.write" },
	Tkn_Kind.Keyword_I32_Write = { kind = .Keyword_I32_Write, mnemonic = "i32.write" },
	Tkn_Kind.Keyword_I64_Write = { kind = .Keyword_I64_Write, mnemonic = "i64.write" },
	Tkn_Kind.Keyword_U8_Write  = { kind = .Keyword_U8_Write,  mnemonic = "u8.write"  },
	Tkn_Kind.Keyword_U16_Write = { kind = .Keyword_U16_Write, mnemonic = "u16.write" },
	Tkn_Kind.Keyword_U32_Write = { kind = .Keyword_U32_Write, mnemonic = "u32.write" },
	Tkn_Kind.Keyword_U64_Write = { kind = .Keyword_U64_Write, mnemonic = "u64.write" },

	// Jump instructions
	Tkn_Kind.Keyword_Je  = { kind = .Keyword_Je,  mnemonic = "je"  },
	Tkn_Kind.Keyword_Jne = { kind = .Keyword_Jne, mnemonic = "jne" },
	Tkn_Kind.Keyword_Jl  = { kind = .Keyword_Jl,  mnemonic = "jl"  },
	Tkn_Kind.Keyword_Jle = { kind = .Keyword_Jle, mnemonic = "jle" },
	Tkn_Kind.Keyword_Jg  = { kind = .Keyword_Jg,  mnemonic = "jg"  },
	Tkn_Kind.Keyword_Jge = { kind = .Keyword_Jge, mnemonic = "jge" },
	Tkn_Kind.Keyword_Jmp = { kind = .Keyword_Jmp, mnemonic = "jmp" },

	// Misc Instructions
	Tkn_Kind.Keyword_Push = { kind = .Keyword_Push, mnemonic = "push" },
	Tkn_Kind.Keyword_Pop  = { kind = .Keyword_Pop,  mnemonic = "pop"  },
	Tkn_Kind.Keyword_Call = { kind = .Keyword_Call, mnemonic = "call" },
	Tkn_Kind.Keyword_Ret  = { kind = .Keyword_Ret,  mnemonic = "ret"  },
	Tkn_Kind.Keyword_Halt = { kind = .Keyword_Halt, mnemonic = "halt" },
};

tkn_kind_is_reg :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_R0 ||
			k == .Keyword_R1 ||
			k == .Keyword_R2 ||
			k == .Keyword_R3 ||
			k == .Keyword_R4 ||
			k == .Keyword_R5 ||
			k == .Keyword_R6 ||
			k == .Keyword_R7 ||
			k == .Keyword_IP ||
			k == .Keyword_SP);
}

tkn_kind_is_ctype :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_C_Void    ||
			k == .Keyword_C_Int8    ||
			k == .Keyword_C_Int16   ||
			k == .Keyword_C_Int32   ||
			k == .Keyword_C_Int64   ||
			k == .Keyword_C_Uint8   ||
			k == .Keyword_C_Uint16  ||
			k == .Keyword_C_Uint32  ||
			k == .Keyword_C_Uint64  ||
			k == .Keyword_C_Float32 ||
			k == .Keyword_C_Float64 ||
			k == .Keyword_C_Ptr);
}

tkn_kind_is_load :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_I8_Load  ||
			k == .Keyword_I16_Load ||
			k == .Keyword_I32_Load ||
			k == .Keyword_I64_Load ||
			k == .Keyword_U8_Load  ||
			k == .Keyword_U16_Load ||
			k == .Keyword_U32_Load ||
			k == .Keyword_U64_Load);
}

tkn_kind_is_arithmetic :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_I8_Add  ||
			k == .Keyword_I16_Add ||
			k == .Keyword_I32_Add ||
			k == .Keyword_I64_Add ||
			k == .Keyword_U8_Add  ||
			k == .Keyword_U16_Add ||
			k == .Keyword_U32_Add ||
			k == .Keyword_U64_Add ||
			k == .Keyword_I8_Sub  ||
			k == .Keyword_I16_Sub ||
			k == .Keyword_I32_Sub ||
			k == .Keyword_I64_Sub ||
			k == .Keyword_U8_Sub  ||
			k == .Keyword_U16_Sub ||
			k == .Keyword_U32_Sub ||
			k == .Keyword_U64_Sub ||
			k == .Keyword_I8_Mul  ||
			k == .Keyword_I16_Mul ||
			k == .Keyword_I32_Mul ||
			k == .Keyword_I64_Mul ||
			k == .Keyword_U8_Mul  ||
			k == .Keyword_U16_Mul ||
			k == .Keyword_U32_Mul ||
			k == .Keyword_U64_Mul ||
			k == .Keyword_I8_Div  ||
			k == .Keyword_I16_Div ||
			k == .Keyword_I32_Div ||
			k == .Keyword_I64_Div ||
			k == .Keyword_U8_Div  ||
			k == .Keyword_U16_Div ||
			k == .Keyword_U32_Div ||
			k == .Keyword_U64_Div);
}

tkn_kind_is_cond_jump :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_I8_Je   ||
			k == .Keyword_I16_Je  ||
			k == .Keyword_I32_Je  ||
			k == .Keyword_I64_Je  ||
			k == .Keyword_U8_Je   ||
			k == .Keyword_U16_Je  ||
			k == .Keyword_U32_Je  ||
			k == .Keyword_U64_Je  ||
			k == .Keyword_I8_Jne  ||
			k == .Keyword_I16_Jne ||
			k == .Keyword_I32_Jne ||
			k == .Keyword_I64_Jne ||
			k == .Keyword_U8_Jne  ||
			k == .Keyword_U16_Jne ||
			k == .Keyword_U32_Jne ||
			k == .Keyword_U64_Jne ||
			k == .Keyword_I8_Jl   ||
			k == .Keyword_I16_Jl  ||
			k == .Keyword_I32_Jl  ||
			k == .Keyword_I64_Jl  ||
			k == .Keyword_U8_Jl   ||
			k == .Keyword_U16_Jl  ||
			k == .Keyword_U32_Jl  ||
			k == .Keyword_U64_Jl  ||
			k == .Keyword_I8_Jle  ||
			k == .Keyword_I16_Jle ||
			k == .Keyword_I32_Jle ||
			k == .Keyword_I64_Jle ||
			k == .Keyword_U8_Jle  ||
			k == .Keyword_U16_Jle ||
			k == .Keyword_U32_Jle ||
			k == .Keyword_U64_Jle ||
			k == .Keyword_I8_Jg   ||
			k == .Keyword_I16_Jg  ||
			k == .Keyword_I32_Jg  ||
			k == .Keyword_I64_Jg  ||
			k == .Keyword_U8_Jg   ||
			k == .Keyword_U16_Jg  ||
			k == .Keyword_U32_Jg  ||
			k == .Keyword_U64_Jg  ||
			k == .Keyword_I8_Jge  ||
			k == .Keyword_I16_Jge ||
			k == .Keyword_I32_Jge ||
			k == .Keyword_I64_Jge ||
			k == .Keyword_U8_Jge  ||
			k == .Keyword_U16_Jge ||
			k == .Keyword_U32_Jge ||
			k == .Keyword_U64_Jge);
}

tkn_kind_is_mem_transfer :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_I8_Read   ||
			k == .Keyword_I16_Read  ||
			k == .Keyword_I32_Read  ||
			k == .Keyword_I64_Read  ||
			k == .Keyword_U8_Read   ||
			k == .Keyword_U16_Read  ||
			k == .Keyword_U32_Read  ||
			k == .Keyword_U64_Read  ||
			k == .Keyword_I8_Write  ||
			k == .Keyword_I16_Write ||
			k == .Keyword_I32_Write ||
			k == .Keyword_I64_Write ||
			k == .Keyword_U8_Write  ||
			k == .Keyword_U16_Write ||
			k == .Keyword_U32_Write ||
			k == .Keyword_U64_Write);
}

tkn_kind_is_push_pop :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_Push || k == .Keyword_Pop);
}

tkn_kind_is_cmp :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_I8_Cmp   ||
			k == .Keyword_I16_Cmp  ||
			k == .Keyword_I32_Cmp  ||
			k == .Keyword_I64_Cmp  ||
			k == .Keyword_U8_Cmp   ||
			k == .Keyword_U16_Cmp  ||
			k == .Keyword_U32_Cmp  ||
			k == .Keyword_U64_Cmp);
}

tkn_kind_is_jump :: proc(k: Tkn_Kind) -> bool {
	return (k == .Keyword_Je ||
			k == .Keyword_Jne ||
			k == .Keyword_Jl ||
			k == .Keyword_Jle ||
			k == .Keyword_Jg ||
			k == .Keyword_Jge ||
			k == .Keyword_Jmp);
}
