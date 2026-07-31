#include "types.h"
u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void func_0205b470(void *arg);

void func_0205aed0(void *arg0, void *arg1) {
    u32 r4 = _ZN3IRQ7DisableEv();
    func_0205b470(arg1);
    _ZN3IRQ7RestoreEj(r4);
}
