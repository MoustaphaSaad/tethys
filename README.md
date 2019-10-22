# tethys
i just do things
i want to build the mountain, and it's name is tethys

## Virtual Machine
We'll build a register based virtual machine

### Day-0
I did the initial setup of the repo with a simple hello world program

### Day-1
Let's start building our virtual machine by specifying our registers
We'll have 8 general purpose registers R0 to R7, also we'll have an instruction pointer register which makes the register total be 9
```C++
enum Reg: uint8_t
{
	// General purpose registers
	Reg_R0,
	Reg_R1,
	Reg_R2,
	Reg_R3,
	Reg_R4,
	Reg_R5,
	Reg_R6,
	Reg_R7,

	// instruction pointer
	Reg_IP,

	//Count of the registers
	Reg_COUNT
};
```
This is how we refer to registers, then we need to specify register data type
our registers are 64-bit wide, but we'll need to refer to 8-bit, 16-bit, and 32-bit parts of each register, also we might need to access these registers as signed integers or unsigned integers, given all this input it follows that our register values will be
```C++
union Reg_Val
{
	int8_t   i8;
	int16_t  i16;
	int32_t  i32;
	int64_t  i64;
	uint8_t  u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
};
```
Let's now talk about our opcodes or instructions, let's start with 3 opcodes
- Halt: stops the execution with success
- Load: loads a constant value into a register
- Add: adds two registers together
Given that we can address each part of the given registers we'll need 8-bit, 16-bit, 32-bit, 64-bit variant of each instruction which interacts with registers
```C++
enum Op: uint8_t
{
	// illegal opcode
	Op_IGL,

	// LOAD [dst] [constant]
	Op_LOAD8,
	Op_LOAD16,
	Op_LOAD32,
	Op_LOAD64,

	// ADD [dst + op1] [op2]
	Op_ADD8,
	Op_ADD16,
	Op_ADD32,
	Op_ADD64,

	Op_HALT,
};
```
Now that we have our registers defined and our instructions defined we'll need to execute them, let's define our execution context or a cpu core, a core consists of the state of all registers + the state of the execution itself
```C++
struct Core
{
	enum STATE
	{
		STATE_OK,	// Ok state, please continue executing instructions
		STATE_HALT,	// execution succeeded, stop executing instructions
		STATE_ERR	// error happened, stop executing instructions
	};

	STATE state;
	// array of core registers
	Reg_Val r[Reg_COUNT];
};
```
the only missing part now is that we need to define how we'll represent our instructions, as you should notice by now that i declare each enum to be uint8_t for a reason, our bytecode will be an array of bytes `mn::Buf<uint8_t>`
now let's write the execution function
```C++
void
core_ins_execute(Core& self, const mn::Buf<uint8_t>& code)
{
	// first we read the current opcode at the current IP
	auto op = pop_op(self, code);
	// we decode it using this switch statement
	switch(op)
	{
	...
	case Op_LOAD32:
	{
		// in case of load32 instruction we read the register index from the bytecode
		// and load the register from the core
		auto& dst = load_reg(self, code);
		// then we read 32-bit value from bytecode and put it in the 32-bit slot in the register
		dst.u32 = pop32(code, self.r[Reg_IP].u64);
		break;
	}
	...
	case Op_ADD32:
	{
		// we read the register index from the bytecode and load the register from the core
		auto& dst = load_reg(self, code);
		// same with src register
		auto& src = load_reg(self, code);
		// we apply the add instruction and put the result in dst register
		dst.u32 += src.u32;
		break;
	}
	...
	case Op_HALT:
		// in case of halt instruction we change the state of this core to halt
		self.state = Core::STATE_HALT;
		break;
	case Op_IGL:
	default:
		// in case of illegal instruction or unhandled instruction we put the core in err state
		self.state = Core::STATE_ERR;
		break;
	}
}
```
Now we can play with this code in the playground and write a program that adds two numbers together
```C++
auto code = mn::buf_new<uint8_t>();

// LOAD32 R0 -1
vm::push8(code, vm::Op_LOAD32);
vm::push8(code, vm::Reg_R0);
vm::push32(code, uint32_t(-1));

// LOAD32 R1 2
vm::push8(code, vm::Op_LOAD32);
vm::push8(code, vm::Reg_R1);
vm::push32(code, 2);

// ADD32 R0 R1
vm::push8(code, vm::Op_ADD32);
vm::push8(code, vm::Reg_R0);
vm::push8(code, vm::Reg_R1);

// HALT
vm::push8(code, vm::Op_HALT);

// create a cpu core
auto cpu = vm::core_new();
// continue execution till we reach an err or halt
while (cpu.state == vm::Core::STATE_OK)
	vm::core_ins_execute(cpu, code);

// print the value inside the R0 register
mn::print("R0 = {}\n", cpu.r[vm::Reg_R0].i32);
```
And that's it for Day-01 we have a fully functioning Virtual machine, sadly it can only perform addition

### Day-2
Today we'll start making an assembler for our vm, after all we won't be put bytes together manually

this is our target, we want to make this code work
```C++
// create a src entity from a code string
auto src = as::src_from_str(R"""(
proc main
	i32.load r0 -1
	i32.load r1 2
	i32.add r0 r1
	halt
end
)""");
mn_defer(as::src_free(src));

// scan it and print the errors if they exist
if(as::scan(src) == false)
{
	mn::printerr("{}", as::src_errs_dump(src, mn::memory::tmp()));
	return;
}
// print the scanned tokens
mn::print("{}", as::src_tkns_dump(src, mn::memory::tmp()));
```

Let's see how we'd achieve that

First of all let's define some useful things
```C++
// Represents a position in source code
struct Pos
{
	uint32_t line, col;
};
// Represents a range of characters in the source code
struct Rng
{
	const char* begin;
	const char* end;
};
// A line is just a range of characters
typedef Rng Line;
```

then we can define errors to be
```C++
// Error representation is the location and the message
// we want to show at that location
struct Err
{
	Pos pos;
	Rng rng;
	mn::Str msg;
};
```

#### X Macro Enums
Let me introduce a new technique called "x macro enum" which is usually used to solve this problem
you have an enum and we want to declare it in two places with the exact same order let's say one in the enum definition itself and the other in some string mapping

```C++
// we declare the list to be a macro of itself with the weird X macro around each item
#define NUMBERS_LIST \
	X(ONE, "1"), \
	X(TWO, "2"), \
	X(THREE, "3"),

enum NUMBERS {
	// here we define the X macro to extract the name from the list
	#define X(ENUM_NAME, ENUM_STRING) ENUM_NAME
		NUMBERS_LIST
	#undef X
}

const char* NUMBERS_NAMES[] = {
	// here we define the X macro to extract the string from the list
	#define X(ENUM_NAME, ENUM_STRING) ENUM_STRING
		NUMBERS_LIST
	#undef X
}
```

This is useful because we'll want to do some kind of token list
```C++
// This is a list of the tokens
#define TOKEN_LISTING \
	TOKEN(NONE, "<NONE>"), \
	TOKEN(ID, "<ID>"), \
	TOKEN(INTEGER, "<INTEGER>"), \
	TOKEN(FLOAT, "<FLOAT>"), \
	TOKEN(KEYWORDS__BEGIN, ""), \
	TOKEN(KEYWORD_PROC, "PROC"), \
	TOKEN(KEYWORD_END, "END"), \
	TOKEN(KEYWORD_HALT, "HALT"), \
	...
	TOKEN(KEYWORD_I32_LOAD, "i32.load"), \
	...
	TOKEN(KEYWORD_I32_ADD, "i32.add"), \
	...
	TOKEN(KEYWORD_R0, "R0"), \
	TOKEN(KEYWORD_R1, "R1"), \
	...
	TOKEN(KEYWORDS__END, ""),
```

Now that we have token list, let's define the Tkn struct
```C++
// This is token representation
struct Tkn
{
	enum KIND
	{
		#define TOKEN(k, s) KIND_##k
			TOKEN_LISTING
		#undef TOKEN
	};

	inline static const char* NAMES[] = {
		#define TOKEN(k, s) s
			TOKEN_LISTING
		#undef TOKEN
	};

	KIND kind;
	const char* str;
	Rng rng;
	Pos pos;

	inline operator bool() const { return kind != KIND_NONE; }
};
```

#### Interning
Now let's introduce a new concept which is called interning, the basic premise is that you
have each unique value allocated once and you refer to it by the same pointer everywhere

this extremely useful in scanning and parsing since it will convert all the string compares to pointer compare

Let's say you have the following string list
```C++
const char* names[] = {"Mostafa", "Mostafa", "Saad", "Adam", "Adam"};
```
we'll need to internalize each value in some hash table if it's not there, if it's in the hash table we'll just return pointer to it in the hash table
```C++
mn::Map<mn::Str, bool> string_table;

const char* intern(const mn::Str& str)
{
	// check if it's there, then return a pointer to the string inside the hash table
	if(auto it = mn::map_lookup(string_table, str))
		return it->key.ptr;
	// if it doesn't exist then we add it and return pointer to it
	else
		return mn::map_insert(string_table, str, true)->key.ptr;
}
```
Now we don't need to do "strcmp" anymore since if two pointers are equal then their content is equal as well.

#### Scanning

Let's head back to scanning, our interface should be as simple as this
```C++
// given the source code this function scans it and returns true on success and false otherwise
bool scan(Src* src);
```

Let's implement that
```C++
auto scanner = scanner_new(src);
while(true)
{
	if(auto tkn = scanner_tkn(&scanner))
		src_tkn(src, tkn);
	else
		break;
}
```

The main workhorse here is the scanner_tkn which returns the next token each time it's called
Let's have a look at that
```C++
Tkn scanner_tkn(Scanner* self)
{
	// First skip whitespaces
	scanner_skip_whitespaces(self);

	// check that you are not at the end of file
	if(scanner_eof(self))
		return Tkn{};

	// init the location of the token
	Tkn tkn{};
	tkn.pos = self->pos;
	tkn.rng.begin = self->it;

	// if the current character is a letter
	if(is_letter(self->c))
	{
		// scan it as id at first
		tkn.kind = Tkn::KIND_ID;
		tkn.str = scanner_id(self);

		// let's loop over all the keywords and check if they equal id
		// so for example the keyword "proc" will be scanned as ID at first
		// and this loop will correct it to be a proc token and not an id
		for(size_t i = size_t(Tkn::KIND_KEYWORDS__BEGIN + 1);
			i < size_t(Tkn::KIND_KEYWORDS__END);
			++i)
		{
			// as usual assembly code is case insensitive
			if(case_insensitive_cmp(tkn.str, Tkn::NAMES[i]))
			{
				tkn.kind = Tkn::KIND(i);
				break;
			}
		}
	}
	// if the current character is a number then scan the number
	else if(is_digit(self->c))
	{
		scanner_num(self, tkn);
	}
	// if the current character is a sign and the next character is a number then scan the number
	else if(self->c == '-' || self->c == '+')
	{
		auto next = mn::rune_read(mn::rune_next(self->it));
		if(is_digit(next))
			scanner_num(self, tkn);
	}
	// not recognized character then this is probably an error
	else
	{
		src_err(self->src, self->pos, mn::strf("illegal character {}", self->c));
	}
	tkn.rng.end = self->it;
	return tkn;
}
```

Of course you can browse the pull request to see the details of each little function.

Now if we run the code at the start it will print the following:
```
line: 2, col: 1, kind: "PROC" str: "proc"
line: 2, col: 6, kind: "<ID>" str: "main"
line: 3, col: 2, kind: "i32.load" str: "i32.load"
line: 3, col: 11, kind: "R0" str: "r0"
line: 3, col: 14, kind: "<INTEGER>" str: "-1"
line: 4, col: 2, kind: "i32.load" str: "i32.load"
line: 4, col: 11, kind: "R1" str: "r1"
line: 4, col: 14, kind: "<INTEGER>" str: "2"
line: 5, col: 2, kind: "i32.add" str: "i32.add"
line: 5, col: 10, kind: "R0" str: "r0"
line: 5, col: 13, kind: "R1" str: "r1"
line: 6, col: 2, kind: "HALT" str: "halt"
line: 7, col: 1, kind: "END" str: "end"
```

### Day-3
Today's theme is testing, one of the most things i hate is adding a new feature and suddenly break old code

first of all let's add a cli interface to our assembler that can only scan files for now and print the scanned tokens
the user should write `tas scan path/to/file` to scan the file

#### Command Line Arguments
I use a simple scheme for command line arguments
`program.exe [command] [flags|OPTIONAL] [targets]`
commands tend to be things like, "scan", "parse", etc..., and obviously targets are the files we are running these commands on
currently we only have 2 commands, let's do the command line argument parsing

```C++
const char* HELP_MSG = R"MSG(tas tethys assembler
tas [command] [targets] [flags]
COMMANDS:
  help: prints this message
    'tas help'
  scan: scans the file
    'tas scan path/to/file.zy'
)MSG";

inline static void
print_help()
{
	mn::print("{}\n", HELP_MSG);
}

struct Args
{
	mn::Str command;
	mn::Buf<mn::Str> targets;
	mn::Buf<mn::Str> flags;
};

inline static void
args_parse(Args& self, int argc, char** argv)
{
	// if the user provides no argument then there's something
	if(argc < 2)
	{
		print_help();
		return;
	}

	// parse the command
	self.command = mn::str_from_c(argv[1]);
	for(size_t i = 2; i < size_t(argc); ++i)
	{
		// filter the flags which should start with '--' or '-'
		if(mn::str_prefix(argv[i], "--"))
			buf_push(self.flags, mn::str_from_c(argv[i] + 2));
		else if(mn::str_prefix(argv[i], "-"))
			buf_push(self.flags, mn::str_from_c(argv[i] + 1));
		// Otherwise this is a target
		else
			buf_push(self.targets, mn::str_from_c(argv[i]));
	}
}

// Check if a flag is set
inline static bool
args_has_flag(Args& self, const char* search)
{
	for(const mn::Str& f: self.flags)
		if(f == search)
			return true;
	return false;
}
```

Now that we can parse the command line arguments let's check our main function

```C++
if(args.command == "help")
{
	print_help();
	return 0;
}
else if(args.command == "scan")
{
	if(args.targets.count == 0)
	{
		mn::printerr("no input files\n");
		return -1;
	}
	else if(args.targets.count > 1)
	{
		mn::printerr("multiple input files are not supported yet\n");
		return -1;
	}

	if(mn::path_is_file(args.targets[0]) == false)
	{
		mn::printerr("'{}' is not a file \n", args.targets[0]);
		return -1;
	}

	auto src = as::src_from_file(args.targets[0].ptr);
	mn_defer(as::src_free(src));

	// Try to scan the file and print the errors on failure
	if(as::scan(src) == false)
	{
		mn::print("{}", as::src_errs_dump(src, mn::memory::tmp()));
		return -1;
	}

	// print tokens on success
	mn::print("{}", as::src_tkns_dump(src, mn::memory::tmp()));
	return 0;
}
```
TADA, now we have command line interface for our assembler, that we'll use to generate test cases

#### Unit tests
Now, let's automate the tests, our scheme is simple we'll put the scan test cases in a test/scan folder
each test case consists of a input file and an expected output file
```
- unittest
	- test
		- scan
			- case-01.in
			- case-01.out
			- case-02.in
			- case-03.out
		- parse
			- case-01.in
			- case-01.out
```
in the unittests we'll iterate over the files in scan folder and perform a scan action on them and compare the two outputs

Let's do the code
```C++
// Get the files in the test/scan folder
auto files = mn::path_entries(TEST_DIR, mn::memory::tmp());

// sort the files by name
std::sort(begin(files), end(files), [](const auto& a, const auto& b) { return a.name < b.name; });

// loop over the files
for(size_t i = 2; i < files.count; i += 2)
{
	// ignore folders
	if (files[i].kind == mn::Path_Entry::KIND_FOLDER)
		continue;

	// get the input and output
	auto input = mn::path_join(mn::str_tmp(), TEST_DIR, "scan", files[i].name);
	auto output = mn::path_join(mn::str_tmp(), TEST_DIR, "scan", files[i + 1].name);
	auto expected = file_content_normalized(output);
	auto answer = mn::str_tmp();

	// perform the scan
	auto unit = as::src_from_file(input.ptr);
	mn_defer(as::src_free(unit));

	if (as::scan(unit) == false)
		answer = as::src_errs_dump(unit, mn::memory::tmp());
	else
		answer = as::src_tkns_dump(unit, mn::memory::tmp());

	// compare the results
	if(expected != answer)
	{
		// print data on error
		mn::printerr("TEST CASE: input '{}', output '{}'\n", input, output);
		mn::printerr("EXPECTED\n{}\nFOUND\n{}", expected, answer);
	}
	CHECK(expected == answer);
}
```

Now it's time to generate our first test case, let's write the simple add program we did in the playground in a file and put in the "test/scan/simple_add.in" folder
```
proc main
	i32.load r0 -1
	i32.load r1 2
	i32.add r0 r1
	halt
end
```
then let's invoke the command line tool to get the output and save it to a file "test/scan/simple_add.out"
`tas scan test/scan/simple_add.in > test/scan/simple_add.out`
and if we run our unittest program it should check this test case, we can also do a case that generates an error

Now we can add more tests as we go and it will be as easy as writing the tests and what we expect and everything from now on is automated
