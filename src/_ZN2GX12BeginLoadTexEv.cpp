//cpp
// @symbol _ZN2GX12BeginLoadTexEv
/* GX::BeginLoadTex() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"

extern u16 data_02086324[];
extern u16 data_02086326[];
extern u16 data_02086328[];
extern int data_020a60b8;
extern int data_020a60ac;
extern int data_020a60bc;
extern int data_020a60c0;

extern "C" {
extern int func_0205417c(void);
}

namespace GX {
void BeginLoadTex(){
    int r = func_0205417c();
    int i = r * 6;
    data_020a60b8 = r;
    data_020a60ac = (int)*(u16 *)((char *)data_02086324 + i) << 12;
    data_020a60bc = (int)*(u16 *)((char *)data_02086326 + i) << 12;
    data_020a60c0 = (int)*(u16 *)((char *)data_02086328 + i) << 12;
}
}
