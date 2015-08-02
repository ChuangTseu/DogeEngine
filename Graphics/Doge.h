#pragma once

#include <cstdint>
#include <cassert>

#define DogeAssert(exp) assert(exp)

#define DogeAssertAlways() assert(false)

#define DogeAssertMessage(exp, message) assert(exp)

#define CONCAT_MACRO_INTERNAL(x, y) x##y
#define CONCAT_MACRO(x, y) CONCAT_MACRO_INTERNAL(x, y)

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

#define AS_U8_ADDRESSOF(var) ((u8*) &var)

#define AS_U8_PTR(ptr) ((u8*) ptr)

