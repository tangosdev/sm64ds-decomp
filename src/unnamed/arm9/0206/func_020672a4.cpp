//cpp
#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "IRQ.h"

extern "C" {

extern char *data_020a9db8; /* 0x020a9db8 */

/* NitroSDK MB_CommSetParentStateCallback(callback): store the parent state
 * callback with interrupts off. */
void func_020672a4(u32 val) {
    u32 r4 = val;
    u32 saved = IRQ::Disable();
    ((MbParentWork *)data_020a9db8)->mParentCallback = r4;
    IRQ::Restore(saved);
}
}
