#include <stdint.h>
#include <arch/x86_64/cpu/serial.h>

/*

CREDITS: xyve7 on github
https://github.com/xyve7/mubsan

*/
/*
const char* const mubsan_type_check_kinds[] = {
    "load of",
    "store to",
    "reference binding to",
    "member access within",
    "member call on",
    "constructor call on",
    "downcast of",
    "downcast of",
    "upcast of",
    "cast to virtual base of",
    "_Nonnull binding to",
    "dynamic operation on"};

typedef struct {
    const char* file;
    uint32_t line;
    uint32_t col;
} mubsan_source_location;

typedef struct {
    uint16_t kind;
    uint16_t info;
    char name[];
} mubsan_type_description;

typedef struct {
    mubsan_source_location loc;
    mubsan_type_description* type;
    uint8_t alignment;
    uint8_t check_kind;
} mubsan_type_mismatch_info_v1;

typedef struct {
    mubsan_source_location loc;
    mubsan_type_description* type;
} mubsan_overflow;

typedef struct {
    mubsan_source_location loc;
} mubsan_pointer_overflow;

typedef struct {
    mubsan_source_location loc;
    mubsan_type_description* array_type;
    mubsan_type_description* index_type;
} mubsan_out_of_bounds;

typedef struct {
    mubsan_source_location loc;
    //mubsan_source_location attr_loc;
    //int arg_index;
} mubsan_not_null_arg;

typedef struct {
    mubsan_source_location loc;
    mubsan_type_description* type;
} mubsan_invalid_value;

typedef struct {
    mubsan_source_location loc;
    mubsan_type_description* lhs_type;
    mubsan_type_description* rhs_type;
} mubsan_shift_out_of_bounds;
typedef struct {
    mubsan_source_location loc;
} mubsan_unreachable;
void __ubsan_handle_type_mismatch_v1(mubsan_type_mismatch_info_v1* data, uintptr_t ptr) {
    const char* reason = "type mismatch";

    if (ptr == 0) {
        reason = "dereference of a null pointer";
    } else if (data->alignment && (ptr & (data->alignment - 1))) {
        reason = "use of a misaligned pointer";
    }

    serial_printf("mubsan @ line %u, column %u, file %s: %s, %s type %s at alignment %u at address 0x%lx\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               reason,
               mubsan_type_check_kinds[data->check_kind],
               data->type->name,
               data->alignment,
               ptr);
}
void __ubsan_handle_add_overflow(mubsan_overflow* data, uintptr_t lhs, uintptr_t rhs) {
    serial_printf("mubsan @ line %u, column %u, file %s: addition overflow, for type %s, expression %lu + %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->type->name,
               lhs,
               rhs);
}
void __ubsan_handle_sub_overflow(mubsan_overflow* data, uintptr_t lhs, uintptr_t rhs) {
    serial_printf("mubsan @ line %u, column %u, file %s: subtraction overflow, for type %s, expression %lu - %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->type->name,
               lhs,
               rhs);
}
void __ubsan_handle_mul_overflow(mubsan_overflow* data, uintptr_t lhs, uintptr_t rhs) {
    serial_printf("mubsan @ line %u, column %u, file %s: multiplication overflow, for type %s, expression %lu * %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->type->name,
               lhs,
               rhs);
}
void __ubsan_handle_negate_overflow(mubsan_overflow* data, uintptr_t val) {
    serial_printf("mubsan @ line %u, column %u, file %s: negate overflow, for type %s, value %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->type->name,
               val);
}
void __ubsan_handle_divrem_overflow(mubsan_overflow* data, uintptr_t lhs, uintptr_t rhs) {
    serial_printf("mubsan @ line %u, column %u, file %s: divistion overflow, for type %s, expression %lu / %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->type->name,
               lhs,
               rhs);
}
void __ubsan_handle_pointer_overflow(mubsan_pointer_overflow* data, uintptr_t base, uintptr_t result) {
    serial_printf("mubsan @ line %u, column %u, file %s: pointer overflow, base 0x%lx, result 0x%lx\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               base,
               result);
}
void __ubsan_handle_out_of_bounds(mubsan_out_of_bounds* data, uintptr_t index) {
    serial_printf("mubsan @ line %u, column %u, file %s: array out of bounds, for type %s, by index type %s %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->array_type->name,
               data->index_type->name,
               index);
}
void __ubsan_handle_nonnull_arg(mubsan_not_null_arg* data) {
    serial_printf("mubsan @ line %u, column %u, file %s: not-null argument is null\n",
               data->loc.line,
               data->loc.col,
               data->loc.file);
}
void __ubsan_handle_load_invalid_value(mubsan_invalid_value* data, uintptr_t val) {
    serial_printf("mubsan @ line %u, column %u, file %s: load of invalid value, for type %s, value %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->type->name,
               val);
}
void __ubsan_handle_shift_out_of_bounds(mubsan_shift_out_of_bounds* data, uintptr_t lhs, uintptr_t rhs) {
    serial_printf("mubsan @ line %u, column %u, file %s: shift out of bounds, of type %s and %s, value %lu and %lu\n",
               data->loc.line,
               data->loc.col,
               data->loc.file,
               data->lhs_type->name,
               data->rhs_type->name,
               lhs,
               rhs);
}
void __ubsan_handle_builtin_unreachable(mubsan_unreachable* data) {
    serial_printf("mubsan @ line %u, column %u, file %s: unreachable code was reached\n",
               data->loc.line,
               data->loc.col,
               data->loc.file);
}
*/