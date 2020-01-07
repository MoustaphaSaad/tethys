#pragma once

#include "vm/Exports.h"
#include "vm/Reg.h"
#include "vm/C.h"

#include <mn/Buf.h>
#include <mn/Library.h>

namespace vm
{
	struct Core
	{
		enum STATE
		{
			STATE_OK,
			STATE_HALT,
			STATE_ERR
		};

		enum CMP
		{
			CMP_NONE,
			CMP_LESS,
			CMP_EQUAL,
			CMP_GREATER
		};

		STATE state;
		// any compare result will be put here
		CMP cmp;
		Reg_Val r[Reg_COUNT];

		mn::Buf<uint8_t> bytecode;
		mn::Buf<uint8_t> stack;

		mn::Buf<mn::Library> c_libraries;
		mn::Buf<void*> c_procs_address;
		mn::Buf<C_Proc> c_procs_desc;
	};

	VM_EXPORT Core
	core_new();

	VM_EXPORT void
	core_free(Core& self);

	inline static void
	destruct(Core& self)
	{
		core_free(self);
	}

	VM_EXPORT void
	core_ins_execute(Core& self);
}