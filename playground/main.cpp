#include <mn/IO.h>
#include <mn/Library.h>
#include <ffi.h>
#include <ffi/FFI.h>

extern "C" int myadd(int a, int b)
{
	return a + b;
}

int
main(int, char**)
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
	return 0;
}