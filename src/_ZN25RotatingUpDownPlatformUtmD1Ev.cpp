//cpp
// @symbol _ZN25RotatingUpDownPlatformUtmD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct RotatingUpDownPlatformUtm : dBgActor_c`: its own vptr, then the
 * ShadowModel member at 0x320, then dBgActor_c's vptr -- inlined, because
 * dBgActor_c's destructor is defined in its class body -- then dBgActor_c's
 * dBgW_KcMbg and Model, then dActor_c. This class adds no other
 * member with a destructor of its own.
 */
#include "RotatingUpDownPlatformUtm.h"

RotatingUpDownPlatformUtm::~RotatingUpDownPlatformUtm()
{
}
