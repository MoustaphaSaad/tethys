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

		STATE state;
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