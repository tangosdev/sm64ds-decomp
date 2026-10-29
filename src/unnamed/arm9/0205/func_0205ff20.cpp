//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

extern u32 data_020a8114;  /* 0x020a8114 */

u32 func_0205ff20(void) {
    u32 saved = IRQ::Disable();
    u32 r2 = data_020a8114;
    if (r2 != 0) {
        IRQ::Restore(saved);
        return 0;
    }
    data_020a8114 = 1;
    IRQ::Restore(saved);
    return 1;
}
}
