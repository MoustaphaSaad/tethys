#pragma once

#include "vm/Exports.h"
#include "vm/Reg.h"

#include <mn/Buf.h>

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
	};

	inline static Core
	core_new()
	{
		return Core{};
	}

	VM_EXPORT void
	core_ins_execute(Core& self, const mn::Buf<uint8_t>& code);
}