//cpp
// @symbol _ZN10dCapIcon_cD0Ev
/* ROM RTTI names this class dCapIcon_c. The real destructor body restores the
 * class vptr and unlinks the icon; the inline operator delete supplies the
 * Memory::operator_delete2 tail of the deleting variant. objisolate retains
 * D0 from the compiler's full destructor group. */
#include "CapIcon.h"

dCapIcon_c::~dCapIcon_c()
{
    Unlink();
}
