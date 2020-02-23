#pragma once

#include "ir/Exports.h"
#include "ir/Handles.h"

namespace ir
{
	struct Type;

	struct Value
	{
		enum KIND
		{
			KIND_NONE,
			KIND_ARG,
			KIND_INS,
			KIND_IMM,
			KIND_PROC,
		};

		KIND kind;
		Type* type;
		union
		{
			struct
			{
				Proc_ID proc_id;
				// index of the argument in the function
				size_t index;
			} arg;

			struct
			{
				Proc_ID proc_id;
				Ins_ID id;
			} ins;

			union
			{
				int8_t i8;
				int16_t i16;
				int32_t i32;
				int64_t i64;
				uint8_t u8;
				uint16_t u16;
				uint32_t u32;
				uint64_t u64;
				float f32;
				double f64;
			} imm;

			struct
			{
				Proc_ID id;
			} proc;
		};
	};

	IR_EXPORT Value
	value_arg(Type* type, Proc_ID proc, size_t ix);

	IR_EXPORT Value
	value_ins(Type* type, Proc_ID proc, Ins_ID id);
}