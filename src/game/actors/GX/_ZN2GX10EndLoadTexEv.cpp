//cpp
// @symbol _ZN2GX10EndLoadTexEv
/* GX::EndLoadTex() at 0x020569e0 (arm9, size 0x70)
 * Ends a banked texture upload: waits on the upload DMA channel if one is active,
 * restores the texture bank, then clears the four upload-window globals.
 *
 * Language-mode migration only (phase 1 of notes/plan-cpp-language-mode.md). GX is a
 * namespace, not a class -- no `this`, no vtable, no layout -- so the symbol is now
 * mangled by the compiler from GX::EndLoadTex() rather than spelled by hand. The
 * callee declarations are left exactly as the C version had them, including the raw
 * mangled name for GX::SetBankForTex: repairing those is per-reference work (phase 3),
 * and mixing it in here would put two independent codegen variables into one
 * byte-verification.
 */
#include "types.h"

extern "C" {
extern void func_02059fa8(s32 channel);
extern void _ZN2GX13SetBankForTexEt(u32 tex);
}

extern s32 data_02099fd0;    /* 0x02099fd0 */
extern u32 data_020a60b8;            /* 0x020a60b8 */
extern u32 data_020a60ac;           /* 0x020a60ac */
extern u32 data_020a60bc;           /* 0x020a60bc */
extern u32 data_020a60c0;           /* 0x020a60c0 */

namespace GX {
void EndLoadTex()
{
    if (data_02099fd0 != -1) {
        func_02059fa8(data_02099fd0);
    }
    _ZN2GX13SetBankForTexEt(data_020a60b8);
    data_020a60c0 = 0;
    data_020a60bc = 0;
    data_020a60ac = 0;
    data_020a60b8  = 0;
}
}
