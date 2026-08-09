//cpp
// @symbol _ZN3G2x18SetBlendBrightnessEPVtts
/* recovered: shared header, real C++ method (static)
 *
 * Writes the hardware brightness registers behind `p`. The SIGN of `amt`
 * selects the mode rather than a separate flag: negative sets bits 0xc0
 * (fade to black) and writes the magnitude, positive sets 0x80 (fade to
 * white) and writes the value as-is. So one signed argument carries both
 * "which way" and "how much".
 *
 * No `this`: r0 is the register pointer. See include/G2x.h.
 */
#include "G2x.h"

void G2x::SetBlendBrightness(volatile unsigned short *p, unsigned short val, short amt)
{
    if (amt < 0) {
        p[0] = val | 0xc0;
        p[2] = -amt;
    } else {
        p[0] = val | 0x80;
        p[2] = amt;
    }
}
