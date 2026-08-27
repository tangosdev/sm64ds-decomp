#include "types.h"
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 prev);

extern u32 data_020a8114;  /* 0x020a8114 */

u32 func_0205ff20(void) {
    u32 saved = _ZN3IRQ7DisableEv();
    u32 r2 = data_020a8114;
    if (r2 != 0) {
        _ZN3IRQ7RestoreEj(saved);
        return 0;
    }
    data_020a8114 = 1;
    _ZN3IRQ7RestoreEj(saved);
    return 1;
}
