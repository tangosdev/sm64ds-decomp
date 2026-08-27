//cpp
// @symbol _ZN3OAM12EnableSubOAMEv
#include "OAM.h"

/* OAM::EnableSubOAM() at 0x020219f0 -- static, no `this`.
 *
 * Clears the sub-screen OAM disable byte at 0x0209e660. Despite the name it only
 * writes zero -- the flag is "sub OAM suppressed", so enabling is clearing it.
 *
 * The declared u32 return is a fiction the ROM does not honour: the function falls
 * off its end without setting r0, which is why the body has no `return`. Declaring
 * it void would spell `Ev` the same way and mangle identically, but the existing
 * declaration is kept so the change stays a language-mode migration and nothing
 * else. The `(long) 0` cast is likewise preserved verbatim.
 */
extern "C" char data_0209e660[];

u32 OAM::EnableSubOAM()
{
    data_0209e660[0] = (long)0;
}
