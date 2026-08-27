#include "types.h"
extern u32  _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void func_02068a74(void);

/* func_02068a54 - call func_02068a74 with IRQ disabled */
void func_02068a54(void)
{
    u32 saved = _ZN3IRQ7DisableEv();
    func_02068a74();
    _ZN3IRQ7RestoreEj(saved);
}