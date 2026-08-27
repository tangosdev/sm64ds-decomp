//cpp
// @symbol _ZN6ThwompD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * THREE vtable stores and FIVE destructor calls, and none of them is written
 * here. `struct Thwomp : daDsnBase_c : dBgActor_c` emits its own vptr, then
 * daDsnBase_c's -- inlined, because that destructor is defined in its class body
 * -- then that class's ShadowModel at 0x338 and TextureSequence at 0x324 in
 * reverse declaration order, then dBgActor_c's vptr and dBgActor_c's own
 * dBgW_KcMbg and Model, then dActor_c.
 *
 * This is the only one of the eleven intermediates whose leaf destructor is 0x60
 * rather than 0x50, and the two extra calls are exactly the two sub-objects
 * daDsnBase_c declares.
 */
#include "Thwomp.h"

Thwomp::~Thwomp()
{
}
