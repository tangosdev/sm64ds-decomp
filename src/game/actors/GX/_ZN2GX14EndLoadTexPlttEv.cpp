//cpp
// @symbol _ZN2GX14EndLoadTexPlttEv
/* GX::EndLoadTexPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
extern u32 data_02099fd0;  /* RENDER_DMA_CHANNEL: 0x02099fd0 */
extern u32 data_020a60b4; /* 0x020a60b4 */
extern u32 data_020a60b0; /* 0x020a60b0 */

extern "C" {
extern void func_02059fa8(u32 dmaId);
extern void _ZN2GX17SetBankForTexPlttEt(u32 bank);
}

namespace GX {
void EndLoadTexPltt(){
    u32 dmaId = data_02099fd0;
    if (dmaId != (u32)-1) {
        func_02059fa8(dmaId);
    }
    _ZN2GX17SetBankForTexPlttEt(data_020a60b4);
    data_020a60b4 = 0;
    data_020a60b0 = 0;
}
}
