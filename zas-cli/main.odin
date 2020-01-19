package main

import "core:fmt"
import "core:os"
import "../amon"
import "../zas"

HELP_MSG :: `zas zay assembler
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
    'tas build -o pkg.zyc path/to/file.zy'`;

Args :: struct {
	cmd: string,
	targets: [dynamic]string,
	out: string,
}

args_delete :: proc(self: ^Args) {
	delete(self.targets);
}

args_parse :: proc(self: ^Args) -> bool {
	if len(os.args) < 2 do return false;

	self.cmd = os.args[1];
	for i := 2; i < len(os.args); i += 1 {
		if os.args[i] == "-o" {
			if i + 1 >= len(os.args) {
				fmt.eprintln("you need to specify output name");
				return false;
			}

			self.out = os.args[i + 1];
			i += 1;
		} else {
			append(&self.targets, os.args[i]);
		}
	}
	return true;
}

main :: proc() {
	context.allocator = amon.loc_leak_allocator();
	defer amon.loc_leak_detect();

	args: Args;
	if args_parse(&args) == false {
		fmt.println(HELP_MSG);
		os.exit(-1);
	}
	defer args_delete(&args);

	exit_code := 0;

	switch args.cmd {
	case "help":
		fmt.println(HELP_MSG);

	case "scan":
		if len(args.targets) == 0 {
			fmt.eprintln("no input files\n");
			exit_code = -1;
			break;
		} else if len(args.targets) > 1 {
			fmt.eprintln("multiple input files are not supported yet\n");
			exit_code = -1;
			break;
		}

		src, ok := zas.src_from_file(args.targets[0]);
		if ok == false {
			fmt.eprintf("error while reading the file '%v'\n", args.targets[0]);
			exit_code = -1;
			break;
		}
		defer zas.src_delete(&src);

		if zas.src_scan(&src) == false {
			fmt.eprintln(src.errs);
			exit_code = -1;
			break;
		}

		tkns := zas.src_tkns_dump(&src);
		defer delete(tkns);

		fmt.println(tkns);

	case "parse":
		if len(args.targets) == 0 {
			fmt.eprintln("no input files\n");
			exit_code = -1;
			break;
		} else if len(args.targets) > 1 {
			fmt.eprintln("multiple input files are not supported yet\n");
			exit_code = -1;
			break;
		}

		src, ok := zas.src_from_file(args.targets[0]);
		if ok == false {
			fmt.eprintf("error while reading the file '%v'\n", args.targets[0]);
			exit_code = -1;
			break;
		}
		defer zas.src_delete(&src);

		if zas.src_scan(&src) == false {
			fmt.eprintln(src.errs);
			exit_code = -1;
			break;
		}

		if zas.src_parse(&src) == false {
			fmt.eprintln(src.errs);
			os.exit(-1);
		}

		decls := zas.src_decls_dump(&src);
		defer delete(decls);

		fmt.println(decls);
	}

	if exit_code != 0 do os.exit(exit_code);
}