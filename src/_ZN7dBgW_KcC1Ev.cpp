//cpp
// @symbol _ZN7dBgW_KcC1Ev
/* recovered: real C++ constructor -- base step and vptr store are synthesized
 *
 * Declaring dBgW() in include/dBgW.h is what makes this TU emit
 * `bl _ZN4dBgWC2Ev' then the dBgW_Kc vptr store; only the two body
 * statements are written. func_02038228 clears the CLPS block pointer at
 * +0x24; kclFile at +0x20 is zeroed after it, in ROM order. The C2 variant
 * this definition also emits is stripped by objisolate -- its ROM body is
 * enrolled separately as src/_ZN7dBgW_KcC2Ev.c (section 1).
 */
#include "dBgW_Kc.h"

extern "C" void func_02038228(void *clps);

dBgW_Kc::dBgW_Kc()
{
    func_02038228(&clps);
    kclFile = 0;
}
