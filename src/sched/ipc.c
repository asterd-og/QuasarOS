#include <sched/ipc.h>

ipc_signal ipc_signals[256]; // Signals queue
u64 ipc_queue_idx;

i64 ipc_get(u64 pid) {
    if (ipc_queue_idx > 0) {
        for (u64 i = 0; i < ipc_queue_idx; i++) {
            if (ipc_signals[i].pid == 0) continue;
            if (ipc_signals[i].pid == pid) {
                return ipc_signals[i].signal;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

i64 ipc_get_ret(u64 pid) {
    if (ipc_queue_idx > 0) {
        for (u64 i = 0; i < ipc_queue_idx; i++) {
            if (ipc_signals[i].pid == 0) continue;
            if (ipc_signals[i].pid == pid) {
                return ipc_signals[i].ret;
            }
        }
        return 0;
    }
    return 0;
}

void ipc_transmit(u64 signal, u64 ret) {
    ipc_signals[ipc_queue_idx].pid = sched_get_pid();
    ipc_signals[ipc_queue_idx].signal = signal;
    ipc_signals[ipc_queue_idx].ret = ret;
    ipc_queue_idx++;
}

void ipc_dispatch(u64 pid) {
    if (ipc_queue_idx > 0) {
        for (u64 i = 0; i < ipc_queue_idx; i++) {
            if (ipc_signals[i].pid == 0) continue;
            if (ipc_signals[i].pid == pid) {
                ipc_signals[i].pid = 0;
                ipc_signals[i].signal = 0;
                ipc_signals[i].ret = 0;
                return;
            }
        }
        return;
    }
    return;
}