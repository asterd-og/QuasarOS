void __stack_chk_fail(void) {
    int vec = 0;
    char* str = "Stack smashing detected.\n";
    asm volatile (
        "int $0x30\n\t"
        : "+a" (vec), "+b" (str), "+c" (vec), "+d" (vec) // I/O operands
        : 
        : "memory" // probably
    );
    while (1) {
        ;
    }
}

void  __ltdf2(void) {}
void __gtdf2(void) {}
void __nedf2(void) {}
void __divdf3(void) {}
void  __unorddf2(void) {}
void __floatdidf(void) {}
void  __adddf3(void) {}
void __muldf3(void) {}
void __floatsidf(void) {}
void __gedf2(void) {}
void __fixdfsi(void) {}
void __ledf2(void) {}
void __eqdf2(void) {}
void __subdf3(void) {}
void __fixdfdi(void) {}
void __fixunsdfdi(void) {}
void __floatundidf(void) {}
void __gxx_personality_v0(void) {}
void __cxa_begin_catch(void) {}
void __cxa_atexit(void) {}
void _Unwind_Resume(void) {}
void __clang_call_terminate(void) {}
void __extendsfdf2(void) {}
void __ubsan_handle_type_mismatch_v1(void) {}
void __ubsan_handle_pointer_overflow(void) {}
void __ubsan_handle_load_invalid_value(void) {}
void __ubsan_handle_out_of_bounds(void) {}
void __ubsan_handle_shift_out_of_bounds(void) {}
void __ubsan_handle_sub_overflow(void) {}
void __ubsan_handle_add_overflow(void) {}
void __ubsan_handle_divrem_overflow(void) {}
void __ubsan_handle_negate_overflow(void) {}
void __ubsan_handle_mul_overflow(void) {}
void __ubsan_handle_nonnull_arg(void) {}