#include "ir/Proc.h"

namespace ir
{
	Proc
	proc_new(const mn::Str& name, Type *type)
	{
		Proc self{};
		self.name = mn::str_clone(name);
		self.type = type;
		self.blocks = bag_new<Basic_Block>();
		self.ins = bag_new<Ins>();
		return self;
	}

	void
	proc_free(Proc& self)
	{
		mn::str_free(self.name);
		type_free(self.type);
		bag_free(self.blocks);
		bag_free(self.ins);
	}

	Value
	proc_arg(const Proc& self, size_t ix)
	{
		if(ix >= self.type->proc.args.count)
			return Value{};
		return value_arg(self.type->proc.args[ix], self.id, ix);
	}
}