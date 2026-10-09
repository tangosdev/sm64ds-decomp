//cpp
// @symbol _ZN13SharedFilePtr9ConstructEj
/* SharedFilePtr::Construct(unsigned int) -- forwards to the init helper at
 * 0x02017e48 (still unnamed in symbols.txt) and hands the handle back so the
 * caller can chain. `this` survives the call in r0, which is what makes the
 * returned reference free.
 *
 * recovered: the fileID RIDES THROUGH. The body is push {r4, lr} / mov r4, r0 /
 * bl func_02017e48 / mov r0, r4: nothing here touches r1, so the fileID the
 * caller left there is what the helper reads. Naming no second argument was
 * byte-exact and wrong -- the value never arrives on a host ABI -- so the
 * parameter is spelled and forwarded, which is byte-identical. */
#include "SharedFilePtr.h"

extern "C" {
void func_02017e48(SharedFilePtr *self, unsigned int fileID);
}

SharedFilePtr &SharedFilePtr::Construct(unsigned int fileID)
{
    func_02017e48(this, fileID);
    return *this;
}
