//cpp
// @symbol _ZN7dBgW_KcC1Ev
/* recovered: real C++ constructor -- base step and vptr store are synthesized
 *
 * Declaring dBgW() in include/dBgW.h is what makes this TU emit
 * `bl _ZN4dBgWC2Ev' then the dBgW_Kc vptr store; only the two body
 * statements are written. The CLPS_BlockRef member constructor clears the
 * pointer at +0x24; kclFile at +0x20 is zeroed after it, in ROM order. The C2 variant
 * this definition also emits is stripped by objisolate; the same source-level
 * constructor is bound to C2 in src/_ZN7dBgW_KcC2Ev.cpp.
 */
#include "dBgW_Kc.h"

dBgW_Kc::dBgW_Kc()
{
    kclFile = 0;
}
