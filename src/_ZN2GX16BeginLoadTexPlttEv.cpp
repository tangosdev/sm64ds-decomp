//cpp
// @symbol _ZN2GX16BeginLoadTexPlttEv
/* GX::BeginLoadTexPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"

extern int data_020a60b4;
extern u16 data_02086314[];
extern int data_020a60b0;

extern "C" {
extern int func_02054168(void);
}

namespace GX {
void BeginLoadTexPltt(){
    int r = func_02054168();
    data_020a60b4 = r;
    data_020a60b0 = (int)data_02086314[r >> 4] << 12;
}
}
