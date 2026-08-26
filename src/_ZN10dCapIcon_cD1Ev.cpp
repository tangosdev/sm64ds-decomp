//cpp
// @symbol _ZN10dCapIcon_cD1Ev
/* ROM RTTI names this class dCapIcon_c. The compiler supplies the vptr store
 * around the one class-specific action: unlinking the icon. objisolate retains
 * D1 from the compiler's full destructor group. */
#include "CapIcon.h"

extern "C" void func_ov001_020ab110(dCapIcon_c *icon);

dCapIcon_c::~dCapIcon_c()
{
    func_ov001_020ab110(this);
}
