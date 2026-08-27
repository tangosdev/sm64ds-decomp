//cpp
// @symbol _ZN13SharedFilePtr9ConstructEj
/* SharedFilePtr::Construct(unsigned int) -- forwards to the init helper at
 * 0x02017e48 (still unnamed in symbols.txt) and hands the handle back so the
 * caller can chain. `this` survives the call in r0, which is what makes the
 * returned reference free.
 *
 * THE fileID PARAMETER IS NOT READ HERE. The mangled name fixes it (`Ej`) and
 * the helper reads it out of the register the caller left it in; nothing in
 * this body touches it. */
#include "SharedFilePtr.h"

extern "C" {
void func_02017e48(SharedFilePtr *self);
}

SharedFilePtr &SharedFilePtr::Construct(unsigned int fileID)
{
    func_02017e48(this);
    return *this;
}
