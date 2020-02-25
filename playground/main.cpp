#include <mn/IO.h>
#include <mn/Library.h>
#include <mn/Defer.h>

#include <ffi.h>
#include <ffi/FFI.h>

#include <ir/Pkg.h>

extern "C" int myadd(int a, int b)
{
	return a + b;
}

void
ffi_playground()
{
	auto l = mn::library_open("");
	auto f = mn::library_proc(l, "myadd");
	mn::print("{}, {}\n", l, f);
	mn::library_close(l);
	mn::print("Hello, World!");

	ffi_cif cif;
	ffi_type* arg_types[2];
	void* arg_values[2];
	ffi_arg ret;

	int arg1 = 1;
	int arg2 = 2;

	arg_types[0] = &ffi_type_sint;
	arg_values[0] = &arg1;
	arg_types[1] = &ffi_type_sint;
	arg_values[1] = &arg2;

	auto dbg = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 2, &ffi_type_sint, arg_types);
	mn::print("ffi_prep_cif = {}\n", dbg);
	ffi_call(&cif, FFI_FN(myadd), &ret, arg_values);
	mn::print("{}\n", ret);
}

void
ir_playground()
{
	// create test package
	auto pkg = ir::pkg_new("test");
	mn_defer(ir::pkg_free(pkg));

	// create add proc with this signature `int add(int a, int b)`
	auto proc_add = ir::pkg_proc_new(pkg, "add", ir::type_proc(ir::type_int32, {ir::type_int32, ir::type_int32}));
	{
		// get the arguments
		auto a = ir::proc_arg(proc_add, 0);
		auto b = ir::proc_arg(proc_add, 1);

		// create the entry basic block of this function
		auto entry = ir::proc_basic_block_new(proc_add);

		// add the two args together
		auto res = ir::basic_block_ins_add(entry, a, b);
		// return the result
		ir::basic_block_ins_ret(entry, res);
	}
}

int
main(int, char**)
{
	ir_playground();
	return 0;
}