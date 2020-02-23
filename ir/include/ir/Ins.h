#pragma once

#include "ir/Bag.h"
#include "ir/Value.h"

namespace ir
{
	struct Ins
	{
		enum OP
		{
			OP_IGL,
			OP_ADD,
			OP_RET,
		};

		OP op;
		union
		{
			struct
			{
				Value a, b;
			} op_add;

			Value op_ret;
		};
	};

	inline static Ins
	ins_add(Value a, Value b)
	{
		Ins self{};
		self.op = Ins::OP_ADD;
		self.op_add.a = a;
		self.op_add.b = b;
		return self;
	}

	inline static Ins
	ins_ret(Value a)
	{
		Ins self{};
		self.op = Ins::OP_RET;
		self.op_ret = a;
		return self;
	}
}