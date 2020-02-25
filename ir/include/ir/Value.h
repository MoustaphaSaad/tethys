#pragma once

#include "ir/Exports.h"
#include "ir/Type.h"

namespace ir
{
	struct Proc;
	struct Ins;

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
				Proc* proc;
				// index of the argument in the function
				size_t index;
			} arg;

			Ins* ins;

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
				Proc* proc;
			} proc;
		};
	};

	IR_EXPORT Value
	value_arg(Type* type, Proc* proc, size_t ix);

	IR_EXPORT Value
	value_ins(Type* type, Ins* ins);

	template<typename T>
	inline static Value
	value_imm(T v)
	{
		Value self{};
		if constexpr (std::is_same_v<T, int8_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_int8;
			self.imm.i8 = v;
		}
		else if constexpr (std::is_same_v<T, int16_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_int16;
			self.imm.i16 = v;
		}
		else if constexpr (std::is_same_v<T, int32_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_int32;
			self.imm.i32 = v;
		}
		else if constexpr (std::is_same_v<T, int64_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_int64;
			self.imm.i64 = v;
		}
		else if constexpr (std::is_same_v<T, uint8_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_uint8;
			self.imm.i8 = v;
		}
		else if constexpr (std::is_same_v<T, uint16_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_uint16;
			self.imm.i16 = v;
		}
		else if constexpr (std::is_same_v<T, uint32_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_uint32;
			self.imm.i32 = v;
		}
		else if constexpr (std::is_same_v<T, uint64_t>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_uint64;
			self.imm.i64 = v;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_float32;
			self.imm.f32 = v;
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			self.kind = Value::KIND_IMM;
			self.type = type_float64;
			self.imm.f64 = v;
		}
		else
		{
			static_assert(sizeof(T) == 0, "unsupported immediate value");
		}
		return self;
	}
}