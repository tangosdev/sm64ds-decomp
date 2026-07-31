#include "types.h"
struct Pair { void* a; u32 idx; };

extern int func_0205d568(int r0, struct Pair p);

int func_0204eda4(int r0, char* a, u32 idx) {
    int r = 0;
    if (idx < *(u16*)(*(char**)(a + 0x54) + 8)) {
        struct Pair p;
        p.a = a;
        p.idx = idx;
        r = func_0205d568(r0, p);
    }
    return r;
}
