#pragma once

#include "ir/Bag.h"

namespace ir
{
	struct Proc;
	using Proc_Bag = Bag<Proc>;
	using Proc_ID  = typename Bag<Proc>::Handle;

	struct Basic_Block;
	using Basic_Block_Bag = Bag<Basic_Block>;
	using Basic_Block_ID  = typename Bag<Basic_Block>::Handle;

	struct Ins;
	using Ins_Bag = Bag<Ins>;
	using Ins_ID  = typename Bag<Ins>::Handle;
}