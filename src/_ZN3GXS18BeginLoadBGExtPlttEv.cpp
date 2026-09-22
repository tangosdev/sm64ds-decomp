//cpp
// @symbol _ZN3GXS18BeginLoadBGExtPlttEv
/* GXS::BeginLoadBGExtPltt() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GXS is a namespace, not a class: no `this`,
 * no vtable, no layout, so nothing outside this file can shift. The body and
 * every callee declaration are unchanged; the symbol is now mangled by the
 * compiler instead of spelled by hand.
 */
extern "C" void *func_02054118(void);
extern "C" void *data_020a60a4;

namespace GXS {
void BeginLoadBGExtPltt()
{
    data_020a60a4 = func_02054118();
}
}
