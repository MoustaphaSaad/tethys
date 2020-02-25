#include "ir/Pkg.h"

namespace ir
{
	Pkg
	pkg_new(const mn::Str& name)
	{
		Pkg self{};
		self.name = mn::str_clone(name);
		self.proc_pool = mn::pool_new(sizeof(Proc), 64);
		self.procs = mn::buf_new<Proc*>();
		return self;
	}

	void
	pkg_free(Pkg& self)
	{
		for(auto p: self.procs)
			pkg_proc_free(self, p);
		mn::buf_free(self.procs);

		mn::pool_free(self.proc_pool);
		mn::str_free(self.name);
	}

	Proc*
	pkg_proc_new(Pkg& self, const mn::Str& name, Type* type)
	{
		auto proc = (Proc*)mn::pool_get(self.proc_pool);
		proc->name = mn::str_clone(name);
		proc->type = type;
		proc->ins_pool = mn::pool_new(sizeof(Ins), 64);
		proc->block_pool = mn::pool_new(sizeof(Basic_Block), 64);
		proc->blocks = mn::buf_new<Basic_Block*>();

		mn::buf_push(self.procs, proc);
		return proc;
	}

	void
	pkg_proc_free(Pkg& self, Proc* proc)
	{
		for(auto b: proc->blocks)
			proc_basic_block_free(proc, b);
		mn::buf_free(proc->blocks);

		mn::pool_free(proc->block_pool);
		mn::pool_free(proc->ins_pool);
		type_free(proc->type);
		mn::str_free(proc->name);
		mn::pool_put(self.proc_pool, proc);
	}

	Value
	proc_arg(Proc* self, size_t ix)
	{
		if(ix >= self->type->proc.args.count)
			return Value{};
		return value_arg(self->type->proc.args[ix], self, ix);
	}

	Basic_Block*
	proc_basic_block_new(Proc* proc)
	{
		auto bb = (Basic_Block*)mn::pool_get(proc->block_pool);
		bb->proc = proc;
		bb->code = mn::buf_new<Ins*>();

		mn::buf_push(proc->blocks, bb);
		return bb;
	}

	void
	proc_basic_block_free(Proc* self, Basic_Block* bb)
	{
		assert(self == bb->proc);
		mn::buf_free(bb->code);
		mn::pool_put(self->block_pool, bb);
	}

	Value
	basic_block_ins_add(Basic_Block* bb, Value a, Value b)
	{
		auto ins = (Ins*)mn::pool_get(bb->proc->ins_pool);
		*ins = ins_add(a, b);
		mn::buf_push(bb->code, ins);

		auto type = ins->op_add.a.type;
		return value_ins(type, ins);
	}

	Value
	basic_block_ins_ret(Basic_Block* bb, Value v)
	{
		auto ins = (Ins*)mn::pool_get(bb->proc->ins_pool);
		*ins = ins_ret(v);
		mn::buf_push(bb->code, ins);

		auto type = ins->op_ret.type;
		return value_ins(type, ins);
	}
}