#include <mn/IO.h>
#include <mn/Buf.h>

#include <vm/Core.h>
#include <vm/Util.h>
#include <vm/Op.h>

int
main(int, char**)
{
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

	auto cpu = vm::core_new();
	while (cpu.state == vm::Core::STATE_OK)
		vm::core_ins_execute(cpu, code);

	mn::print("R0 = {}\n", cpu.r[vm::Reg_R0].i32);

	mn::buf_free(code);
	return 0;
}