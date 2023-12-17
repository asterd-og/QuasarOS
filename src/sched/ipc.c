#include <sched/ipc.h>

ipc_signal ipc_signals[256]; // Signals queue
u64 ipc_queue_idx;

u64 ipc_get(u64 pid) {
    if (ipc_queue_idx > 0) {
        for (u64 i = 0; i < ipc_queue_idx; i++) {
            if (ipc_signals[i].pid == 0) continue;
            if (ipc_signals[i].pid == pid) {
                return ipc_signals[i].signal;
            }
            return -1;
        }
        return -1;
    }
    return -1;
}

u64 ipc_get_ret(u64 pid) {
    if (ipc_queue_idx > 0) {
        for (u64 i = 0; i < ipc_queue_idx; i++) {
            if (ipc_signals[i].pid == 0) continue;
            if (ipc_signals[i].pid == pid) {
                return ipc_signals[i].ret;
            }
            return -1;
        }
        return -1;
    }
    return -1;
}

void ipc_transmit(u64 signal, u64 ret) {
    ipc_signals[ipc_queue_idx].pid = sched_get_pid();
    ipc_signals[ipc_queue_idx].signal = signal;
    ipc_signals[ipc_queue_idx].ret = ret;
    ipc_queue_idx++;
}