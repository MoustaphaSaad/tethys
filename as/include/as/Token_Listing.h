// This is a list of the tokens
#define TOKEN_LISTING \
	TOKEN(NONE, "<NONE>"), \
	TOKEN(COLON, ":"), \
	TOKEN(ID, "<ID>"), \
	TOKEN(INTEGER, "<INTEGER>"), \
	TOKEN(FLOAT, "<FLOAT>"), \
	TOKEN(KEYWORDS__BEGIN, ""), \
	TOKEN(KEYWORD_PROC, "PROC"), \
	TOKEN(KEYWORD_END, "END"), \
	TOKEN(KEYWORD_HALT, "HALT"), \
	TOKEN(KEYWORD_I8_LOAD, "i8.load"), \
	TOKEN(KEYWORD_I16_LOAD, "i16.load"), \
	TOKEN(KEYWORD_I32_LOAD, "i32.load"), \
	TOKEN(KEYWORD_I64_LOAD, "i64.load"), \
	TOKEN(KEYWORD_U8_LOAD, "u8.load"), \
	TOKEN(KEYWORD_U16_LOAD, "u16.load"), \
	TOKEN(KEYWORD_U32_LOAD, "u32.load"), \
	TOKEN(KEYWORD_U64_LOAD, "u64.load"), \
	TOKEN(KEYWORD_I8_ADD, "i8.add"), \
	TOKEN(KEYWORD_I16_ADD, "i16.add"), \
	TOKEN(KEYWORD_I32_ADD, "i32.add"), \
	TOKEN(KEYWORD_I64_ADD, "i64.add"), \
	TOKEN(KEYWORD_U8_ADD, "u8.add"), \
	TOKEN(KEYWORD_U16_ADD, "u16.add"), \
	TOKEN(KEYWORD_U32_ADD, "u32.add"), \
	TOKEN(KEYWORD_U64_ADD, "u64.add"), \
	TOKEN(KEYWORD_I8_SUB, "i8.sub"), \
	TOKEN(KEYWORD_I16_SUB, "i16.sub"), \
	TOKEN(KEYWORD_I32_SUB, "i32.sub"), \
	TOKEN(KEYWORD_I64_SUB, "i64.sub"), \
	TOKEN(KEYWORD_U8_SUB, "u8.sub"), \
	TOKEN(KEYWORD_U16_SUB, "u16.sub"), \
	TOKEN(KEYWORD_U32_SUB, "u32.sub"), \
	TOKEN(KEYWORD_U64_SUB, "u64.sub"), \
	TOKEN(KEYWORD_I8_MUL, "i8.mul"), \
	TOKEN(KEYWORD_I16_MUL, "i16.mul"), \
	TOKEN(KEYWORD_I32_MUL, "i32.mul"), \
	TOKEN(KEYWORD_I64_MUL, "i64.mul"), \
	TOKEN(KEYWORD_U8_MUL, "u8.mul"), \
	TOKEN(KEYWORD_U16_MUL, "u16.mul"), \
	TOKEN(KEYWORD_U32_MUL, "u32.mul"), \
	TOKEN(KEYWORD_U64_MUL, "u64.mul"), \
	TOKEN(KEYWORD_I8_DIV, "i8.div"), \
	TOKEN(KEYWORD_I16_DIV, "i16.div"), \
	TOKEN(KEYWORD_I32_DIV, "i32.div"), \
	TOKEN(KEYWORD_I64_DIV, "i64.div"), \
	TOKEN(KEYWORD_U8_DIV, "u8.div"), \
	TOKEN(KEYWORD_U16_DIV, "u16.div"), \
	TOKEN(KEYWORD_U32_DIV, "u32.div"), \
	TOKEN(KEYWORD_U64_DIV, "u64.div"), \
	TOKEN(KEYWORD_JMP, "jmp"), \
	TOKEN(KEYWORD_I8_JE, "i8.je"), \
	TOKEN(KEYWORD_I16_JE, "i16.je"), \
	TOKEN(KEYWORD_I32_JE, "i32.je"), \
	TOKEN(KEYWORD_I64_JE, "i64.je"), \
	TOKEN(KEYWORD_U8_JE, "u8.je"), \
	TOKEN(KEYWORD_U16_JE, "u16.je"), \
	TOKEN(KEYWORD_U32_JE, "u32.je"), \
	TOKEN(KEYWORD_U64_JE, "u64.je"), \
	TOKEN(KEYWORD_I8_JNE, "i8.jne"), \
	TOKEN(KEYWORD_I16_JNE, "i16.jne"), \
	TOKEN(KEYWORD_I32_JNE, "i32.jne"), \
	TOKEN(KEYWORD_I64_JNE, "i64.jne"), \
	TOKEN(KEYWORD_U8_JNE, "u8.jne"), \
	TOKEN(KEYWORD_U16_JNE, "u16.jne"), \
	TOKEN(KEYWORD_U32_JNE, "u32.jne"), \
	TOKEN(KEYWORD_U64_JNE, "u64.jne"), \
	TOKEN(KEYWORD_I8_JL, "i8.jl"), \
	TOKEN(KEYWORD_I16_JL, "i16.jl"), \
	TOKEN(KEYWORD_I32_JL, "i32.jl"), \
	TOKEN(KEYWORD_I64_JL, "i64.jl"), \
	TOKEN(KEYWORD_U8_JL, "u8.jl"), \
	TOKEN(KEYWORD_U16_JL, "u16.jl"), \
	TOKEN(KEYWORD_U32_JL, "u32.jl"), \
	TOKEN(KEYWORD_U64_JL, "u64.jl"), \
	TOKEN(KEYWORD_I8_JLE, "i8.jle"), \
	TOKEN(KEYWORD_I16_JLE, "i16.jle"), \
	TOKEN(KEYWORD_I32_JLE, "i32.jle"), \
	TOKEN(KEYWORD_I64_JLE, "i64.jle"), \
	TOKEN(KEYWORD_U8_JLE, "u8.jle"), \
	TOKEN(KEYWORD_U16_JLE, "u16.jle"), \
	TOKEN(KEYWORD_U32_JLE, "u32.jle"), \
	TOKEN(KEYWORD_U64_JLE, "u64.jle"), \
	TOKEN(KEYWORD_I8_JG, "i8.jg"), \
	TOKEN(KEYWORD_I16_JG, "i16.jg"), \
	TOKEN(KEYWORD_I32_JG, "i32.jg"), \
	TOKEN(KEYWORD_I64_JG, "i64.jg"), \
	TOKEN(KEYWORD_U8_JG, "u8.jg"), \
	TOKEN(KEYWORD_U16_JG, "u16.jg"), \
	TOKEN(KEYWORD_U32_JG, "u32.jg"), \
	TOKEN(KEYWORD_U64_JG, "u64.jg"), \
	TOKEN(KEYWORD_I8_JGE, "i8.jge"), \
	TOKEN(KEYWORD_I16_JGE, "i16.jge"), \
	TOKEN(KEYWORD_I32_JGE, "i32.jge"), \
	TOKEN(KEYWORD_I64_JGE, "i64.jge"), \
	TOKEN(KEYWORD_U8_JGE, "u8.jge"), \
	TOKEN(KEYWORD_U16_JGE, "u16.jge"), \
	TOKEN(KEYWORD_U32_JGE, "u32.jge"), \
	TOKEN(KEYWORD_U64_JGE, "u64.jge"), \
	TOKEN(KEYWORD_R0, "R0"), \
	TOKEN(KEYWORD_R1, "R1"), \
	TOKEN(KEYWORD_R2, "R2"), \
	TOKEN(KEYWORD_R3, "R3"), \
	TOKEN(KEYWORD_R4, "R4"), \
	TOKEN(KEYWORD_R5, "R5"), \
	TOKEN(KEYWORD_R6, "R6"), \
	TOKEN(KEYWORD_R7, "R7"), \
	TOKEN(KEYWORD_IP, "IP"), \
	TOKEN(KEYWORDS__END, ""),
