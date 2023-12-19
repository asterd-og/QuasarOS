#pragma once

#include <types.h>
#include <sched/sched.h>

enum {
    SIGKILL = 1,
    SIGSEGV = 2
};

typedef struct {
    u64 pid;
    i64 ret;
    u64 signal;
} ipc_signal;

i64 ipc_get(u64 pid);
i64 ipc_get_ret(u64 pid);
void ipc_transmit(u64 signal, u64 ret);
void ipc_dispatch(u64 pid);