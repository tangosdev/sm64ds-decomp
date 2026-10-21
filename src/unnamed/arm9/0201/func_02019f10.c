extern void _ZN3IRQ7DisableEv(void);
extern void _ZN4CP1516WaitForInterruptEv(void);

void func_0201a028(void) {
    for (;;) {
        _ZN3IRQ7DisableEv();
        _ZN4CP1516WaitForInterruptEv();
    }
}

#include "types.h"
enum IPCStatus { IPC_SUCCESS = 0, IPC_ERROR = -1, IPC_FIFO_FULL = -2 };

extern enum IPCStatus IPCSend(u32 arg0, u32 arg1, u32 arg2);
extern void func_0201a028(void);

void func_02019ff4(void) {
    while (IPCSend(0xd, 2, 0) != 0) {
    }
    func_0201a028();
}

extern void func_0205f958(void);
extern void func_02019ff4(void);
extern void func_0205f5c0(int a);
extern void func_0205fa78(int a);
extern int func_02040714(void);
extern void func_02040724(void);
extern void func_0201a4d0(void);
extern unsigned char data_0209d4e4;

void func_02019f64(void) {
    int r;
    if (data_0209d4e4 != 0) {
        func_0205f958();
        func_02019ff4();
        return;
    }
    func_0205f5c0(3);
    func_0205fa78(1);
    r = func_02040714();
    if (r != 0 && r != 1) {
        do {
            func_02040724();
            func_0201a4d0();
            r = func_02040714();
            if (r == 0)
                break;
        } while (r != 1);
    }
    func_0205f5c0(1);
    func_0205fa78(1);
    func_02019ff4();
}

extern char data_02086214[];
extern void func_02019f64(void);
extern void func_02058200(void *a, void *fn, int c, void *d, int e, int f);
extern void func_02058048(void *a);

int func_02019f10(void)
{
    char *base = (char *)((int)data_02086214 - 0x94);
    func_02058200(base, (void *)func_02019f64, 0, base, 0x3f6c, 0);
    func_02058048(base);
    return 0;
}
