# tethys
I just do things.
I want to build the mountain, and its name is [tethys](https://en.wikipedia.org/wiki/Tethys_Ocean)

## Virtual Machine
We'll build a register based virtual machine.

When you think of a virtual machine, think of the JVM or V8. Do **not** think of VirtualBox or VMWare.

VirtualBox and VMWare virtualize all of the hardware of a computer to run a complete operating system.

Our Virtual Machine creates a virtual processor that runs a single program.

Because we're creating our own processor, we get to decide what the assembly/bytecode this processor can understand and execute. We're free to do whatever we want.

We're going to start completely from scratch and slowly build our mountain step by step. At the start of each day we branch from the master branch and after we're done working we merge the progress into the master. I don't delete branches after merging so you can find them here [ALL BRANCHES](https://github.com/MoustaphaSaad/tethys/branches/all)

We'll write our code in a [C-Style C++](https://moustaphasaad.github.io/Pure_Coding_Style.html).

We won't be using the C++ standard library because it's sort of garbage. Instead we'll be using [mn](https://github.com/MoustaphaSaad/mn).

If you have any questions, comments, suggestions or insults and curse words, please send me an Email: moustapha.saad.abdelhamed@gmail.com or open an issue.

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

### Day-4
Today we'll start building the parser, first let's define what's an instruction?
our instructions consists of `opcode dst src` this is the general structure of our assembly, this should be simple
```C++
struct Ins
{
	// opcode token
	Tkn op;
	Tkn dst;
	Tkn src;
};
```

Now a procedure is just a list of instructions
```C++
struct Proc
{
	// procedure name
	Tkn name;
	// procedure body
	mn::Buf<Ins> ins;
};
```

Now let's do the parsing, first let's parse a procedure
```C++
inline static Proc
parser_proc(Parser* self)
{
	// we must find a 'proc' keyword or we'll issue an error
	parser_eat_must(self, Tkn::KIND_KEYWORD_PROC);
	auto proc = proc_new();

	// then we must find the name of the proc or we'll issue an error
	proc.name = parser_eat_must(self, Tkn::KIND_ID);

	// we should loop until we found the 'end' keyword
	while (parser_look_kind(self, Tkn::KIND_KEYWORD_END) == false)
	{
		// parse a single instructions
		auto ins = parser_ins(self);
		if (ins.op)
			mn::buf_push(proc.ins, ins);
		else
			break;
	}

	// at the end we should be find the 'end' keyword
	parser_eat_kind(self, Tkn::KIND_KEYWORD_END);

	return proc;
}
```

Now parsing a instruction should be as simple as this
```C++
inline static Ins
parser_ins(Parser* self)
{
	Ins ins{};

	Tkn op = parser_look(self);
	if (is_load(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
		ins.src = parser_const(self);
	}
	else if (is_add(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
		ins.src = parser_reg(self);
	}
	else if(op.kind == Tkn::KIND_KEYWORD_HALT)
	{
		ins.op = parser_eat(self);
	}

	return ins;
}
```

and that's it for today

### Day-05
Instructions Assemble!

Today we'll start the bytecode generation pass of our assembler

we'll start with `proc_gen` function which generate the bytecode for an entire proc
```C++
mn::Buf<uint8_t>
proc_gen(const Proc& proc, mn::Allocator allocator)
{
	auto out = mn::buf_with_allocator<uint8_t>(allocator);

	for(const auto& ins: proc.ins)
		ins_gen(ins, out);

	return out;
}
```
Looks simple, in order to generate the entire proc you have to generate its instructions

now let's have a look at `ins_gen`
```C++
inline static void
ins_gen(const Ins& ins, mn::Buf<uint8_t>& out)
{
	switch(ins.op.kind)
	{
	...
	case Tkn::KIND_KEYWORD_I32_LOAD:
	{
		vm::push8(out, uint8_t(vm::Op_LOAD32));
		reg_gen(ins.dst, out);

		// convert the string value to int32_t
		int32_t c = 0;
		// reads returns the number of the parsed items
		size_t res = mn::reads(ins.src.str, c);
		// assert that we parsed the only item we have
		assert(res == 1);
		vm::push32(out, uint32_t(c));
		break;
	}

	...

	case Tkn::KIND_KEYWORD_I32_ADD:
	case Tkn::KIND_KEYWORD_U32_ADD:
		vm::push8(out, uint8_t(vm::Op_ADD32));
		reg_gen(ins.dst, out);
		reg_gen(ins.src, out);
		break;

	...

	case Tkn::KIND_KEYWORD_HALT:
		vm::push8(out, uint8_t(vm::Op_HALT));
		break;

	default:
		assert(false && "unreachable");
		vm::push8(out, uint8_t(vm::Op_IGL));
		break;
	}
}
```

and the final piece of the code is `reg_gen` which basically emits the correct byte for each register
```C++
inline static void
reg_gen(const Tkn& r, mn::Buf<uint8_t>& out)
{
	switch(r.kind)
	{
	case Tkn::KIND_KEYWORD_R0:
		vm::push8(out, uint8_t(vm::Reg_R0));
		break;
	case Tkn::KIND_KEYWORD_R1:
		vm::push8(out, uint8_t(vm::Reg_R1));
		break;
	...
	default:
		assert(false && "unreachable");
		break;
	}
}
```

TADA, now you have your own assembler that can generate bytecode which you can run on your own virtual machine.

Now let's play with our assembler
```C++
auto src = as::src_from_str(R"""(
proc main
	i32.load r0 -1
	i32.load r1 2
	i32.add r0 r1
	halt
end
)""");
mn_defer(as::src_free(src));

if(as::scan(src) == false)
{
	mn::printerr("{}", as::src_errs_dump(src, mn::memory::tmp()));
	return;
}

if(as::parse(src) == false)
{
	mn::printerr("{}", as::src_errs_dump(src, mn::memory::tmp()));
	return;
}

auto bytecode = as::proc_gen(src->procs[0]);
mn_defer(mn::buf_free(bytecode));

auto cpu = vm::core_new();
while (cpu.state == vm::Core::STATE_OK)
	vm::core_ins_execute(cpu, bytecode);

mn::print("R0 = {}\n", cpu.r[vm::Reg_R0].i32);
```

It works just like the version we did in `Day-01` but this time we didn't assemble the bytes ourselves, we wrote a program to do it for us

### Day-06
Today we'll start creating our own loader, now all we can do is convert assembly from string format
to binary format the vm can grok, but we'll need to write this binary format to disk and load it and execute it at a later time just like any executable

first let's specify our package, our package is just a bunch of procedures
```C++
struct Pkg
{
	mn::Map<mn::Str, mn::Buf<uint8_t>> procs;
};
```
then we'll need to extend our assembler to generate a package from an assembly file, it's simple we just use the proc_gen as usual but this time we put the proc inside the package struct we defined above
```C++
vm::Pkg
src_gen(Src* src)
{
	auto pkg = vm::pkg_new();
	for(size_t i = 0; i < src->procs.count; ++i)
	{
		auto name = src->procs[i].name.str;
		auto code = proc_gen(src->procs[i]);
		vm::pkg_proc_add(pkg, name, code);
	}
	return pkg;
}
```

#### Bytecode file format
Now we'll need to write this package to disk, each OS has its own complicated format, windows has Portable Executable (PE), linux has ELF, etc..., they are complicated beasts we'll go with a simple format

our file format consists of the following schema
```
File:
	[Number of procs in file: uint32_t]
	[Procs]
Proc:
	[Proc name length: uint32_t]
	[Proc name bytes]
	[Bytecode length: uint32_t]
	[Bytecode]
```

let's have a look at our save function
```C++
void
pkg_save(const Pkg& self, const mn::Str& filename)
{
	// open file
	auto f = mn::file_open(filename, mn::IO_MODE::WRITE, mn::OPEN_MODE::CREATE_OVERWRITE);
	assert(f != nullptr);
	mn_defer(mn::file_close(f));

	// write procs count to the file
	uint32_t len = uint32_t(self.procs.count);
	mn::stream_write(f, mn::block_from(len));

	// write each proc
	for(auto it = mn::map_begin(self.procs);
		it != mn::map_end(self.procs);
		it = mn::map_next(self.procs, it))
	{
		// first write proc name
		write_string(f, it->key);
		// then write proc bytecode
		write_bytes(f, it->value);
	}
}

inline static void
write_string(mn::File f, const mn::Str& str)
{
	// to write a string you just write its length as uint32_t
	uint32_t len = uint32_t(str.count);
	mn::stream_write(f, mn::block_from(len));
	// then you write the string bytes
	mn::stream_write(f, mn::block_from(str));
}

inline static void
write_bytes(mn::File f, const mn::Buf<uint8_t>& bytes)
{
	// to write bytecode you just write its length as uint32_t
	uint32_t len = uint32_t(bytes.count);
	mn::stream_write(f, mn::block_from(len));
	// then you write the bytecode bytes
	mn::stream_write(f, mn::block_from(bytes));
}
```

Now that we can save the package to disk, we'll of course need to load it back to memory
let's do that
```C++
Pkg
pkg_load(const mn::Str& filename)
{
	auto self = pkg_new();

	// open a file
	auto f = mn::file_open(filename, mn::IO_MODE::READ, mn::OPEN_MODE::OPEN_ONLY);
	assert(f != nullptr);
	mn_defer(mn::file_close(f));

	// read procs count
	uint32_t len = 0;
	mn::stream_read(f, mn::block_from(len));
	mn::map_reserve(self.procs, len);

	// read each proc
	for(size_t i = 0; i < len; ++i)
	{
		// first read the name
		auto name = read_string(f);
		// then read the bytecode
		auto bytes = read_bytes(f);
		// now add this proc to the package
		pkg_proc_add(self, name, bytes);
	}

	return self;
}

inline static mn::Str
read_string(mn::File f)
{
	// first read the string length
	uint32_t len = 0;
	mn::stream_read(f, mn::block_from(len));

	// then read the string bytes
	auto v = mn::str_new();
	mn::str_resize(v, len);
	mn::stream_read(f, mn::block_from(v));

	return v;
}

inline static mn::Buf<uint8_t>
read_bytes(mn::File f)
{
	// first read bytecode length
	uint32_t len = 0;
	mn::stream_read(f, mn::block_from(len));

	// then read the bytecode bytes
	auto v = mn::buf_with_count<uint8_t>(len);
	mn::stream_read(f, mn::block_from(v));

	return v;
}
```

#### Build command
Now that we can generate package from assembly source code and we can save this data to disk and load it from disk, let's add two commands to our assembler.

first let's do the build command
```
build: builds the file
  'tas build -o pkg_name.zyc path/to/file.zy'
```

to support this command we just copy the same code we do for parsing and append the last three lines to it
```C++
auto src = as::src_from_file(args.targets[0].ptr);
mn_defer(as::src_free(src));

if(as::scan(src) == false)
{
	mn::printerr("{}", as::src_errs_dump(src, mn::memory::tmp()));
	return -1;
}

if(as::parse(src) == false)
{
	mn::printerr("{}", as::src_errs_dump(src, mn::memory::tmp()));
	return -1;
}

// generate package from our assembly src
auto pkg = as::src_gen(src);
mn_defer(vm::pkg_free(pkg));

// then save this package to disk
vm::pkg_save(pkg, args.out_name);
```

#### Run command
now that we can assemble files to bytecode, we need to do what every OS does when you run any executable, you simply invoke the OS loader which reads the executable file format and loads the instructions into memory then starts the main function, it's time to add the run command which can load and run the assembled bytecode
```
run: loads and runs the specified package
  'tas run path/to/pkg_name.zyc'
```

```C++
// first read the package from disk
auto pkg = vm::pkg_load(args.targets[0].ptr);
mn_defer(vm::pkg_free(pkg));

// search for and load the main proc
auto code = vm::pkg_load_proc(pkg, "main");
mn_defer(mn::buf_free(code));

// execute the main proc
auto cpu = vm::core_new();
while (cpu.state == vm::Core::STATE_OK)
	vm::core_ins_execute(cpu, code);

// print the R0 register and you'll get the same result we have seen before
// R0 = 1
mn::print("R0 = {}\n", cpu.r[vm::Reg_R0].i32);
```

and that's it for today, now we have a vm along with the assembler, binary file format and the loader to run the bytecode. I think next we can extend our vm with new instructions

### Day-07
Today we'll add more arithmetic instructions, we'll add
- sub: perform subtraction
- mul: unsigned integer multiplication
- imul: signed integer multiplication
- div: unsigned integer division
- idiv: signed integer division

first let's add them to our opcodes
```C++
enum Op: uint8_t
{
	...
	// SUB [dst + op1] [op2]
	Op_SUB8,
	Op_SUB16,
	Op_SUB32,
	Op_SUB64,

	// MUL [dst + op1] [op2]
	Op_MUL8,
	Op_MUL16,
	Op_MUL32,
	Op_MUL64,

	// IMUL [dst + op1] [op2]
	Op_IMUL8,
	Op_IMUL16,
	Op_IMUL32,
	Op_IMUL64,

	// DIV [dst + op1] [op2]
	Op_DIV8,
	Op_DIV16,
	Op_DIV32,
	Op_DIV64,

	// IDIV [dst + op1] [op2]
	Op_IDIV8,
	Op_IDIV16,
	Op_IDIV32,
	Op_IDIV64,
	...
};
```

then we'll need to implement these opcodes in our vm
```C++
void
core_ins_execute(Core& self, const mn::Buf<uint8_t>& code)
{
	auto op = pop_op(self, code);
	switch(op)
	{
	...
	case Op_SUB32:
	{
		auto& dst = load_reg(self, code);
		auto& src = load_reg(self, code);
		dst.u32 -= src.u32;
		break;
	}
	...
	case Op_MUL32:
	{
		auto& dst = load_reg(self, code);
		auto& src = load_reg(self, code);
		dst.u32 *= src.u32;
		break;
	}
	...
	case Op_IMUL32:
	{
		auto& dst = load_reg(self, code);
		auto& src = load_reg(self, code);
		dst.i32 *= src.i32;
		break;
	}
	...
	case Op_DIV32:
	{
		auto& dst = load_reg(self, code);
		auto& src = load_reg(self, code);
		dst.u32 /= src.u32;
		break;
	}
	...
	case Op_IDIV32:
	{
		auto& dst = load_reg(self, code);
		auto& src = load_reg(self, code);
		dst.i32 /= src.i32;
		break;
	}
	...
	}
}
```

Now we have the opcode and we have their implementation, now we need to make the assembler aware of them

first let's add the instructions keywords to the tokens
```C++
// This is a list of the tokens
#define TOKEN_LISTING \
	...
	TOKEN(KEYWORDS__BEGIN, ""), \
	...
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
	...
	TOKEN(KEYWORDS__END, ""),
```

now we can scan these instructions, let's then add them to the parsing
```C++
// Here we extend the is_add function to be is_arithmetic and it checks sub, mul, and div
inline static bool
is_arithmetic(const Tkn& tkn)
{
	return (tkn.kind == Tkn::KIND_KEYWORD_I8_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_I16_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_I32_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_I64_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_U8_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_U16_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_U32_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_U64_ADD ||
			tkn.kind == Tkn::KIND_KEYWORD_I8_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_I16_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_I32_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_I64_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_U8_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_U16_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_U32_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_U64_SUB ||
			tkn.kind == Tkn::KIND_KEYWORD_I8_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_I16_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_I32_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_I64_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_U8_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_U16_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_U32_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_U64_MUL ||
			tkn.kind == Tkn::KIND_KEYWORD_I8_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_I16_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_I32_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_I64_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_U8_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_U16_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_U32_DIV ||
			tkn.kind == Tkn::KIND_KEYWORD_U64_DIV);
}

inline static Ins
parser_ins(Parser* self)
{
	Ins ins{};

	Tkn op = parser_look(self);
	if (is_load(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
		ins.src = parser_const(self);
	}
	else if (is_arithmetic(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
		ins.src = parser_reg(self);
	}
	else if(op.kind == Tkn::KIND_KEYWORD_HALT)
	{
		ins.op = parser_eat(self);
	}

	return ins;
}
```

now we can parse the instructions, let's add them to code generation
```C++
inline static void
ins_gen(const Ins& ins, mn::Buf<uint8_t>& out)
{
	switch(ins.op.kind)
	{
	...
	case Tkn::KIND_KEYWORD_I32_SUB:
	case Tkn::KIND_KEYWORD_U32_SUB:
		vm::push8(out, uint8_t(vm::Op_SUB32));
		reg_gen(ins.dst, out);
		reg_gen(ins.src, out);
		break;
	...
	case Tkn::KIND_KEYWORD_I32_MUL:
		vm::push8(out, uint8_t(vm::Op_IMUL32));
		reg_gen(ins.dst, out);
		reg_gen(ins.src, out);
		break;

	case Tkn::KIND_KEYWORD_U32_MUL:
		vm::push8(out, uint8_t(vm::Op_MUL32));
		reg_gen(ins.dst, out);
		reg_gen(ins.src, out);
		break;
	...
	case Tkn::KIND_KEYWORD_I32_DIV:
		vm::push8(out, uint8_t(vm::Op_IDIV32));
		reg_gen(ins.dst, out);
		reg_gen(ins.src, out);
		break;

	case Tkn::KIND_KEYWORD_U32_DIV:
		vm::push8(out, uint8_t(vm::Op_DIV32));
		reg_gen(ins.dst, out);
		reg_gen(ins.src, out);
		break;
	...
	}
}
```

Now we can also bytecode generate those instructions, and that's it we can now do complex stuff like this
```
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
```

### Day-8
Today we'll add conditional jumps. this will make this code possible, we try to check if a number (in r2 register) is positive or negative or a zero

note the conditional jumps usage `[i32.jl] [op1] [op2] [success branch]`
```asm
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
```

#### Virtual Machine

first let's add the instructions to the vm
```C++
enum Op: uint8_t
{
	...
	// unsigned compare
	// CMP [op1] [op2]
	Op_CMP8,
	Op_CMP16,
	Op_CMP32,
	Op_CMP64,

	// signed compare
	// ICMP [op1] [op2]
	Op_ICMP8,
	Op_ICMP16,
	Op_ICMP32,
	Op_ICMP64,

	// jump unconditionall
	// JMP [offset signed 64-bit]
	Op_JMP,

	// jump if equal
	// JE [offset signed 64-bit]
	Op_JE,

	// jump if not equal
	// JNE [offset signed 64-bit]
	Op_JNE,

	// jump if less than
	// JL [offset signed 64-bit]
	Op_JL,

	// jump if less than or equal
	// JLE [offset signed 64-bit]
	Op_JLE,

	// jump if greater than
	// JG [offset signed 64-bit]
	Op_JG,

	// jump if greater than or equal
	// JGE [offset signed 64-bit]
	Op_JGE,
	...
};
```

then let's try to implement these instructions in our core
```C++
...
case Op_CMP32:
{
	auto& op1 = load_reg(self, code);
	auto& op2 = load_reg(self, code);
	if (op1.u32 > op2.u32)
		self.cmp = Core::CMP_GREATER;
	else if (op1.u32 < op2.u32)
		self.cmp = Core::CMP_LESS;
	else
		self.cmp = Core::CMP_EQUAL;
	break;
}
...
case Op_ICMP32:
{
	auto& op1 = load_reg(self, code);
	auto& op2 = load_reg(self, code);
	if (op1.i32 > op2.i32)
		self.cmp = Core::CMP_GREATER;
	else if (op1.i32 < op2.i32)
		self.cmp = Core::CMP_LESS;
	else
		self.cmp = Core::CMP_EQUAL;
	break;
}
...
case Op_JMP:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	self.r[Reg_IP].u64 += offset;
	break;
}
case Op_JE:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	if (self.cmp == Core::CMP_EQUAL)
	{
		self.r[Reg_IP].u64 += offset;
	}
	break;
}
case Op_JNE:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	if (self.cmp != Core::CMP_EQUAL)
	{
		self.r[Reg_IP].u64 += offset;
	}
	break;
}
case Op_JL:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	if (self.cmp == Core::CMP_LESS)
	{
		self.r[Reg_IP].u64 += offset;
	}
	break;
}
case Op_JLE:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	if (self.cmp == Core::CMP_LESS || self.cmp == Core::CMP_EQUAL)
	{
		self.r[Reg_IP].u64 += offset;
	}
	break;
}
case Op_JG:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	if (self.cmp == Core::CMP_GREATER)
	{
		self.r[Reg_IP].u64 += offset;
	}
	break;
}
case Op_JGE:
{
	int64_t offset = int64_t(pop64(code, self.r[Reg_IP].u64));
	if (self.cmp == Core::CMP_GREATER || self.cmp == Core::CMP_EQUAL)
	{
		self.r[Reg_IP].u64 += offset;
	}
	break;
}
...
```

#### Scanner
Now that we have the instructions in place, let's add the instructions to assemblers

first let's add the tokens
```C++
...
TOKEN(COLON, ":"), \
...
TOKEN(KEYWORDS__BEGIN, ""), \
...
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
...
TOKEN(KEYWORDS__END, ""),
```

#### Parser
then let's add these instructions to the parser, also we'll need to add label support

first we'll need to add the success label field to the instruction struct itself
```C++
struct Ins
{
	Tkn op;  // operation
	Tkn dst; // destination
	Tkn src; // source
	Tkn lbl; // label
};
```

then let's add the instructions to the parser
```C++
inline static Ins
parser_ins(Parser* self)
{
	...
	else if (is_cond_jump(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
		ins.src = parser_reg(self);
		ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
	}
	else if (op.kind == Tkn::KIND_KEYWORD_JMP)
	{
		ins.op = parser_eat(self);
		ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
	}
	// label
	else if (op.kind == Tkn::KIND_ID)
	{
		ins.op = parser_eat(self);
		parser_eat_must(self, Tkn::KIND_COLON);
	}
	...
}
```

#### Codegen
now let's add them to the code generation, now we'll need to think about what we'll do in a case where someone adds a jump to a forward label like the `negative` label in the our code above.

here's the plan. when we generate the code and find a jump instruction we add the label name and the position in our bytecode buffer in a fixup array and simply we emit 0 in place of the offset. if we find a label we add it to some symbol table and register its bytecode location.

After finishing code generation we go and fix all the jumps. let's start executing.

first let's start by creating the emitter struct
```C++
struct Emitter
{
	Src* src;
	mn::Buf<uint8_t> out;
	mn::Buf<Fixup_Request> fixups;
	mn::Map<const char*, size_t> symbols;
};
```

now let's add the jump code generation
```C++
case Tkn::KIND_KEYWORD_I32_JL:
	// emit compare instruction
	vm::push8(self.out, uint8_t(vm::Op_ICMP32));
	emitter_reg_gen(self, ins.dst);
	emitter_reg_gen(self, ins.src);
	// emit the jump
	vm::push8(self.out, uint8_t(vm::Op_JL));
	// put the fixup request in the array
	emitter_label_fixup_request(self, ins.lbl);
	break;
```

let's have a look at the `emitter_label_fixup_request` function
```C++
inline static void
emitter_label_fixup_request(Emitter& self, const Tkn& label)
{
	// add the fixup request with the fixup location in the output byte array
	mn::buf_push(self.fixups, Fixup_Request{ label, self.out.count });
	// then add 0 as a placeholder
	vm::push64(self.out, 0);
}
```

now let's do the fixup code
```C++
...
// do the fixups
for(auto fixup: self.fixups)
{
	// try to find label in the symbol table
	auto it = mn::map_lookup(self.symbols, fixup.name.str);

	// emit an error if we didn't find the label
	if(it == nullptr)
	{
		src_err(self.src, fixup.name, mn::strf("'{}' undefined symbol", fixup.name.str));
		continue;
	}

	// JL | [offset64] ^
	// code...
	// target_label:
	// code...
	// 
	// calculate the jump offset
	// keep in mind that we put the address of the start of the offset in the code which is 
	// middle of the instruction the '|' position in the example above but we need it to be at the '^' position
	// so we add sizeof(int64_t) to it to align it with the instruction itself then we calc the offset
	int64_t offset = it->value - (fixup.bytecode_index + sizeof(int64_t));
	write64(self.out.ptr + fixup.bytecode_index, uint64_t(offset));
}
...
```

and now if we run the new code we get `-1` if `r2 < 0`, `1` if `r2 > 0`, and `0` if `r2 == 0`

### Day-9
Let's add stack memory to our CPU

```asm
proc main
	; load some constants to the registers
	i32.load r0 1
	i32.load r1 2
	i32.load r2 4

	; write r0 to the stack
	i32.write sp r0
	; move the stack 4 bytes upward
	i32.sub sp r2

	; write r1 to the stack
	i32.write sp r1
	; move the stack 4 bytes upward
	i32.sub sp r2

	; change the values inside r0 and r1
	i32.load r0 3
	i32.load r1 4

	; move the stack 4 bytes downward
	i32.add sp r2
	; read the value of r1 back
	i32.read r1 sp

	; move the stack 4 bytes downward
	i32.add sp r2
	; read the value of r0 back
	i32.read r0 sp

	; stop the execution
	halt
end
```

first we'll need to add the stack pointer register
```C++
enum Reg: uint8_t
{
	...
	// instruction pointer
	Reg_IP,

	// stack pointer
	Reg_SP,

	//Count of the registers
	Reg_COUNT
};

union Reg_Val
{
	...
	uint64_t u64;
	void*	 ptr;
};
```

now that we have added the register to hold the stack memory address let's add the actual stack memory in the cpu core
```C++
struct Core
{
	...
	Reg_Val r[Reg_COUNT];

	mn::Buf<uint8_t> stack_memory;
};
```

then let's initialize it
```C++
Core
core_new()
{
	Core self{};
	// we start with 8 Megabytes of stack memory
	self.stack_memory = mn::buf_with_count<uint8_t>(8ULL * 1024ULL * 1024ULL);
	// we set the stack register to point to the end of the stack since our stack will be growing upward (you grow the stack by subtracting from SP)
	self.r[Reg_SP].ptr = end(self.stack_memory);
	return self;
}
```

now that we have everything set let's add the opcodes
```C++
enum Op: uint8_t
{
	...
	// reads data from the src address register into the specified dst register
	// READ [dst] [src]
	Op_READ8,
	Op_READ16,
	Op_READ32,
	Op_READ64,

	// writes data from the src register into the specified dst address register
	// WRITE [dst] [src]
	Op_WRITE8,
	Op_WRITE16,
	Op_WRITE32,
	Op_WRITE64,
	...
}
```

then let's implement them
```C++
void
core_ins_execute(Core& self, const mn::Buf<uint8_t>& code)
{
	...
	case Op_READ32:
	{
		// get the dst register
		auto& dst = load_reg(self, code);
		// get the src address register
		auto& src = load_reg(self, code);
		// move the pointer of the src register to the front of the 4 bytes
		auto ptr = ((uint32_t*)src.ptr - 1);
		// check if the pointer is a valid pointer to the stack and that we have 4 bytes available
		if(_valid_next_bytes(self, ptr, 4) == false)
		{
			self.state = Core::STATE_ERR;
			break;
		}
		// read the pointer into the dst register
		dst.u32 = *ptr;
		break;
	}
	...
	case Op_WRITE32:
	{
		// get the dst address register
		auto& dst = load_reg(self, code);
		// get the src register
		auto& src = load_reg(self, code);
		// move the pointer of the dst register to the front of the 4 bytes
		auto ptr = ((uint32_t*)dst.ptr - 1);
		// check if the pointer is a valid pointer to the stack and that we have 4 bytes available
		if(_valid_next_bytes(self, ptr, 4) == false)
		{
			self.state = Core::STATE_ERR;
			break;
		}
		// write the src register into the dst address register
		*ptr = src.u32;
		break;
	}
	...
}
```

now that we've got the implementation in place let's add it to the assembler

first let's add it to the token listing
```C++
// This is a list of the tokens
#define TOKEN_LISTING \
	...
	TOKEN(KEYWORDS__BEGIN, ""), \
	...
	TOKEN(KEYWORD_I8_READ, "i8.read"), \
	TOKEN(KEYWORD_I16_READ, "i16.read"), \
	TOKEN(KEYWORD_I32_READ, "i32.read"), \
	TOKEN(KEYWORD_I64_READ, "i64.read"), \
	TOKEN(KEYWORD_U8_READ, "u8.read"), \
	TOKEN(KEYWORD_U16_READ, "u16.read"), \
	TOKEN(KEYWORD_U32_READ, "u32.read"), \
	TOKEN(KEYWORD_U64_READ, "u64.read"), \
	TOKEN(KEYWORD_I8_WRITE, "i8.write"), \
	TOKEN(KEYWORD_I16_WRITE, "i16.write"), \
	TOKEN(KEYWORD_I32_WRITE, "i32.write"), \
	TOKEN(KEYWORD_I64_WRITE, "i64.write"), \
	TOKEN(KEYWORD_U8_WRITE, "u8.write"), \
	TOKEN(KEYWORD_U16_WRITE, "u16.write"), \
	TOKEN(KEYWORD_U32_WRITE, "u32.write"), \
	TOKEN(KEYWORD_U64_WRITE, "u64.write"), \
	...
	TOKEN(KEYWORDS__END, ""),
```

then we'll do the parsing
```C++
inline static Ins
parser_ins(Parser* self)
{
	...
	else if(is_mem_transfer(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
		ins.src = parser_reg(self);
	}
	...
}
```

the last missing piece is to add instructions to code generation
```C++
inline static void
emitter_ins_gen(Emitter& self, const Ins& ins)
{
	switch(ins.op.kind)
	{
	...
	case Tkn::KIND_KEYWORD_I32_READ:
	case Tkn::KIND_KEYWORD_U32_READ:
		vm::push8(self.out, uint8_t(vm::Op_READ32));
		emitter_reg_gen(self, ins.dst);
		emitter_reg_gen(self, ins.src);
		break;
	...
	case Tkn::KIND_KEYWORD_I32_WRITE:
	case Tkn::KIND_KEYWORD_U32_WRITE:
		vm::push8(self.out, uint8_t(vm::Op_WRITE32));
		emitter_reg_gen(self, ins.dst);
		emitter_reg_gen(self, ins.src);
		break;
	...
	}
}
```

and that's it now we can read and write to the stack memory as you've seen above

### Day-10
Today we'll add the `push` and `pop` instructions so we can rewrite the code from yesterday like this

```asm
proc main
	i32.load r0 1
	i32.load r1 2

	push r0
	push r1

	i32.load r0 3
	i32.load r1 4

	pop r1
	pop r0

	halt
end
```

You know how it goes by now, let's add it to the vm

first let's add the opcode
```C++
enum Op: uint8_t
{
	...
	// pushes the register into the stack and increment it
	// PUSH [register]
	Op_PUSH,

	// pops the register into the stack and decrement it
	// POP [register]
	Op_POP,
	...
};
```

now let's implement it
```C++
void
core_ins_execute(Core& self, const mn::Buf<uint8_t>& code)
{
	auto op = pop_op(self, code);
	switch(op)
	{
	...
	case Op_PUSH:
	{
		// load stack pointer
		auto& dst = self.r[Reg_SP];
		// load the source register to be written
		auto& src = load_reg(self, code);
		// allocate space for it on the stack
		auto ptr = ((uint64_t*)dst.ptr - 1);
		// check if the stack have enough space
		if(_valid_next_bytes(self, ptr, 8) == false)
		{
			self.state = Core::STATE_ERR;
			break;
		}
		// write the register to the stack
		*ptr = src.u64;
		// move the stack to the new position
		dst.ptr = ptr;
		break;
	}
	case Op_POP:
	{
		// load the dst register to be read to
		auto& dst = load_reg(self, code);
		// load stack pointer
		auto& src = self.r[Reg_SP];
		// get the stack pointer
		auto ptr = ((uint64_t*)src.ptr);
		// check if the stack have enough space
		if(_valid_next_bytes(self, ptr, 8) == false)
		{
			self.state = Core::STATE_ERR;
			break;
		}
		// read from the stack
		dst.u64 = *ptr;
		// deallocate the stack space
		src.ptr = ptr + 1;
		break;
	}
	...
	}
}
```

now let's add the tokens
```C++
// This is a list of the tokens
#define TOKEN_LISTING \
	...
	TOKEN(KEYWORDS__BEGIN, ""), \
	...
	TOKEN(KEYWORD_PUSH, "push"), \
	TOKEN(KEYWORD_POP, "pop"), \
	...
	TOKEN(KEYWORDS__END, ""),
```

then let's add it to parser
```C++
inline static Ins
parser_ins(Parser* self)
{
	Ins ins{};

	Tkn op = parser_look(self);
	...
	else if(is_push_pop(op))
	{
		ins.op = parser_eat(self);
		ins.dst = parser_reg(self);
	}
	...

	return ins;
}
```

after that we'll add it the code generation
```C++
inline static void
emitter_ins_gen(Emitter& self, const Ins& ins)
{
	switch(ins.op.kind)
	{
	...
	case Tkn::KIND_KEYWORD_PUSH:
		vm::push8(self.out, uint8_t(vm::Op_PUSH));
		emitter_reg_gen(self, ins.dst);
		break;

	case Tkn::KIND_KEYWORD_POP:
		vm::push8(self.out, uint8_t(vm::Op_POP));
		emitter_reg_gen(self, ins.dst);
		break;
	...
	}
}
```

and that's it, now we have the push and pop instructions

### Day-11

Today is a big day, because today we'll add the `CALL` and `RET` instructions and we'll be able to call procedures. YAY

```asm
proc main
	i32.load r0 1
	i32.load r1 2

	call add

	halt
end

proc add
	i32.add r0 r1
	ret
end
```

first we'll need to add it to the VM, let's start by adding the opcodes
```C++
enum Op
{
	...
	// performs a call instruction
	// CALL [address unsigned 64-bit]
	Op_CALL,

	// returns from proc calls
	// RET
	Op_RET,
	...
};
```

then we'll implement them
```C++
case Op_CALL:
{
	// load proc address
	auto address = pop64(code, self.r[Reg_IP].u64);
	// load stack pointer
	auto& SP = self.r[Reg_SP];
	// allocate space for return address
	auto ptr = ((uint64_t*)SP.ptr - 1);
	if(_valid_next_bytes(self, ptr, 8) == false)
	{
		self.state = Core::STATE_ERR;
		break;
	}
	// write the return address
	*ptr = self.r[Reg_IP].u64;
	// move the stack pointer
	SP.ptr = ptr;
	// jump to proc address
	self.r[Reg_IP].u64 = address;
	break;
}
case Op_RET:
{
	// load stack pointer
	auto& SP = self.r[Reg_SP];
	auto ptr = ((uint64_t*)SP.ptr);
	if(_valid_next_bytes(self, ptr, 8) == false)
	{
		self.state = Core::STATE_ERR;
		break;
	}
	// restore the IP
	self.r[Reg_IP].u64 = *ptr;
	// deallocate the space for return address
	SP.ptr = ptr;
	break;
}
```

then we'll need to add it to the assembler, let's add it to the tokens
```C++
TOKEN(KEYWORD_CALL, "call"), \
TOKEN(KEYWORD_RET, "ret"), \
```

then we'll need to parse them
```C++
else if (op.kind == Tkn::KIND_KEYWORD_CALL)
{
	ins.op = parser_eat(self);
	ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
}
else if(op.kind == Tkn::KIND_KEYWORD_RET)
{
	ins.op = parser_eat(self);
}
```

#### Relocation
and let's do the code gen, now the problem with code generation is that we'll need to patch function addresses in call instructions, we can reuse the labels code but consider this
let's say we'll need to reuse some code and instead of generating executable packages we'll need to generate library packages then we'll need to load functions in memory and stitch them as needed so here's the plan

the assembler will add a relocation request in the package when it encounters a call instruction
what's a relocation request?
it's a request for the loader to patch this address when it loads the code in memory and it consists of the name of function (source function name), the offset inside this function bytecode (source offset), and the name of the target function (target function)

so in the example above we'll have a relocation
```
Relocation {
	source_name: main,
	source_it: 21,
	target_name: add
}
```
that's basically telling the loader to patch an address inside main function with offset 21 byte and the address to write is the address of the target function which is add function

let's execute the plan
```C++
case Tkn::KIND_KEYWORD_CALL:
	vm::push8(self.out, uint8_t(vm::Op_CALL));
	// register the relocation request
	vm::pkg_reloc_add(pkg, mn::str_from_c(proc.name.str), self.out.count, mn::str_from_c(ins.lbl.str));
	// put 0 placeholder for the loader to patch
	vm::push64(self.out, 0);
	break;

case Tkn::KIND_KEYWORD_RET:
	vm::push8(self.out, uint8_t(vm::Op_RET));
	break;
```

then we'll need to add the relocations in the package
```C++
// relocations is used to fix proc address on loading in call instructions
struct Reloc
{
	mn::Str source_name;
	mn::Str target_name;
	uint64_t source_offset;
};

struct Pkg
{
	mn::Map<mn::Str, mn::Buf<uint8_t>> procs;
	mn::Buf<Reloc> relocs;
};
```

then we'll need to change the old `pkg_load_proc` function to be this new function
```C++
Bytecode
pkg_bytecode_main_generate(const Pkg& self, mn::Allocator allocator)
{
	// result bytecode
	auto res = mn::buf_with_allocator<uint8_t>(allocator);

	// create a table of function names and their offset in the result bytecode
	auto loaded_procs_table = mn::map_new<mn::Str, uint64_t>();
	mn_defer(mn::map_free(loaded_procs_table));

	// append each proc bytecode to the result bytecode array
	for(auto it = mn::map_begin(self.procs);
		it != mn::map_end(self.procs);
		it = mn::map_next(self.procs, it))
	{
		// add the proc name and offset in the loaded_procs_table
		mn::map_insert(loaded_procs_table, it->key, uint64_t(res.count));
		// add proc bytecode to the res buffer
		mn::buf_concat(res, it->value);
	}

	// after loading procs we'll need to perform the relocs
	for(const auto& reloc: self.relocs)
	{
		auto source_it = mn::map_lookup(loaded_procs_table, reloc.source_name);
		auto target_it = mn::map_lookup(loaded_procs_table, reloc.target_name);
		assert(source_it && target_it);
		write64(res.ptr + source_it->value + reloc.source_offset, target_it->value);
	}

	// search for main proc
	auto main_it = mn::map_lookup(loaded_procs_table, mn::str_lit("main"));

	// return the loaded bytecode and the main proc address
	return Bytecode{res, main_it->value};
}
```

and the last thing we'll need to do is to change the way we load code in our vm core
```C++
auto [code, main_address] = vm::pkg_bytecode_main_generate(pkg);
mn_defer(mn::buf_free(code));

auto cpu = vm::core_new();
mn_defer(vm::core_free(cpu));

// load the main address into the cpu core
cpu.r[vm::Reg_IP].u64 = main_address;

while (cpu.state == vm::Core::STATE_OK)
	vm::core_ins_execute(cpu, code);
```

and that's it, if you run the code you'll get the result of adding 1 +  2 which is 3 in R0 register
```
R0 = 3
R1 = 2
```

### Day-12

Today we'll make some quality of life improvements. We'll add immediate versions of our instructions so that you can write

```asm
i32.add r0 5
```

instead of 

```asm
i32.load r1 5
i32.add r0 r1
```

and we'll do it for `add`, `sub`, `mul`, `imul`, `div`, `idiv`, `cmp`, `icmp`

let's go

#### VM Support

First we'll need to add the opcodes

```C++
// IMMADD [dst + op1] [immediate argument (8-bit, 16-bit, 32-bit, 64-bit number)]
Op_IMMADD8,
Op_IMMADD16,
Op_IMMADD32,
Op_IMMADD64,
```

Then we'll implement them

```C++
case Op_IMMADD8:
{
    auto& dst = load_reg(self, code);
    auto imm = pop8(code, self.r[Reg_IP].u64);
    dst.u8 += imm;
    break;
}
case Op_IMMADD16:
{
    auto& dst = load_reg(self, code);
    auto imm = pop16(code, self.r[Reg_IP].u64);
    dst.u16 += imm;
    break;
}
case Op_IMMADD32:
{
    auto& dst = load_reg(self, code);
    auto imm = pop32(code, self.r[Reg_IP].u64);
    dst.u32 += imm;
    break;
}
case Op_IMMADD64:
{
    auto& dst = load_reg(self, code);
    auto imm = pop64(code, self.r[Reg_IP].u64);
    dst.u64 += imm;
    break;
}
```

You can do the same for the other instructions we have listed above

#### Assembler Support

Now let's make our assembler generate these instructions.

First we'll need to parse them. We'll edit the parsing of our arithmetic and conditional jump instructions

```C++
else if (is_arithmetic(op))
{
    ins.op = parser_eat(self);
    ins.dst = parser_reg(self);
    auto src = parser_look(self);
    if (src.kind == Tkn::KIND_INTEGER || is_reg(src))
        ins.src = parser_eat(self);
    else
        src_err(self->src, src, mn::strf("expected an integer or a register"));
}
else if (is_cond_jump(op))
{
    ins.op = parser_eat(self);
    ins.dst = parser_reg(self);
    auto src = parser_look(self);
    if(src.kind == Tkn::KIND_INTEGER || is_reg(src))
        ins.src = parser_eat(self);
    else
        src_err(self->src, src, mn::strf("expected an integer or a register"));
    ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
}
```

Then we'll need to add them to code gen

```C++
case Tkn::KIND_KEYWORD_I32_ADD:
    if (is_reg(ins.src))
    {
        vm::push8(self.out, uint8_t(vm::Op_ADD32));
        emitter_reg_gen(self, ins.dst);
        emitter_reg_gen(self, ins.src);
    }
    else if(ins.src.kind == Tkn::KIND_INTEGER)
    {
        vm::push8(self.out, uint8_t(vm::Op_IMMADD32));
        emitter_reg_gen(self, ins.dst);
        vm::push32(self.out, uint32_t(convert_to<int32_t>(ins.src)));
    }
    break;

case Tkn::KIND_KEYWORD_U32_ADD:
    if (is_reg(ins.src))
    {
        vm::push8(self.out, uint8_t(vm::Op_ADD32));
        emitter_reg_gen(self, ins.dst);
        emitter_reg_gen(self, ins.src);
    }
    else if(ins.src.kind == Tkn::KIND_INTEGER)
    {
        vm::push8(self.out, uint8_t(vm::Op_IMMADD32));
        emitter_reg_gen(self, ins.dst);
        vm::push32(self.out, convert_to<uint32_t>(ins.src));
    }
    break;
```

And just like that we're now able to do this

```asm
i32.load r2 -2
i32.jl r2 0 negative
jmp maybe_positive
```

Instead of this

```asm
i32.load r2 -2
i32.load r1 0
i32.jl r2 r1 negative
jmp maybe_positive
```

### Day-13

Today we'll add comments support in our assembler

```asm
; this is the main procedure
proc main
	i32.load r2 -2; this is equal to r2 = -2
end
```

first we'll address the token scanning part by adding the comment token to our list
```C++
TOKEN(COMMENT, "<COMMENT>"), \
```
then we'll add the support in the scanner
```C++
case ';':
	tkn.kind = Tkn::KIND_COMMENT;
	tkn.str = scanner_comment(self);
	break;
```
and we'll of course add the `scanner_comment` function which consumes the characters till the newline character
```C++
inline static const char*
scanner_comment(Scanner* self)
{
	auto begin_it = self->it;
	auto prev = self->c;
	while (self->c != '\n')
	{
		prev = self->c;
		if (scanner_eat(self) == false)
			break;
	}

	auto end_it = self->it;
	if (prev == '\r')
		--end_it;

	return mn::str_intern(self->src->str_table, begin_it, end_it);
}
```

Now the only remaining part is the parser, the change is as simple as removing any tokens that the parser doesn't care about like the comment tokens
```C++
inline static bool
tkn_is_ignore(const Tkn& tkn)
{
	return tkn.kind == Tkn::KIND_COMMENT;
}

inline static Parser
parser_new(Src* src)
{
	Parser self{};
	self.src = src;
	self.tkns = mn::buf_clone(src->tkns);
	self.ix = 0;
	// remove ignore tokens
	mn::buf_remove_if(self.tkns, [](const Tkn& tkn) { return tkn_is_ignore(tkn); });
	return self;
}
```

and that's it

### Day-14

Today we'll add the ability to explicitly do cmp instruction.
When you needed to do a conditional jump like this
```asm
i32.jl r2 0 negative
```
the assembler will implicitly generate a cmp and a jl instruction so it will translate it to
```asm
i32.cmp r2 0
jl negative
```
but you couldn't have written these two instructions yourself, today we fix that by adding these two
instructions to the assembler

First we'll add them to our TokenListing file
```C++
TOKEN(KEYWORD_I8_CMP, "i8.cmp"), \
TOKEN(KEYWORD_I16_CMP, "i16.cmp"), \
TOKEN(KEYWORD_I32_CMP, "i32.cmp"), \
TOKEN(KEYWORD_I64_CMP, "i64.cmp"), \
TOKEN(KEYWORD_U8_CMP, "u8.cmp"), \
TOKEN(KEYWORD_U16_CMP, "u16.cmp"), \
TOKEN(KEYWORD_U32_CMP, "u32.cmp"), \
TOKEN(KEYWORD_U64_CMP, "u64.cmp"), \
TOKEN(KEYWORD_JE, "je"), \
TOKEN(KEYWORD_JNE, "jne"), \
TOKEN(KEYWORD_JL, "jl"), \
TOKEN(KEYWORD_JLE, "jle"), \
TOKEN(KEYWORD_JG, "jg"), \
TOKEN(KEYWORD_JGE, "jge"), \
```

now we'll need to change the parser, first we'll need to add these functions
```C++
inline static bool
is_cmp(const Tkn& tkn)
{
	return (tkn.kind == Tkn::KIND_KEYWORD_I8_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_I16_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_I32_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_I64_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_U8_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_U16_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_U32_CMP ||
			tkn.kind == Tkn::KIND_KEYWORD_U64_CMP);
}

inline static bool
is_pure_jump(const Tkn& tkn)
{
	return (tkn.kind == Tkn::KIND_KEYWORD_JE ||
			tkn.kind == Tkn::KIND_KEYWORD_JNE ||
			tkn.kind == Tkn::KIND_KEYWORD_JL ||
			tkn.kind == Tkn::KIND_KEYWORD_JLE ||
			tkn.kind == Tkn::KIND_KEYWORD_JG ||
			tkn.kind == Tkn::KIND_KEYWORD_JGE ||
			tkn.kind == Tkn::KIND_KEYWORD_JMP);
}
```

then we'll change the parser_ins function
```C++
else if (is_pure_jump(op))
{
	ins.op = parser_eat(self);
	ins.lbl = parser_eat_must(self, Tkn::KIND_ID);
}
else if(is_cmp(op))
{
	ins.op = parser_eat(self);
	ins.dst = parser_reg(self);
	auto src = parser_look(self);
	if(src.kind == Tkn::KIND_INTEGER || is_reg(src))
		ins.src = parser_eat(self);
	else
		src_err(self->src, src, mn::strf("expected an integer or a register"));
}
```

last thing to do is add them to our code gen pass
```C++
case Tkn::KIND_KEYWORD_JE:
	vm::push8(self.out, uint8_t(vm::Op_JE));
	emitter_label_fixup_request(self, ins.lbl);
	break;

case Tkn::KIND_KEYWORD_JNE:
	vm::push8(self.out, uint8_t(vm::Op_JNE));
	emitter_label_fixup_request(self, ins.lbl);
	break;

case Tkn::KIND_KEYWORD_JL:
	vm::push8(self.out, uint8_t(vm::Op_JL));
	emitter_label_fixup_request(self, ins.lbl);
	break;

case Tkn::KIND_KEYWORD_JLE:
	vm::push8(self.out, uint8_t(vm::Op_JLE));
	emitter_label_fixup_request(self, ins.lbl);
	break;

case Tkn::KIND_KEYWORD_JG:
	vm::push8(self.out, uint8_t(vm::Op_JG));
	emitter_label_fixup_request(self, ins.lbl);
	break;

case Tkn::KIND_KEYWORD_JGE:
	vm::push8(self.out, uint8_t(vm::Op_JGE));
	emitter_label_fixup_request(self, ins.lbl);
	break;
...
case Tkn::KIND_KEYWORD_I8_CMP:
	if(is_reg(ins.src))
	{
		vm::push8(self.out, uint8_t(vm::Op_ICMP8));
		emitter_reg_gen(self, ins.dst);
		emitter_reg_gen(self, ins.src);
	}
	else if(ins.src.kind == Tkn::KIND_INTEGER)
	{
		vm::push8(self.out, uint8_t(vm::Op_IMMICMP8));
		emitter_reg_gen(self, ins.dst);
		vm::push8(self.out, uint8_t(convert_to<int8_t>(ins.src)));
	}
	break;
...
case Tkn::KIND_KEYWORD_U8_CMP:
	if(is_reg(ins.src))
	{
		vm::push8(self.out, uint8_t(vm::Op_CMP8));
		emitter_reg_gen(self, ins.dst);
		emitter_reg_gen(self, ins.src);
	}
	else if(ins.src.kind == Tkn::KIND_INTEGER)
	{
		vm::push8(self.out, uint8_t(vm::Op_IMMCMP8));
		emitter_reg_gen(self, ins.dst);
		vm::push8(self.out, convert_to<uint8_t>(ins.src));
	}
	break;
```

and that's it now we can write the sign function from earlier like this, with one compare and three conditional branches
```asm
proc main
	; this is program to check the sign of the number in r2
	i32.load r2 -2
	i32.cmp r2 0 ; one compare
	; three conditional branches
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
```

### Day-15

Today we'll start our quest to call C runtime library's printf function, first of all we'll add
constants support which will only support strings for now, and a temporary instruction `debugstr` to debug the constant

at the end of the day we'll be able to run this code
```asm
constant msg "Hello, World!\0"

proc main
	u64.load r1 msg
	debugstr r1
	halt
end
```

which will print the `DEBUG: 'Hello, World!'` string

now let's start by discussing the changes to the VM

first we'll add the `debugstr` opcode
```C++
// TEMPORARY Op
// prints a debug string
// DEBUGSTR [register = address unsigned 64-bit]
Op_DEBUGSTR,
```

then we'll implement it
```C++
case Op_DEBUGSTR:
{
	auto& str_address = load_reg(self);
	mn::print("DEBUG: '{}'\n", (const char*)str_address.ptr);
	break;
}
```

and that's it for the VM for now, we'll move on to the assembler now

first let's add a couple of tokens
```C++
TOKEN(STRING, "<STRING>"), \
...
TOKEN(KEYWORD_CONSTANT, "CONSTANT"), \
TOKEN(KEYWORD_DEBUGSTR, "DEBUGSTR"), \
```

then we'll need to scan the string token
```C++
inline static void
scanner_string(Scanner* self, Tkn* tkn)
{
	auto begin_it = self->it;
	auto end_it = self->it;

	auto prev = self->c;
	// eat all runes even those escaped by \ like \"
	while(self->c != '"' || prev == '\\')
	{
		if (scanner_eat(self) == false)
		{
			src_err(self->src, self->pos, mn::strf("unterminated string"));
			break;
		}
		prev = self->c;
	}

	end_it = self->it;
	scanner_eat(self); // for the "
	tkn->str = mn::str_intern(self->src->str_table, begin_it, end_it);
	tkn->rng.begin = begin_it;
	tkn->rng.end = end_it;
}

inline static Tkn
scanner_tkn(Scanner* self)
{
	...
	case '"':
	tkn.kind = Tkn::KIND_STRING;
	scanner_string(self, &tkn);
	no_intern = true;
	no_rng = true;
	break;
	...
}
```

then we'll move on to parsing but first remember that constant is not really an instruction it's a declaration on the top level so we'll need to change our parse_tree to match that
```C++
struct Constant
{
	Tkn name;
	Tkn value;
};

struct Decl
{
	enum KIND
	{
		KIND_NONE,
		KIND_PROC,
		KIND_CONSTANT
	};

	KIND kind;
	union
	{
		Proc proc;
		Constant constant;
	};
};

Decl*
decl_proc_new(Proc proc)
{
	auto self = mn::alloc_zerod<Decl>();
	self->kind = Decl::KIND_PROC;
	self->proc = proc;
	return self;
}

Decl*
decl_constant_new(Constant constant)
{
	auto self = mn::alloc_zerod<Decl>();
	self->kind = Decl::KIND_CONSTANT;
	self->constant = constant;
	return self;
}

void
decl_free(Decl* self)
{
	if (self->kind == Decl::KIND_PROC)
		proc_free(self->proc);
	mn::free(self);
}
```

then we'll go on and parse that
```C++
inline static Constant
parser_constant(Parser* self)
{
	parser_eat_must(self, Tkn::KIND_KEYWORD_CONSTANT);
	Constant constant{};
	constant.name = parser_eat_must(self, Tkn::KIND_ID);
	constant.value = parser_eat_must(self, Tkn::KIND_STRING);
	return constant;
}

bool
parse(Src* src)
{
	auto parser = parser_new(src);
	mn_defer(parser_free(parser));

	while(parser.ix < parser.tkns.count)
	{
		auto tkn = parser_look(&parser);
		if (tkn.kind == Tkn::KIND_KEYWORD_PROC)
		{
			auto proc = parser_proc(&parser);
			if (src_has_err(src))
			{
				proc_free(proc);
				break;
			}
			mn::buf_push(src->decls, decl_proc_new(proc));
		}
		else if(tkn.kind == Tkn::KIND_KEYWORD_CONSTANT)
		{
			auto constant = parser_constant(&parser);
			if (src_has_err(src))
				break;
			mn::buf_push(src->decls, decl_constant_new(constant));
		}
	}

	return src_has_err(src) == false;
}
```

also we'll need to parse `debugstr` and `u64/i64` variant of the `load` instruction to accept and id which refers to a constant
```C++
if (is_load(op))
{
	ins.op = parser_eat(self);
	ins.dst = parser_reg(self);
	ins.src = parser_imm(self, op.kind == Tkn::KIND_KEYWORD_U64_LOAD || op.kind == Tkn::KIND_KEYWORD_I64_LOAD);
}
...
else if(op.kind == Tkn::KIND_KEYWORD_DEBUGSTR)
{
	ins.op = parser_eat(self);
	ins.dst = parser_reg(self);
}
```

with `parser_const` name changing to `parser_imm` and implementation is changed to
```C++
inline static Tkn
parser_imm(Parser* self, bool constant_allowed)
{
	auto op = parser_look(self);
	if (op.kind == Tkn::KIND_INTEGER ||
		op.kind == Tkn::KIND_FLOAT)
	{
		return parser_eat(self);
	}

	if (constant_allowed && op.kind == Tkn::KIND_ID)
		return parser_eat(self);

	src_err(self->src, op, mn::strf("expected a constant but found '{}'", op.str));
	return Tkn{};
}
```

now that we can parse the constants we'll need to add them to code gen
```C++
for(auto decl: src->decls)
{
	switch(decl->kind)
	{
	case Decl::KIND_PROC:
	{
		auto emitter = emitter_new(src, &globals);
		mn_defer(emitter_free(emitter));

		auto code = emitter_proc_gen(emitter, decl->proc, pkg);
		vm::pkg_proc_add(pkg, decl->proc.name.str, code);
		break;
	}

	case Decl::KIND_CONSTANT:
	{
		mn::str_clear(tmp_str);
		// replace escaped characters with their binary equivalent
		_escape_string(tmp_str, decl->constant.value.rng.begin, decl->constant.value.rng.end);

		// add this constant bytes to the package format
		vm::pkg_constant_add(
			pkg,
			mn::str_from_c(decl->constant.name.str),
			block_from(tmp_str)
		);
		break;
	}

	default:
		assert(false && "unreachable");
		break;
	}
}
```

then we'll need to add the implementation for `debugstr` and i64/u64 load instruction
```C++
case Tkn::KIND_KEYWORD_U32_LOAD:
{
	vm::push8(self.out, uint8_t(vm::Op_LOAD32));
	emitter_reg_gen(self, ins.dst);
	if(ins.src.kind == Tkn::KIND_ID)
	{
		// since out pointer size is 64 bit then show error message for all loads except 64-bit loads
		src_err(self.src, ins.src, mn::strf("unable to load the address (64-bit) into a (32-bit) wide value"));
	}
	else if (ins.src.kind == Tkn::KIND_INTEGER)
	{
		vm::push32(self.out, convert_to<uint32_t>(ins.src));
	}
	break;
}

case Tkn::KIND_KEYWORD_I64_LOAD:
{
	vm::push8(self.out, uint8_t(vm::Op_LOAD64));
	emitter_reg_gen(self, ins.dst);
	if(ins.src.kind == Tkn::KIND_ID)
	{
		// inform the package file format about the relocation
		vm::pkg_constant_reloc_add(
			pkg,
			mn::str_from_c(proc.name.str),
			self.out.count,
			mn::str_from_c(ins.src.str)
		);
		// as usual put a 0 placeholder to be patched by the loader later
		vm::push64(self.out, 0);
	}
	else if (ins.src.kind == Tkn::KIND_INTEGER)
	{
		vm::push64(self.out, uint64_t(convert_to<int64_t>(ins.src)));
	}
	break;
}
...
case Tkn::KIND_KEYWORD_DEBUGSTR:
	vm::push8(self.out, uint8_t(vm::Op_DEBUGSTR));
	emitter_reg_gen(self, ins.dst);
	break;
```

now that we have the assembler in place we'll need to go back the package file format and loader to add constants support

first we'll need to add a table for constants and a Buf for constant relocations
```C++
struct Pkg
{
	mn::Map<mn::Str, mn::Buf<uint8_t>> constants;
	mn::Map<mn::Str, mn::Buf<uint8_t>> procs;
	mn::Buf<Reloc> relocs;
	mn::Buf<Reloc> constant_relocs;
};
```

then we'll add them to the save function
```C++
// write constants count
len = uint32_t(self.constants.count);
mn::stream_write(f, mn::block_from(len));

// write each constant
for(auto it = mn::map_begin(self.constants);
	it != mn::map_end(self.constants);
	it = mn::map_next(self.constants, it))
{
	write_string(f, it->key);
	write_bytes(f, it->value);
}

// write relocs count
len = uint32_t(self.constant_relocs.count);
mn::stream_write(f, mn::block_from(len));

// write each reloc
for(const auto& reloc: self.constant_relocs)
{
	write_string(f, reloc.source_name);
	write_string(f, reloc.target_name);
	mn::stream_write(f, mn::block_from(reloc.source_offset));
}
```

and the load function
```C++
// read constants count
len = 0;
mn::stream_read(f, mn::block_from(len));
mn::map_reserve(self.constants, len);

// read each constant
for(size_t i = 0; i < len; ++i)
{
	auto name = read_string(f);
	auto bytes = read_bytes(f);
	mn::map_insert(self.constants, name, bytes);
}

// read relocs count
len = 0;
mn::stream_read(f, mn::block_from(len));
mn::buf_reserve(self.constant_relocs, len);

// read each reloc
for(size_t i = 0; i < len; ++i)
{
	Reloc reloc{};
	reloc.source_name = read_string(f);
	reloc.target_name = read_string(f);
	mn::stream_read(f, mn::block_from(reloc.source_offset));
	mn::buf_push(self.constant_relocs, reloc);
}
```

then we'll need to edit `pkg_bytecode_main_generate` and change it to `pkg_core_load` which will take in the core struct and prepare it for execution directly

```C++
void
pkg_core_load(const Pkg& self, Core& core, uint64_t stack_size_in_bytes)
{
	auto loaded_procs_table = mn::map_new<mn::Str, uint64_t>();
	mn_defer(mn::map_free(loaded_procs_table));

	// append each proc bytecode to the result bytecode array
	for(auto it = mn::map_begin(self.procs);
		it != mn::map_end(self.procs);
		it = mn::map_next(self.procs, it))
	{
		// add the proc name and offset in the loaded_procs_table
		mn::map_insert(loaded_procs_table, it->key, uint64_t(core.bytecode.count));
		mn::buf_concat(core.bytecode, it->value);
	}

	// after loading procs we'll need to perform the relocs
	for(const auto& reloc: self.relocs)
	{
		auto source_it = mn::map_lookup(loaded_procs_table, reloc.source_name);
		auto target_it = mn::map_lookup(loaded_procs_table, reloc.target_name);
		assert(source_it && target_it);
		write64(core.bytecode.ptr + source_it->value + reloc.source_offset, target_it->value);
	}

	// make a similar table for constants
	auto loaded_constants_table = mn::map_new<mn::Str, uint64_t>();
	mn_defer(mn::map_free(loaded_constants_table));

	// just like the code we'll concat each constant in top of stack memory
	for(auto it = mn::map_begin(self.constants);
		it != mn::map_end(self.constants);
		it = mn::map_next(self.constants, it))
	{
		mn::map_insert(loaded_constants_table, it->key, uint64_t(core.stack.count));
		mn::buf_concat(core.stack, it->value);
	}

	// then add the requested stack memory to the stack (default = 8MB)
	mn::buf_resize(core.stack, stack_size_in_bytes);

	// after loading constants we'll need to perform the relocs for constants
	for(const auto& reloc: self.constant_relocs)
	{
		auto source_it = mn::map_lookup(loaded_procs_table, reloc.source_name);
		auto target_it = mn::map_lookup(loaded_constants_table, reloc.target_name);
		assert(source_it && target_it);
		write64(core.bytecode.ptr + source_it->value + reloc.source_offset, uint64_t(core.stack.ptr + target_it->value));
	}

	// set the IP register to point to start of main
	auto main_it = mn::map_lookup(loaded_procs_table, mn::str_lit("main"));
	core.r[Reg_IP].u64 = main_it->value;
	// set the SP register to point to the bottom of the stack
	core.r[Reg_SP].ptr = core.stack.ptr + stack_size_in_bytes;
}
```

and we're done. now we can debug a string constant using our VM

I did a couple of fixes along the day like fixing the `write` opcode which allocated the memory in the instruction itself unlike x64 assembly where you allocate memory before writing

old behavior
```asm
i32.write sp r0
i32.sub sp 4
```

correct behavior
```asm
i32.sub sp 4
i32.write sp r0
```

let's fix that by changing the write instruction implementation
```C++
case Op_WRITE32:
{
	auto& dst = load_reg(self);
	auto& src = load_reg(self);
	if(valid_next_bytes(self, dst.ptr, sizeof(uint32_t)) == false)
	{
		self.state = Core::STATE_ERR;
		break;
	}
	*(uint32_t*)dst.ptr = src.u32;
	break;
}
...
case Op_READ32:
{
	auto& dst = load_reg(self);
	auto& src = load_reg(self);
	if(valid_next_bytes(self, src.ptr, sizeof(uint32_t)) == false)
	{
		self.state = Core::STATE_ERR;
		break;
	}
	dst.u32 = *(uint32_t*)src.ptr;
	break;
}
```

also we had a problem where global symbols could collide with local symbols
for example you could define a constant named `msg` and a proc called `msg` this should of course generate an error message so let's do in the generation step of our assembler

we'll put each global symbol into a table then we'll check it every time we define a local one
```C++
// load all global symbols into globals map and try to resolve symbol redefinition erros
auto globals = mn::map_new<const char*, Tkn>();
mn_defer(mn::map_free(globals));

for(auto decl: src->decls)
{
	switch(decl->kind)
	{
	case Decl::KIND_PROC:
	{
		if(auto it = mn::map_lookup(globals, decl->proc.name.str))
			src_err(src, decl->proc.name, mn::strf("symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
		else
			mn::map_insert(globals, decl->proc.name.str, decl->proc.name);
		break;
	}

	case Decl::KIND_CONSTANT:
	{
		if(auto it = mn::map_lookup(globals, decl->constant.name.str))
			src_err(src, decl->constant.name, mn::strf("symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
		else
			mn::map_insert(globals, decl->constant.name.str, decl->constant.name);
		break;
	}

	default:
		assert(false && "unreachable");
		break;
	}
}
```

then we'll need to check it each time we add a local symbol
```C++
inline static void
emitter_register_symbol(Emitter& self, const Tkn& label)
{
	// check global symbols
	assert(self.globals != nullptr);

	if(auto it = mn::map_lookup(*self.globals, label.str))
	{
		src_err(self.src, label, mn::strf("global symbol redefinition, it was first defined in {}:{}", it->value.pos.line, it->value.pos.col));
	}

	if (mn::map_lookup(self.symbols, label.str) == nullptr)
	{
		mn::map_insert(self.symbols, label.str, self.out.count);
	}
	else
	{
		src_err(self.src, label, mn::strf("'{}' local symbol redefinition", label.str));
	}
}
```

and that's it for today

### Day-16

Today we'll start working on C function interop, our goal is to eventually accept C procedures forward declarations
```asm
constant msg "Hello, World!\0"

; just like this
proc C.add(i32, i32)

proc main
	halt	
end
```

Today we'll work on the scanner and we'll make it accept the above code
we'll need to add the following tokens to our token listing

```C++
...
TOKEN(OPEN_PAREN, "("), \
TOKEN(CLOSE_PAREN, ")"), \
TOKEN(COMMA, ","), \
...
TOKEN(KEYWORDS__BEGIN, ""), \
TOKEN(KEYWORD_I8,  "i8"), \
TOKEN(KEYWORD_I16, "i16"), \
TOKEN(KEYWORD_I32, "i32"), \
TOKEN(KEYWORD_I64, "i64"), \
TOKEN(KEYWORD_U8,  "u8"), \
TOKEN(KEYWORD_U16, "u16"), \
TOKEN(KEYWORD_U32, "u32"), \
TOKEN(KEYWORD_U64, "u64"), \
...
```

then we'll need to add the support for our 3 non keywords tokens in the scanner
```C++
...
case '(':
	tkn.kind = Tkn::KIND_OPEN_PAREN;
	tkn.str = "(";
	no_intern = true;
	break;
case ')':
	tkn.kind = Tkn::KIND_CLOSE_PAREN;
	tkn.str = ")";
	no_intern = true;
	break;
case ',':
	tkn.kind = Tkn::KIND_COMMA;
	tkn.str = ",";
	no_intern = true;
	break;
case ';':
...
```

and that's it for today, in the next day we'll work on the parsing

### Day-17

today we'll work on adding c procedures support in the parser

first, we'll need to add the c procedure declaration to our parse tree
```C++
struct C_Proc
{
	Tkn name;
	mn::Buf<Tkn> args;
};

struct Decl
{
	enum KIND
	{
		KIND_NONE,
		KIND_PROC,
		KIND_CONSTANT,
		KIND_C_PROC,
	};

	KIND kind;
	union
	{
		Proc proc;
		Constant constant;
		C_Proc c_proc;
	};
};
```

then we'll need to parse it
```C++
inline static C_Proc
parser_c_proc(Parser* self)
{
	parser_eat_must(self, Tkn::KIND_KEYWORD_PROC);
	auto proc = c_proc_new();
	proc.name = parser_eat_must(self, Tkn::KIND_ID);

	parser_eat_must(self, Tkn::KIND_OPEN_PAREN);
	while(parser_look_kind(self, Tkn::KIND_CLOSE_PAREN) == false)
	{
		auto tkn = parser_look(self);
		if (is_type(tkn))
			mn::buf_push(proc.args, parser_eat(self));
		else
			src_err(self->src, parser_eat(self), mn::strf("expected a type token"));
		parser_eat_kind(self, Tkn::KIND_COMMA);
	}
	parser_eat_must(self, Tkn::KIND_CLOSE_PAREN);
	return proc;
}
```

and we'll need to change our main parse function to check for C procedures, all C procs start with package name which is `C` then a `.`
so for example if you have a shared library called `foo` and you want to declare proc called `bar` inside it you'd write
`proc C.foo.bar(i32, i32)`

```C++
auto proc_name = parser_look(&parser, 1);
if(mn::str_prefix(proc_name.str, "C."))
{
	auto c_proc = parser_c_proc(&parser);
	if(src_has_err(src))
	{
		c_proc_free(c_proc);
		break;
	}
	mn::buf_push(src->decls, decl_c_proc_new(c_proc));
}
else
{
	auto proc = parser_proc(&parser);
	if (src_has_err(src))
	{
		proc_free(proc);
		break;
	}
	mn::buf_push(src->decls, decl_proc_new(proc));
}
```


