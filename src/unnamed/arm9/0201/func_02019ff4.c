#include "types.h"
enum IPCStatus {
    IPC_SUCCESS = 0,
    IPC_ERROR = -1,
    IPC_FIFO_FULL = -2
};

extern enum IPCStatus IPCSend(u32 arg0, u32 arg1, u32 arg2);
extern void func_0201a028(void);

void func_02019ff4(void) {
    while (IPCSend(0xd, 2, 0) != 0) {}
    func_0201a028();
}
