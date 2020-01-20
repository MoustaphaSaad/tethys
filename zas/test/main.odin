package main

import "core:fmt"
import "core:strings"
import "core:os"
import ".."

scan_str :: proc(code: string) -> string {
	src := zas.src_from_string(code);
	defer zas.src_delete(&src);

	answer: string;
	if zas.src_scan(&src) {
		answer = zas.src_tkns_dump(&src);
	} else {
		answer = zas.src_errs_dump(&src);
	}

	when os.OS == "windows" {
		res, _ := strings.replace_all(fmt.tprintf("\n%v", answer), "\n", "\r\n");
		return res;
	} else {
		return fmt.tprintf("\n%v", answer);
	}
}

scan_add_program :: proc() {
	answer := scan_str(`
	proc main
		;this is a comment
		i32.load r0 -1
		i32.load r1 2
		i32.add r0 r1
		halt
	end
	`);

	expected := `
line: 2, col: 2, kind: 'Keyword_Proc', str: 'proc'
line: 2, col: 7, kind: 'ID', str: 'main'
line: 3, col: 3, kind: 'Comment', str: 'this is a comment'
line: 4, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 4, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 4, col: 15, kind: 'Const_Integer', str: '-1'
line: 5, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 5, col: 12, kind: 'Keyword_R1', str: 'r1'
line: 5, col: 15, kind: 'Const_Integer', str: '2'
line: 6, col: 3, kind: 'Keyword_I32_Add', str: 'i32.add'
line: 6, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 6, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 7, col: 3, kind: 'Keyword_Halt', str: 'halt'
line: 8, col: 2, kind: 'Keyword_End', str: 'end'
`;

	assert(answer == expected);
}

scan_arithmetic_program :: proc() {
	answer := scan_str(`
	proc main
		i32.load r0 2
		i32.load r1 4
		i32.add r0 r1

		i32.load r0 2
		i32.load r1 4
		i32.sub r0 r1

		i32.load r0 2
		i32.load r1 4
		i32.mul r0 r1
		u32.mul r0 r1

		i32.load r0 2
		i32.load r1 4
		i32.div r0 r1
		u32.div r0 r1
		halt
	end
	`);

	expected := `
line: 2, col: 2, kind: 'Keyword_Proc', str: 'proc'
line: 2, col: 7, kind: 'ID', str: 'main'
line: 3, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 3, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 3, col: 15, kind: 'Const_Integer', str: '2'
line: 4, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 4, col: 12, kind: 'Keyword_R1', str: 'r1'
line: 4, col: 15, kind: 'Const_Integer', str: '4'
line: 5, col: 3, kind: 'Keyword_I32_Add', str: 'i32.add'
line: 5, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 5, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 7, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 7, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 7, col: 15, kind: 'Const_Integer', str: '2'
line: 8, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 8, col: 12, kind: 'Keyword_R1', str: 'r1'
line: 8, col: 15, kind: 'Const_Integer', str: '4'
line: 9, col: 3, kind: 'Keyword_I32_Sub', str: 'i32.sub'
line: 9, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 9, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 11, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 11, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 11, col: 15, kind: 'Const_Integer', str: '2'
line: 12, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 12, col: 12, kind: 'Keyword_R1', str: 'r1'
line: 12, col: 15, kind: 'Const_Integer', str: '4'
line: 13, col: 3, kind: 'Keyword_I32_Mul', str: 'i32.mul'
line: 13, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 13, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 14, col: 3, kind: 'Keyword_U32_Mul', str: 'u32.mul'
line: 14, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 14, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 16, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 16, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 16, col: 15, kind: 'Const_Integer', str: '2'
line: 17, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 17, col: 12, kind: 'Keyword_R1', str: 'r1'
line: 17, col: 15, kind: 'Const_Integer', str: '4'
line: 18, col: 3, kind: 'Keyword_I32_Div', str: 'i32.div'
line: 18, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 18, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 19, col: 3, kind: 'Keyword_U32_Div', str: 'u32.div'
line: 19, col: 11, kind: 'Keyword_R0', str: 'r0'
line: 19, col: 14, kind: 'Keyword_R1', str: 'r1'
line: 20, col: 3, kind: 'Keyword_Halt', str: 'halt'
line: 21, col: 2, kind: 'Keyword_End', str: 'end'
`;

	assert(answer == expected);
}

scan_jump_program :: proc() {
	answer := scan_str(`
	proc main
		i32.load r2 -2
		i32.load r1 0
		i32.jl r2 r1 negative
		jmp maybe_positive

	negative:
		i32.load r0 -1
		jmp exit

	maybe_positive:
		i32.jg r2 r1 positive
		i32.load r0 0
		jmp exit

	positive:
		i32.load r0 1

	exit:
		halt
	end
	`);

	expected := `
line: 2, col: 2, kind: 'Keyword_Proc', str: 'proc'
line: 2, col: 7, kind: 'ID', str: 'main'
line: 3, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 3, col: 12, kind: 'Keyword_R2', str: 'r2'
line: 3, col: 15, kind: 'Const_Integer', str: '-2'
line: 4, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 4, col: 12, kind: 'Keyword_R1', str: 'r1'
line: 4, col: 15, kind: 'Const_Integer', str: '0'
line: 5, col: 3, kind: 'Keyword_I32_Jl', str: 'i32.jl'
line: 5, col: 10, kind: 'Keyword_R2', str: 'r2'
line: 5, col: 13, kind: 'Keyword_R1', str: 'r1'
line: 5, col: 16, kind: 'ID', str: 'negative'
line: 6, col: 3, kind: 'Keyword_Jmp', str: 'jmp'
line: 6, col: 7, kind: 'ID', str: 'maybe_positive'
line: 8, col: 2, kind: 'ID', str: 'negative'
line: 8, col: 10, kind: 'Colon', str: ':'
line: 9, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 9, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 9, col: 15, kind: 'Const_Integer', str: '-1'
line: 10, col: 3, kind: 'Keyword_Jmp', str: 'jmp'
line: 10, col: 7, kind: 'ID', str: 'exit'
line: 12, col: 2, kind: 'ID', str: 'maybe_positive'
line: 12, col: 16, kind: 'Colon', str: ':'
line: 13, col: 3, kind: 'Keyword_I32_Jg', str: 'i32.jg'
line: 13, col: 10, kind: 'Keyword_R2', str: 'r2'
line: 13, col: 13, kind: 'Keyword_R1', str: 'r1'
line: 13, col: 16, kind: 'ID', str: 'positive'
line: 14, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 14, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 14, col: 15, kind: 'Const_Integer', str: '0'
line: 15, col: 3, kind: 'Keyword_Jmp', str: 'jmp'
line: 15, col: 7, kind: 'ID', str: 'exit'
line: 17, col: 2, kind: 'ID', str: 'positive'
line: 17, col: 10, kind: 'Colon', str: ':'
line: 18, col: 3, kind: 'Keyword_I32_Load', str: 'i32.load'
line: 18, col: 12, kind: 'Keyword_R0', str: 'r0'
line: 18, col: 15, kind: 'Const_Integer', str: '1'
line: 20, col: 2, kind: 'ID', str: 'exit'
line: 20, col: 6, kind: 'Colon', str: ':'
line: 21, col: 3, kind: 'Keyword_Halt', str: 'halt'
line: 22, col: 2, kind: 'Keyword_End', str: 'end'
`;

	assert(answer == expected);
}

scan_constant_program :: proc() {
	answer := scan_str(`
	constant msg "Hello, World!\n\0"
	`);

	expected := `
line: 2, col: 2, kind: 'Keyword_Constant', str: 'constant'
line: 2, col: 11, kind: 'ID', str: 'msg'
line: 2, col: 15, kind: 'Const_String', str: 'Hello, World!\n\0'
`;

	assert(answer == expected);
}

scan_c_proc_program :: proc() {
	answer := scan_str(`
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32)

	proc main
		halt
	end
	`);

	expected := `
line: 2, col: 2, kind: 'Keyword_Constant', str: 'constant'
line: 2, col: 11, kind: 'ID', str: 'msg'
line: 2, col: 15, kind: 'Const_String', str: 'Hello, World!\0'
line: 4, col: 2, kind: 'Keyword_Proc', str: 'proc'
line: 4, col: 7, kind: 'ID', str: 'C.add'
line: 4, col: 12, kind: 'Open_Paren', str: '('
line: 4, col: 13, kind: 'Keyword_C_Int32', str: 'C.int32'
line: 4, col: 20, kind: 'Comma', str: ','
line: 4, col: 22, kind: 'Keyword_C_Int32', str: 'C.int32'
line: 4, col: 29, kind: 'Close_Paren', str: ')'
line: 6, col: 2, kind: 'Keyword_Proc', str: 'proc'
line: 6, col: 7, kind: 'ID', str: 'main'
line: 7, col: 3, kind: 'Keyword_Halt', str: 'halt'
line: 8, col: 2, kind: 'Keyword_End', str: 'end'
`;

	assert(answer == expected);
}

parse_str :: proc(code: string) -> string {
	src := zas.src_from_string(code);
	defer zas.src_delete(&src);

	assert(zas.src_scan(&src));

	answer: string;
	if zas.src_parse(&src) {
		answer = zas.src_decls_dump(&src);
	} else {
		answer = zas.src_errs_dump(&src);
	}

	when os.OS == "windows" {
		res, _ := strings.replace_all(fmt.tprintf("\n%v", answer), "\n", "\r\n");
		return res;
	} else {
		return fmt.tprintf("\n%v", answer);
	}
}

parse_c_proc_program :: proc() {
	answer := parse_str(`
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32) C.int32

	proc main
		halt
	end
	`);

	expected := `
constant msg "Hello, World!\0"
proc C.add(C.int32, C.int32) C.int32
proc main
  halt
end
`;

	assert(answer == expected);
}

parse_c_proc_extra_comma_program :: proc() {
	answer := parse_str(`
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32,) C.int32

	proc main
		halt
	end
	`);

	expected := `
constant msg "Hello, World!\0"
proc C.add(C.int32, C.int32) C.int32
proc main
  halt
end
`;

	assert(answer == expected);
}

parse_c_proc_wrong_id_program :: proc() {
	answer := parse_str(`
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32, wrong) C.int32

	proc main
		halt
	end
	`);

	expected := `
>>	proc C.add(C.int32, C.int32, wrong) C.int32
>>	                             ^^^^^         
Error[<STRING>:4:31]: expected a type token
`;

	assert(answer == expected);
}

parse_imm_arithmetic_program :: proc() {
	answer := parse_str(`
	proc main
		i32.load r0 2
		i32.add r0 4

		i32.load r0 2
		i32.sub r0 4

		i32.load r0 2
		i32.mul r0 -4
		u32.mul r0 4

		i32.load r0 2
		i32.div r0 -4
		u32.div r0 4
		halt
	end
	`);

	expected := `
proc main
  i32.load r0 2
  i32.add r0 4
  i32.load r0 2
  i32.sub r0 4
  i32.load r0 2
  i32.mul r0 -4
  u32.mul r0 4
  i32.load r0 2
  i32.div r0 -4
  u32.div r0 4
  halt
end
`;

	assert(answer == expected);
}

parse_explicit_cmp_program :: proc() {
	answer := parse_str(`
	proc main
		; this is program to check the sign of the number in r2
		i32.load r2 -2
		i32.cmp r2 0
		jl negative
		jg positive
		je zero

	negative:
		i32.load r0 -1
		jmp exit

	positive:
		i32.load r0 1
		jmp exit

	zero:
		i32.load r0 0

	exit:
		halt
	end
	`);

	expected := `
proc main
  i32.load r2 -2
  i32.cmp r2 0
  jl negative
  jg positive
  je zero
negative:
  i32.load r0 -1
  jmp exit
positive:
  i32.load r0 1
  jmp exit
zero:
  i32.load r0 0
exit:
  halt
end
`;

	assert(answer == expected);
}

parse_imm_jump_program :: proc() {
	answer := parse_str(`
	proc main
		; this is program to check the sign of the number in r2
		i32.load r2 -2
		i32.jl r2 0 negative ;jump if less r2 < 0 to negative label
		jmp maybe_positive

	negative:
		i32.load r0 -1
		jmp exit

	maybe_positive:
		i32.jg r2 0 positive
		i32.load r0 0
		jmp exit

	positive:
		i32.load r0 1

	exit:
		halt
	end
	`);

	expected := `
proc main
  i32.load r2 -2
  i32.jl r2 0 negative
  jmp maybe_positive
negative:
  i32.load r0 -1
  jmp exit
maybe_positive:
  i32.jg r2 0 positive
  i32.load r0 0
  jmp exit
positive:
  i32.load r0 1
exit:
  halt
end
`;

	assert(answer == expected);
}

parse_add_program :: proc() {
	answer := parse_str(`
	proc main
		i32.load r0 -1
		i32.load r1 2
		i32.add r0 r1
		halt
	end
	`);

	expected := `
proc main
  i32.load r0 -1
  i32.load r1 2
  i32.add r0 r1
  halt
end
`;

	assert(answer == expected);
}

parse_arithmetic_program :: proc() {
	answer := parse_str(`
	proc main
		i32.load r0 2
		i32.load r1 4
		i32.add r0 r1

		i32.load r0 2
		i32.load r1 4
		i32.sub r0 r1

		i32.load r0 2
		i32.load r1 4
		i32.mul r0 r1
		u32.mul r0 r1

		i32.load r0 2
		i32.load r1 4
		i32.div r0 r1
		u32.div r0 r1
		halt
	end
	`);

	expected := `
proc main
  i32.load r0 2
  i32.load r1 4
  i32.add r0 r1
  i32.load r0 2
  i32.load r1 4
  i32.sub r0 r1
  i32.load r0 2
  i32.load r1 4
  i32.mul r0 r1
  u32.mul r0 r1
  i32.load r0 2
  i32.load r1 4
  i32.div r0 r1
  u32.div r0 r1
  halt
end
`;

	assert(answer == expected);
}

parse_jump_program :: proc() {
	answer := parse_str(`
	proc main
		i32.load r2 -2
		i32.load r1 0
		i32.jl r2 r1 negative
		jmp maybe_positive

	negative:
		i32.load r0 -1
		jmp exit

	maybe_positive:
		i32.jg r2 r1 positive
		i32.load r0 0
		jmp exit

	positive:
		i32.load r0 1

	exit:
		halt
	end
	`);

	expected := `
proc main
  i32.load r2 -2
  i32.load r1 0
  i32.jl r2 r1 negative
  jmp maybe_positive
negative:
  i32.load r0 -1
  jmp exit
maybe_positive:
  i32.jg r2 r1 positive
  i32.load r0 0
  jmp exit
positive:
  i32.load r0 1
exit:
  halt
end
`;

	assert(answer == expected);
}

parse_mem_read_write_program :: proc() {
	answer := parse_str(`
	proc main
		i32.load r0 1
		i32.load r1 2

		i32.sub sp 4 ; allocate i32
		i32.write sp r0

		i32.sub sp 4 ; allocate i32
		i32.write sp r1

		i32.load r0 3
		i32.load r1 4

		i32.read r1 sp
		i32.add sp 4 ; free i32

		i32.read r0 sp
		i32.add sp 4 ; free i32

		halt
	end
	`);

	expected := `
proc main
  i32.load r0 1
  i32.load r1 2
  i32.sub sp 4
  i32.write sp r0
  i32.sub sp 4
  i32.write sp r1
  i32.load r0 3
  i32.load r1 4
  i32.read r1 sp
  i32.add sp 4
  i32.read r0 sp
  i32.add sp 4
  halt
end
`;

	assert(answer == expected);
}

main :: proc() {
	scan_add_program();
	scan_arithmetic_program();
	scan_jump_program();
	scan_constant_program();
	scan_c_proc_program();
	parse_c_proc_program();
	parse_c_proc_extra_comma_program();
	parse_c_proc_wrong_id_program();
	parse_imm_arithmetic_program();
	parse_explicit_cmp_program();
	parse_imm_jump_program();
	parse_add_program();
	parse_arithmetic_program();
	parse_jump_program();
	parse_mem_read_write_program();
}