#include "ir/Value.h"

namespace ir
{
	Value
	value_arg(Type* type, Proc* proc, size_t ix)
	{
		Value self{};
		self.kind = Value::KIND_ARG;
		self.type = type;
		self.arg.proc = proc;
		self.arg.index = ix;
		return self;
	}

	Value
	value_ins(Type* type, Ins* ins)
	{
		Value self{};
		self.kind = Value::KIND_INS;
		self.type = type;
		self.ins = ins;
		return self;
	}
}