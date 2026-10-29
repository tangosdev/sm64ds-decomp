#include "types.h"

extern int (*data_020a9dec)(int);
extern void func_0206a600(void);
extern void Crash(void);
extern u32 IPCSend(u32 a, u32 b, u32 c);

void func_0206a634(int unused, int arg) {
    if ((arg & 0x3f) == 0x11) {
        int r = 0;
        if (data_020a9dec)
            r = data_020a9dec(0);
        if (r == 0)
            return;
        func_0206a600();
    } else {
        Crash();
    }
}

void func_0206a600(void) {
    u32 r6 = 0xd;
    u32 r5 = 2;
    u32 r4 = 0;
    u32 r0;
    do {
        r0 = IPCSend(r6, r5, r4);
    } while (r0 != 0);
    Crash();
}
