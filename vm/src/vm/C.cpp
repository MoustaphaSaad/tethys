#include "vm/C.h"

namespace vm
{
	// API
	C_Proc
	c_proc_new()
	{
		C_Proc self{};
		self.lib = mn::str_new();
		self.name = mn::str_new();
		self.arg_types = mn::buf_new<C_TYPE>();
		return self;
	}

	void
	c_proc_free(C_Proc& self)
	{
		mn::str_free(self.lib);
		mn::str_free(self.name);
		mn::buf_free(self.arg_types);
	}
}