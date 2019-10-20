#pragma once

#include <stdint.h>

namespace vm
{
	enum Reg: uint8_t
	{
		// General purpose registers
		Reg_R0,
		Reg_R1,
		Reg_R2,
		Reg_R3,
		Reg_R4,
		Reg_R5,
		Reg_R6,
		Reg_R7,

		// instruction pointer
		Reg_IP,

		//Count of the registers
		Reg_COUNT
	};

	union Reg_Val
	{
		int8_t   i8;
		int16_t  i16;
		int32_t  i32;
		int64_t  i64;
		uint8_t  u8;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;
	};
}