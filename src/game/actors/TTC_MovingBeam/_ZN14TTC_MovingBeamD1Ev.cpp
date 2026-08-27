//cpp
// @symbol _ZN14TTC_MovingBeamD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct TTC_MovingBeam : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own.
 */
#include "TTC_MovingBeam.h"

TTC_MovingBeam::~TTC_MovingBeam()
{
}
