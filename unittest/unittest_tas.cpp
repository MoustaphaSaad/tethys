#include <doctest/doctest.h>

#include <as/Src.h>
#include <as/Scan.h>
#include <as/Parse.h>

#include <mn/Defer.h>
#include <mn/IO.h>
#include <mn/Path.h>


// scanning tests

inline static mn::Str
scan_str(const char* str)
{
	auto unit = as::src_from_str(str);
	mn_defer(as::src_free(unit));

	if(as::scan(unit) == false)
		return mn::str_tmpf("\n{}", as::src_errs_dump(unit, mn::memory::tmp()));
	else
		return mn::str_tmpf("\n{}", as::src_tkns_dump(unit, mn::memory::tmp()));
}


TEST_CASE("scan: simple add program")
{
	auto answer = scan_str(R"""(
	proc main
		;this is a comment
		i32.mov r0 -1
		i32.mov r1 2
		i32.add r0 r1
		halt
	end
	)""");

	const char* expected =R"""(
line: 2, col: 2, kind: "PROC" str: "proc"
line: 2, col: 7, kind: "<ID>" str: "main"
line: 3, col: 3, kind: "<COMMENT>" str: "this is a comment"
line: 4, col: 3, kind: "i32.mov" str: "i32.mov"
line: 4, col: 11, kind: "R0" str: "r0"
line: 4, col: 14, kind: "<INTEGER>" str: "-1"
line: 5, col: 3, kind: "i32.mov" str: "i32.mov"
line: 5, col: 11, kind: "R1" str: "r1"
line: 5, col: 14, kind: "<INTEGER>" str: "2"
line: 6, col: 3, kind: "i32.add" str: "i32.add"
line: 6, col: 11, kind: "R0" str: "r0"
line: 6, col: 14, kind: "R1" str: "r1"
line: 7, col: 3, kind: "HALT" str: "halt"
line: 8, col: 2, kind: "END" str: "end"
)""";

	CHECK(answer == expected);
}

TEST_CASE("scan: simple arithmetic program")
{
	auto answer = scan_str(R"""(
	proc main
		i32.mov r0 2
		i32.mov r1 4
		i32.add r0 r1

		i32.mov r0 2
		i32.mov r1 4
		i32.sub r0 r1

		i32.mov r0 2
		i32.mov r1 4
		i32.mul r0 r1
		u32.mul r0 r1

		i32.mov r0 2
		i32.mov r1 4
		i32.div r0 r1
		u32.div r0 r1
		halt
	end
	)""");

	const char* expected =R"""(
line: 2, col: 2, kind: "PROC" str: "proc"
line: 2, col: 7, kind: "<ID>" str: "main"
line: 3, col: 3, kind: "i32.mov" str: "i32.mov"
line: 3, col: 11, kind: "R0" str: "r0"
line: 3, col: 14, kind: "<INTEGER>" str: "2"
line: 4, col: 3, kind: "i32.mov" str: "i32.mov"
line: 4, col: 11, kind: "R1" str: "r1"
line: 4, col: 14, kind: "<INTEGER>" str: "4"
line: 5, col: 3, kind: "i32.add" str: "i32.add"
line: 5, col: 11, kind: "R0" str: "r0"
line: 5, col: 14, kind: "R1" str: "r1"
line: 7, col: 3, kind: "i32.mov" str: "i32.mov"
line: 7, col: 11, kind: "R0" str: "r0"
line: 7, col: 14, kind: "<INTEGER>" str: "2"
line: 8, col: 3, kind: "i32.mov" str: "i32.mov"
line: 8, col: 11, kind: "R1" str: "r1"
line: 8, col: 14, kind: "<INTEGER>" str: "4"
line: 9, col: 3, kind: "i32.sub" str: "i32.sub"
line: 9, col: 11, kind: "R0" str: "r0"
line: 9, col: 14, kind: "R1" str: "r1"
line: 11, col: 3, kind: "i32.mov" str: "i32.mov"
line: 11, col: 11, kind: "R0" str: "r0"
line: 11, col: 14, kind: "<INTEGER>" str: "2"
line: 12, col: 3, kind: "i32.mov" str: "i32.mov"
line: 12, col: 11, kind: "R1" str: "r1"
line: 12, col: 14, kind: "<INTEGER>" str: "4"
line: 13, col: 3, kind: "i32.mul" str: "i32.mul"
line: 13, col: 11, kind: "R0" str: "r0"
line: 13, col: 14, kind: "R1" str: "r1"
line: 14, col: 3, kind: "u32.mul" str: "u32.mul"
line: 14, col: 11, kind: "R0" str: "r0"
line: 14, col: 14, kind: "R1" str: "r1"
line: 16, col: 3, kind: "i32.mov" str: "i32.mov"
line: 16, col: 11, kind: "R0" str: "r0"
line: 16, col: 14, kind: "<INTEGER>" str: "2"
line: 17, col: 3, kind: "i32.mov" str: "i32.mov"
line: 17, col: 11, kind: "R1" str: "r1"
line: 17, col: 14, kind: "<INTEGER>" str: "4"
line: 18, col: 3, kind: "i32.div" str: "i32.div"
line: 18, col: 11, kind: "R0" str: "r0"
line: 18, col: 14, kind: "R1" str: "r1"
line: 19, col: 3, kind: "u32.div" str: "u32.div"
line: 19, col: 11, kind: "R0" str: "r0"
line: 19, col: 14, kind: "R1" str: "r1"
line: 20, col: 3, kind: "HALT" str: "halt"
line: 21, col: 2, kind: "END" str: "end"
)""";

	CHECK(answer == expected);
}

TEST_CASE("scan: simple jump program")
{
	auto answer = scan_str(R"""(
	proc main
		i32.mov r2 -2
		i32.mov r1 0
		i32.jl r2 r1 negative
		jmp maybe_positive

	negative:
		i32.mov r0 -1
		jmp exit

	maybe_positive:
		i32.jg r2 r1 positive
		i32.mov r0 0
		jmp exit

	positive:
		i32.mov r0 1

	exit:
		halt
	end
	)""");

	const char* expected =R"""(
line: 2, col: 2, kind: "PROC" str: "proc"
line: 2, col: 7, kind: "<ID>" str: "main"
line: 3, col: 3, kind: "i32.mov" str: "i32.mov"
line: 3, col: 11, kind: "R2" str: "r2"
line: 3, col: 14, kind: "<INTEGER>" str: "-2"
line: 4, col: 3, kind: "i32.mov" str: "i32.mov"
line: 4, col: 11, kind: "R1" str: "r1"
line: 4, col: 14, kind: "<INTEGER>" str: "0"
line: 5, col: 3, kind: "i32.jl" str: "i32.jl"
line: 5, col: 10, kind: "R2" str: "r2"
line: 5, col: 13, kind: "R1" str: "r1"
line: 5, col: 16, kind: "<ID>" str: "negative"
line: 6, col: 3, kind: "jmp" str: "jmp"
line: 6, col: 7, kind: "<ID>" str: "maybe_positive"
line: 8, col: 2, kind: "<ID>" str: "negative"
line: 8, col: 10, kind: ":" str: ":"
line: 9, col: 3, kind: "i32.mov" str: "i32.mov"
line: 9, col: 11, kind: "R0" str: "r0"
line: 9, col: 14, kind: "<INTEGER>" str: "-1"
line: 10, col: 3, kind: "jmp" str: "jmp"
line: 10, col: 7, kind: "<ID>" str: "exit"
line: 12, col: 2, kind: "<ID>" str: "maybe_positive"
line: 12, col: 16, kind: ":" str: ":"
line: 13, col: 3, kind: "i32.jg" str: "i32.jg"
line: 13, col: 10, kind: "R2" str: "r2"
line: 13, col: 13, kind: "R1" str: "r1"
line: 13, col: 16, kind: "<ID>" str: "positive"
line: 14, col: 3, kind: "i32.mov" str: "i32.mov"
line: 14, col: 11, kind: "R0" str: "r0"
line: 14, col: 14, kind: "<INTEGER>" str: "0"
line: 15, col: 3, kind: "jmp" str: "jmp"
line: 15, col: 7, kind: "<ID>" str: "exit"
line: 17, col: 2, kind: "<ID>" str: "positive"
line: 17, col: 10, kind: ":" str: ":"
line: 18, col: 3, kind: "i32.mov" str: "i32.mov"
line: 18, col: 11, kind: "R0" str: "r0"
line: 18, col: 14, kind: "<INTEGER>" str: "1"
line: 20, col: 2, kind: "<ID>" str: "exit"
line: 20, col: 6, kind: ":" str: ":"
line: 21, col: 3, kind: "HALT" str: "halt"
line: 22, col: 2, kind: "END" str: "end"
)""";

	CHECK(answer == expected);
}

TEST_CASE("scan: constant")
{
	auto answer = scan_str(R"""(
	constant msg "Hello, World!\n\0"
	)""");

	const char* expected =R"""(
line: 2, col: 2, kind: "CONSTANT" str: "constant"
line: 2, col: 11, kind: "<ID>" str: "msg"
line: 2, col: 15, kind: "<STRING>" str: "Hello, World!\n\0"
)""";

	CHECK(answer == expected);
}

TEST_CASE("scan: c proc")
{
	auto answer = scan_str(R"""(
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32)

	proc main
		halt
	end
	)""");

	const char* expected =R"""(
line: 2, col: 2, kind: "CONSTANT" str: "constant"
line: 2, col: 11, kind: "<ID>" str: "msg"
line: 2, col: 15, kind: "<STRING>" str: "Hello, World!\0"
line: 4, col: 2, kind: "PROC" str: "proc"
line: 4, col: 7, kind: "<ID>" str: "C.add"
line: 4, col: 12, kind: "(" str: "("
line: 4, col: 13, kind: "C.int32" str: "C.int32"
line: 4, col: 20, kind: "," str: ","
line: 4, col: 22, kind: "C.int32" str: "C.int32"
line: 4, col: 29, kind: ")" str: ")"
line: 6, col: 2, kind: "PROC" str: "proc"
line: 6, col: 7, kind: "<ID>" str: "main"
line: 7, col: 3, kind: "HALT" str: "halt"
line: 8, col: 2, kind: "END" str: "end"
)""";

	CHECK(answer == expected);
}


// parsing tests
inline static mn::Str
parse_str(const char* str)
{
	auto unit = as::src_from_str(str);
	mn_defer(as::src_free(unit));

	REQUIRE(as::scan(unit));

	if(as::parse(unit) == false)
		return mn::str_tmpf("\n{}", as::src_errs_dump(unit, mn::memory::tmp()));
	else
		return mn::str_tmpf("\n{}", as::decl_dump(unit, mn::memory::tmp()));
}

TEST_CASE("parse: c proc")
{
	auto answer = parse_str(R"""(
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32) C.int32

	proc main
		halt
	end
	)""");

	const char* expected =R"""(
constant msg "Hello, World!\0"
proc C.add(C.int32, C.int32) C.int32
PROC main
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: c proc extra comma")
{
	auto answer = parse_str(R"""(
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32,) C.int32

	proc main
		halt
	end
	)""");

	const char* expected =R"""(
constant msg "Hello, World!\0"
proc C.add(C.int32, C.int32) C.int32
PROC main
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: c proc wrong id")
{
	auto answer = parse_str(R"""(
	constant msg "Hello, World!\0"

	proc C.add(C.int32, C.int32, wrong) C.void

	proc main
		halt
	end
	)""");

	const char* expected =R"""(
>> 	proc C.add(C.int32, C.int32, wrong) C.void
>> 	                             ^^^^^        
Error[<STRING>:4:31]: expected a type token
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: constant debugstr")
{
	auto answer = parse_str(R"""(
	constant msg "Hello, World!\0"

	proc main
		u64.mov r1 msg
		halt
	end
	)""");

	const char* expected =R"""(
constant msg "Hello, World!\0"
PROC main
  u64.mov r1 msg
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: imm arithmetic")
{
	auto answer = parse_str(R"""(
	proc main
		i32.mov r0 2
		i32.add r0 4

		i32.mov r0 2
		i32.sub r0 4

		i32.mov r0 2
		i32.mul r0 -4
		u32.mul r0 4

		i32.mov r0 2
		i32.div r0 -4
		u32.div r0 4
		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r0 2
  i32.add r0 4
  i32.mov r0 2
  i32.sub r0 4
  i32.mov r0 2
  i32.mul r0 -4
  u32.mul r0 4
  i32.mov r0 2
  i32.div r0 -4
  u32.div r0 4
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: explicit cmp")
{
	auto answer = parse_str(R"""(
	proc main
		; this is program to check the sign of the number in r2
		i32.mov r2 -2
		i32.cmp r2 0
		jl negative
		jg positive
		je zero

	negative:
		i32.mov r0 -1
		jmp exit

	positive:
		i32.mov r0 1
		jmp exit

	zero:
		i32.mov r0 0

	exit:
		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r2 -2
  i32.cmp r2 0
  jl negative
  jg positive
  je zero
negative:
  i32.mov r0 -1
  jmp exit
positive:
  i32.mov r0 1
  jmp exit
zero:
  i32.mov r0 0
exit:
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: imm jump")
{
	auto answer = parse_str(R"""(
	proc main
		; this is program to check the sign of the number in r2
		i32.mov r2 -2
		i32.jl r2 0 negative ;jump if less r2 < 0 to negative label
		jmp maybe_positive

	negative:
		i32.mov r0 -1
		jmp exit

	maybe_positive:
		i32.jg r2 0 positive
		i32.mov r0 0
		jmp exit

	positive:
		i32.mov r0 1

	exit:
		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r2 -2
  i32.jl r2 0 negative
  jmp maybe_positive
negative:
  i32.mov r0 -1
  jmp exit
maybe_positive:
  i32.jg r2 0 positive
  i32.mov r0 0
  jmp exit
positive:
  i32.mov r0 1
exit:
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: simple add")
{
	auto answer = parse_str(R"""(
	proc main
		i32.mov r0 -1
		i32.mov r1 2
		i32.add r0 r1
		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r0 -1
  i32.mov r1 2
  i32.add r0 r1
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: simple arithmetic")
{
	auto answer = parse_str(R"""(
	proc main
		i32.mov r0 2
		i32.mov r1 4
		i32.add r0 r1

		i32.mov r0 2
		i32.mov r1 4
		i32.sub r0 r1

		i32.mov r0 2
		i32.mov r1 4
		i32.mul r0 r1
		u32.mul r0 r1

		i32.mov r0 2
		i32.mov r1 4
		i32.div r0 r1
		u32.div r0 r1
		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r0 2
  i32.mov r1 4
  i32.add r0 r1
  i32.mov r0 2
  i32.mov r1 4
  i32.sub r0 r1
  i32.mov r0 2
  i32.mov r1 4
  i32.mul r0 r1
  u32.mul r0 r1
  i32.mov r0 2
  i32.mov r1 4
  i32.div r0 r1
  u32.div r0 r1
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: simple jump")
{
	auto answer = parse_str(R"""(
	proc main
		i32.mov r2 -2
		i32.mov r1 0
		i32.jl r2 r1 negative
		jmp maybe_positive

	negative:
		i32.mov r0 -1
		jmp exit

	maybe_positive:
		i32.jg r2 r1 positive
		i32.mov r0 0
		jmp exit

	positive:
		i32.mov r0 1

	exit:
		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r2 -2
  i32.mov r1 0
  i32.jl r2 r1 negative
  jmp maybe_positive
negative:
  i32.mov r0 -1
  jmp exit
maybe_positive:
  i32.jg r2 r1 positive
  i32.mov r0 0
  jmp exit
positive:
  i32.mov r0 1
exit:
  halt
END
)""";

	CHECK(answer == expected);
}

TEST_CASE("parse: mem read write")
{
	auto answer = parse_str(R"""(
	proc main
		i32.mov r0 1
		i32.mov r1 2

		i32.sub sp 4 ; allocate i32
		i32.mov [sp] r0

		i32.sub sp 4 ; allocate i32
		i32.mov [sp] r1

		i32.mov r0 3
		i32.mov r1 4

		i32.mov r1 [sp]
		i32.add sp 4 ; free i32

		i32.mov r0 [sp]
		i32.add sp 4 ; free i32

		halt
	end
	)""");

	const char* expected =R"""(
PROC main
  i32.mov r0 1
  i32.mov r1 2
  i32.sub sp 4
  i32.mov [sp] r0
  i32.sub sp 4
  i32.mov [sp] r1
  i32.mov r0 3
  i32.mov r1 4
  i32.mov r1 [sp]
  i32.add sp 4
  i32.mov r0 [sp]
  i32.add sp 4
  halt
END
)""";

	CHECK(answer == expected);
}
