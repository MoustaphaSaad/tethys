#ifndef FFI_HEADER_H
#define FFI_HEADER_H


#include "ffi/Exports.h"

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

extern void foo(void);

enum FFI_TYPE_KIND
{
	FFI_TYPE_KIND_VOID,
	FFI_TYPE_KIND_INT8,
	FFI_TYPE_KIND_INT16,
	FFI_TYPE_KIND_INT32,
	FFI_TYPE_KIND_INT64,
	FFI_TYPE_KIND_UINT8,
	FFI_TYPE_KIND_UINT16,
	FFI_TYPE_KIND_UINT32,
	FFI_TYPE_KIND_UINT64,
	FFI_TYPE_KIND_FLOAT32,
	FFI_TYPE_KIND_FLOAT64,
	FFI_TYPE_KIND_POINTER
};

struct FFI_Type
{
	enum FFI_TYPE_KIND kind;
	uint32_t alignment;
	size_t size;
};

extern FFI_EXPORT const struct FFI_Type ffi_void;
extern FFI_EXPORT const struct FFI_Type ffi_int8;
extern FFI_EXPORT const struct FFI_Type ffi_int16;
extern FFI_EXPORT const struct FFI_Type ffi_int32;
extern FFI_EXPORT const struct FFI_Type ffi_int64;
extern FFI_EXPORT const struct FFI_Type ffi_uint8;
extern FFI_EXPORT const struct FFI_Type ffi_uint16;
extern FFI_EXPORT const struct FFI_Type ffi_uint32;
extern FFI_EXPORT const struct FFI_Type ffi_uint64;
extern FFI_EXPORT const struct FFI_Type ffi_float32;
extern FFI_EXPORT const struct FFI_Type ffi_float64;
extern FFI_EXPORT const struct FFI_Type ffi_ptr;


enum CALL_CONV
{
	CALL_CONV_DEFAULT,
	CALL_CONV_WIN64,
};


#ifdef __cplusplus
}
#endif

#endif