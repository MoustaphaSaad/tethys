#include <mn/IO.h>
#include <mn/Library.h>

#include <ffi.h>
#include <ffi/FFI.h>

#include <ir/Bag.h>
#include <ir/Ins.h>

#include <mn/Pool.h>

#include <chrono>

extern "C" int myadd(int a, int b)
{
	return a + b;
}

void
pool_vs_bag()
{
	{
		auto b = ir::bag_new<ir::Ins>();
		auto start = std::chrono::high_resolution_clock::now();
		for(size_t i = 0; i < 1000000; ++i)
		{
			if(i % 2 == 0)
				ir::bag_insert(b, ir::ins_add(ir::value_imm(1), ir::value_imm(2)));
			else
				ir::bag_insert(b, ir::ins_ret(ir::value_imm(0)));
		}
		auto end = std::chrono::high_resolution_clock::now();
		mn::print("bag = {} milliseconds\n", (end - start).count() / 1000000);
		ir::bag_free(b);
	}

	{
		auto b = mn::pool_new(sizeof(ir::Ins), 1024);
		auto start = std::chrono::high_resolution_clock::now();
		for(size_t i = 0; i < 1000000; ++i)
		{
			auto ins = (ir::Ins*)mn::pool_get(b);
			if(i % 2 == 0)
				*ins = ir::ins_add(ir::value_imm(1), ir::value_imm(2));
			else
				*ins = ir::ins_ret(ir::value_imm(0));
		}
		auto end = std::chrono::high_resolution_clock::now();
		mn::print("pool = {} milliseconds\n", (end - start).count() / 1000000);
		mn::pool_free(b);
	}
}

int
main(int, char**)
{
	pool_vs_bag();
	return 0;
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