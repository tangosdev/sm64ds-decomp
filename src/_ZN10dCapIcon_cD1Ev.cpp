//cpp
// @symbol _ZN10dCapIcon_cD1Ev
/* ROM RTTI names this class dCapIcon_c. The compiler supplies the vptr store
 * around the one class-specific action: unlinking the icon. objisolate retains
 * D1 from the compiler's full destructor group. */
#include "CapIcon.h"

dCapIcon_c::~dCapIcon_c()
{
    Unlink();
}
