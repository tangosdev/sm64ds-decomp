//cpp
// @symbol _ZN5CrateD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Six destructor calls and two vtable stores, every one a consequence of
 * `struct Crate : dBgActor_c`: its own vptr, destroy
 * dCcAcPos_c x2 (0x5a4, 0x564), ShadowModel (0x508) and
 * dBgCh_Actr (0x320) in reverse declaration order, then dBgActor_c's vptr
 * -- inlined, because dBgActor_c's destructor is defined in its class body --
 * destroy dBgActor_c's own dBgW_KcMbg and Model, then chain to
 * dActor_c.
 */
#include "Crate.h"

Crate::~Crate()
{
}
