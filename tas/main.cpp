#include <mn/IO.h>
#include <mn/Buf.h>
#include <mn/Defer.h>
#include <mn/Path.h>

#include <as/Src.h>
#include <as/Scan.h>
#include <as/Parse.h>

const char* HELP_MSG = R"MSG(tas tethys assembler
tas [command] [targets] [flags]
COMMANDS:
  help: prints this message
    'tas help'
  scan: scans the file
    'tas scan path/to/file.zy'
  parse: parses the file
    'tas parse path/to/file.zy'
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
	if(argc < 2)
	{
		print_help();
		return;
	}

	self.command = mn::str_from_c(argv[1]);
	for(size_t i = 2; i < size_t(argc); ++i)
	{
		if(mn::str_prefix(argv[i], "--"))
			buf_push(self.flags, mn::str_from_c(argv[i] + 2));
		else if(mn::str_prefix(argv[i], "-"))
			buf_push(self.flags, mn::str_from_c(argv[i] + 1));
		else
			buf_push(self.targets, mn::str_from_c(argv[i]));
	}
}

inline static Args
args_new(int argc, char** argv)
{
	Args self{};
	self.command = mn::str_new();
	self.targets = mn::buf_new<mn::Str>();
	self.flags = mn::buf_new<mn::Str>();
	args_parse(self, argc, argv);
	return self;
}

inline static void
args_free(Args& self)
{
	mn::str_free(self.command);
	destruct(self.targets);
	destruct(self.flags);
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
	Args args = args_new(argc, argv);
	mn_defer(args_free(args));

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
	return 0;
}