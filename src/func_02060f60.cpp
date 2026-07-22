//cpp
extern "C" int func_0205ba3c(int bit, int word);
extern "C" void func_02059d8c(int x);
extern "C" int IPCSend(unsigned int a, unsigned int c, unsigned int b);

typedef unsigned short u16;
extern "C" void func_020580f0(u16 *x);

namespace CP15 {
    void FlushAndInvalidateDataCache(unsigned int a, unsigned int b);
    void DrainWriteBuffer();
}
namespace IRQ {
    unsigned int Disable();
    void Restore(unsigned int x);
}

struct T {
    int *f0;
    int f4;
    char pad[0x2c];
    int f34;
};

#pragma opt_loop_invariants off
extern "C" int func_02060f60(T *thiz, int v, int count) {
    unsigned int saved;
    int r;

    if (!(*(volatile int *)&thiz->f34 & 2)) {
        int *p = (int *)(((int)thiz + 0x34) & 0xFFFFFFFFFFFFFFFFLL);
        *p |= 2;
        if (func_0205ba3c(0xb, 1) == 0) {
            do {
                func_02059d8c(0x64);
            } while (func_0205ba3c(0xb, 1) == 0);
        }
        func_02060f60(thiz, 0, 1);
    }

    CP15::FlushAndInvalidateDataCache((unsigned int)thiz->f0, 0x20);
    CP15::DrainWriteBuffer();

    do {
        thiz->f4 = v;
        *(int *)(((int)thiz + 0x34) & 0xFFFFFFFFFFFFFFFFLL) |= 0x20;

        while (IPCSend(0xb, v, 1) < 0) {
        }

        if (v == 0) {
            int *q = thiz->f0;
            while (IPCSend(0xb, (unsigned int)q, 1) < 0) {
            }
        }

        saved = IRQ::Disable();
        while (thiz->f34 & 0x20) {
            func_020580f0((u16 *)0);
        }
        IRQ::Restore(saved);

        r = *thiz->f0;
        if (r != 4) {
            break;
        }
        count--;
    } while (count > 0);

    return r == 0;
}
