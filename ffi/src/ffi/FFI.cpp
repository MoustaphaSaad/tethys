#include "ffi/FFI.h"

#include <stddef.h>

#define FFI_TYPE_DEFINE(name, kind, type)\
struct _align_##name\
{\
	char c;\
	type x;\
};\
const struct FFI_Type ffi_##name = { kind, offsetof(_align_##name, x), sizeof(type) };

const struct FFI_Type ffi_void = { FFI_TYPE_KIND_VOID, 1, 1 };
FFI_TYPE_DEFINE(int8, FFI_TYPE_KIND_INT8, int8_t);
FFI_TYPE_DEFINE(int16, FFI_TYPE_KIND_INT16, int16_t);
FFI_TYPE_DEFINE(int32, FFI_TYPE_KIND_INT32, int32_t);
FFI_TYPE_DEFINE(int64, FFI_TYPE_KIND_INT64, int64_t);
FFI_TYPE_DEFINE(uint8, FFI_TYPE_KIND_UINT8, uint8_t);
FFI_TYPE_DEFINE(uint16, FFI_TYPE_KIND_UINT16, uint16_t);
FFI_TYPE_DEFINE(uint32, FFI_TYPE_KIND_UINT32, uint32_t);
FFI_TYPE_DEFINE(uint64, FFI_TYPE_KIND_UINT64, uint64_t);
FFI_TYPE_DEFINE(float32, FFI_TYPE_KIND_FLOAT32, float);
FFI_TYPE_DEFINE(float64, FFI_TYPE_KIND_FLOAT64, double);
FFI_TYPE_DEFINE(ptr, FFI_TYPE_KIND_POINTER, void*);

#undef FFI_TYPE_DEFINE
