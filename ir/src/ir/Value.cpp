#include "ir/Value.h"

namespace ir
{
	Value
	value_arg(Type* type, Proc_ID proc, size_t ix)
	{
		Value self{};
		self.kind = Value::KIND_ARG;
		self.type = type;
		self.arg.proc_id = proc;
		self.arg.index = ix;
		return self;
	}

	Value
	value_ins(Type* type, Proc_ID proc, Ins_ID id)
	{
		Value self{};
		self.kind = Value::KIND_INS;
		self.type = type;
		self.ins.proc_id = proc;
		self.ins.id = id;
		return self;
	}
}