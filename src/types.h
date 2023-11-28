#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#define divRoundUp(x, y) (x + ( y - 1)) / y
#define alignUp(x, y) divRoundUp(x, y) * y
#define alignDown(x, y) (x / y) * y

#define pageSize 4096

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uintptr_t uptr;
typedef intptr_t iptr;

typedef uint64_t usize;
typedef int64_t isize;