#include "types.h"
struct State;
extern struct State data_ov002_02110034;
extern int func_ov002_020d600c(void* p);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, struct State* st);

int func_ov002_020d5c6c(char* c)
{
    void* p;
    u16 f = *(u16*)(c + 0x6ce) & 2;
    if (f == 0)
        return 0;
    p = *(void**)(c + 0x360);
    if (p != 0) {
        if (func_ov002_020d600c(p))
            goto change;
    }
    *(u16*)(void*)(int)(c + 0x6ce) &= ~2;
    return 0;
change:
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110034);
    return 1;
}
