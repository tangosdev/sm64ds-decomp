//cpp
// @symbol _ZN3GXS19BeginLoadOBJExtPlttEv
/* GXS::BeginLoadOBJExtPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no `this`,
 * no vtable, no layout, so nothing outside this file can shift. The body and
 * every callee declaration are unchanged; the symbol is now mangled by the
 * compiler instead of spelled by hand.
 */
extern "C" void *func_020540f0(void);
extern "C" void *data_020a60a8;

namespace GXS {
void BeginLoadOBJExtPltt()
{
    data_020a60a8 = func_020540f0();
}
}
