#include "types.h"
extern u32 func_0205b988(u32 a, u32 b, u32 c);
extern void Crash(void);

void func_0206a600(void) {
    u32 r6 = 0xd;
    u32 r5 = 2;
    u32 r4 = 0;
    u32 r0;
    do {
        r0 = func_0205b988(r6, r5, r4);
    } while (r0 != 0);
    Crash();
}
