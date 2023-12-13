#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#define div_round_up(x, y) (x + ( y - 1)) / y
#define align_up(x, y) div_round_up(x, y) * y
#define align_down(x, y) (x / y) * y

#define page_size 4096

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

typedef char symbol[];