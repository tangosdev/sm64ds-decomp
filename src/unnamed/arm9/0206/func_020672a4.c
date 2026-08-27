#include "types.h"
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 prev);

typedef struct { u32 unk[0]; } SomeObj;
extern SomeObj* data_020a9db8;  /* 0x020a9db8 */

void func_020672a4(u32 val) {
    u32 r4 = val;
    u32 saved = _ZN3IRQ7DisableEv();
    SomeObj* p = data_020a9db8;
    p = (SomeObj*)((u32)p + 0x1000);
    *(u32*)((u32)p + 0x4e4) = r4;
    _ZN3IRQ7RestoreEj(saved);
}
