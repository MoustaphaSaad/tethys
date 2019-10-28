#include <mn/IO.h>
#include <mn/Buf.h>
#include <mn/Defer.h>
#include <mn/Path.h>

#include <as/Src.h>
#include <as/Scan.h>
#include <as/Parse.h>
#include <as/Gen.h>

#include <vm/Core.h>

const char* HELP_MSG = R"MSG(tas tethys assembler
tas [command] [targets] [flags]
COMMANDS:
  help: prints this message
    'tas help'
  scan: scans the file
    'tas scan path/to/file.zy'
  parse: parses the file
    'tas parse path/to/file.zy'
  build: builds the file
    'tas build -o pkg_name.zyc path/to/file.zy'
  run: loads and runs the specified package
    'tas run path/to/pkg_name.zyc'
FLAGS:
  -o: specifies output file
    'tas build -o pkg.zyc path/to/file.zy'
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
	mn::Str out_name;
};

inline static bool
args_parse(Args& self, int argc, char** argv)
{
	if(argc < 2)
	{
		return false;
	}

	self.command = mn::str_from_c(argv[1]);
	for(size_t i = 2; i < size_t(argc); ++i)
	{
		if(::strcmp(argv[i], "-o") == 0)
		{
			if(i + 1 >= argc)
			{
				mn::printerr("you need to specify output name\n");
				return false;
			}

			mn::str_free(self.out_name);
			self.out_name = mn::str_from_c(argv[i + 1]);
			++i;
		}
		else if (mn::str_prefix(argv[i], "--"))
		{
			buf_push(self.flags, mn::str_from_c(argv[i] + 2));
		}
		else if (mn::str_prefix(argv[i], "-"))
		{
			buf_push(self.flags, mn::str_from_c(argv[i] + 1));
		}
		else
		{
			buf_push(self.targets, mn::str_from_c(argv[i]));
		}
	}
	return true;
}

inline static Args
args_new()
{
	Args self{};
	self.command = mn::str_new();
	self.targets = mn::buf_new<mn::Str>();
	self.flags = mn::buf_new<mn::Str>();
	self.out_name = mn::str_from_c("pkg.zyc");
	return self;
}

inline static void
args_free(Args& self)
{
	mn::str_free(self.command);
	destruct(self.targets);
	destruct(self.flags);
	mn::str_free(self.out_name);
}

inline static bool
args_has_flag(Args& self, const char* search)
{
	for(const mn::Str& f: self.flags)
		if(f == search)
			return true;
	return false;
}

int
main(int argc, char** argv)
{
	Args args = args_new();
	mn_defer(args_free(args));

	if(args_parse(args, argc, argv) == false)
	{
		print_help();
		return -1;
	}

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

		if(as::scan(src) == false)
		{
			mn::printerr("{}", as::src_errs_dump(src, mn::memory::tmp()));
			return -1;
		}

		mn::print("{}", as::src_tkns_dump(src, mn::memory::tmp()));
		return 0;
	}
	else if(args.command == "parse")
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

		mn::print("{}", as::proc_dump(src, mn::memory::tmp()));
		return 0;
	}
	else if(args.command == "build")
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

		auto pkg = as::src_gen(src);
		mn_defer(vm::pkg_free(pkg));

		vm::pkg_save(pkg, args.out_name);
		return 0;
	}
	else if(args.command == "run")
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

		auto pkg = vm::pkg_load(args.targets[0].ptr);
		mn_defer(vm::pkg_free(pkg));

		auto code = vm::pkg_load_proc(pkg, "main");
		mn_defer(mn::buf_free(code));

		auto cpu = vm::core_new();
		while (cpu.state == vm::Core::STATE_OK)
			vm::core_ins_execute(cpu, code);

		mn::print("R0 = {}\n", cpu.r[vm::Reg_R0].i32);
		return 0;
	}
	return 0;
}