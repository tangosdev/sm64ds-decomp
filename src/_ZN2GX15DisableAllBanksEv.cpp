//cpp
// @symbol _ZN2GX15DisableAllBanksEv
/* GX::DisableAllBanks() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */


extern "C" {
extern void func_02053f58(void);
extern void func_02054018(void);
extern void func_02054004(void);
extern void func_02053f6c(void);
extern void func_02053fa8(void);
extern void func_02053f94(void);
extern void func_02053f80(void);
extern void func_02053fe0(void);
extern void func_02053fbc(void);
extern void func_02053f44(void);
extern void func_02053f30(void);
extern void func_02053f08(void);
extern void func_02053ee0(void);
}

namespace GX {
void DisableAllBanks(){
 func_02053f58(); func_02054018(); func_02054004(); func_02053f6c();
 func_02053fa8(); func_02053f94(); func_02053f80(); func_02053fe0();
 func_02053fbc(); func_02053f44(); func_02053f30(); func_02053f08();
 func_02053ee0();
}
}
