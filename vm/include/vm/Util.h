#pragma once

#include <mn/Buf.h>

#include <stdint.h>
#include <assert.h>

namespace vm
{
	inline static uint8_t
	pop8(const mn::Buf<uint8_t>& bytes, uint64_t& ix)
	{
		uint8_t r = bytes[ix];
		ix += sizeof(uint8_t);
		return r;
	}

	inline static uint16_t
	pop16(const mn::Buf<uint8_t>& bytes, uint64_t& ix)
	{
		assert(ix + sizeof(uint16_t) <= bytes.count);
		uint16_t r = *(uint16_t*)(bytes.ptr + ix);
		ix += sizeof(uint16_t);
		return r;
	}

	inline static uint32_t
	pop32(const mn::Buf<uint8_t>& bytes, uint64_t& ix)
	{
		assert(ix + sizeof(uint32_t) <= bytes.count);
		uint32_t r = *(uint32_t*)(bytes.ptr + ix);
		ix += sizeof(uint32_t);
		return r;
	}

	inline static uint64_t
	pop64(const mn::Buf<uint8_t>& bytes, uint64_t& ix)
	{
		assert(ix + sizeof(uint64_t) <= bytes.count);
		uint64_t r = *(uint64_t*)(bytes.ptr + ix);
		ix += sizeof(uint64_t);
		return r;
	}

	inline static void
	push8(mn::Buf<uint8_t>& bytes, uint8_t v)
	{
		mn::buf_push(bytes, v);
	}

	inline static void
	push16(mn::Buf<uint8_t>& bytes, uint16_t v)
	{
		mn::buf_push(bytes, uint8_t(v));
		mn::buf_push(bytes, uint8_t(v >> 8));
	}

	inline static void
	push32(mn::Buf<uint8_t>& bytes, uint32_t v)
	{
		mn::buf_push(bytes, uint8_t(v));
		mn::buf_push(bytes, uint8_t(v >> 8));
		mn::buf_push(bytes, uint8_t(v >> 16));
		mn::buf_push(bytes, uint8_t(v >> 24));
	}

	inline static void
	push64(mn::Buf<uint8_t>& bytes, uint64_t v)
	{
		mn::buf_push(bytes, uint8_t(v));
		mn::buf_push(bytes, uint8_t(v >> 8));
		mn::buf_push(bytes, uint8_t(v >> 16));
		mn::buf_push(bytes, uint8_t(v >> 24));
		mn::buf_push(bytes, uint8_t(v >> 32));
		mn::buf_push(bytes, uint8_t(v >> 40));
		mn::buf_push(bytes, uint8_t(v >> 48));
		mn::buf_push(bytes, uint8_t(v >> 56));
	}
}