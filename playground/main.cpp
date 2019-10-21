#include <mn/IO.h>
#include <mn/Buf.h>
#include <mn/Defer.h>

#include <vm/Core.h>
#include <vm/Util.h>
#include <vm/Op.h>

#include <as/Src.h>
#include <as/Scan.h>

void
vm_play()
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
}

void
as_play()
{
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

	mn::print("{}", as::src_tkns_dump(src, mn::memory::tmp()));
}

int
main(int, char**)
{
	as_play();
	return 0;
}