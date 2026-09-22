//cpp
// @symbol _ZN3HUD17RenderSilverStarsEv
/* recovered: shared header, real C++ method
 *
 * Draws one sprite per silver star collected, in a row starting at x=0x10 and
 * stepping 0x11 across at y=0xa. The count comes from data_0209f310 indexed by
 * data_0209f250[0] -- the same byte HUD::RenderRedCoins reads, where a non-zero
 * value instead pushes the red-coin row from y=0xa down to y=0x1c. So this row
 * and that one share a slot, and the red coins move when silver stars occupy it.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this` at all; as a method it now
 * receives one in r0 and ignores it, which is byte-free here. Nothing about the
 * ROM says this was a static -- unlike OAM's methods, whose call sites pass no
 * `this` -- so it is left a normal member.
 */
#include "HUD.h"

extern "C" {
extern unsigned char data_0209f250[];
extern signed char data_0209f310[];
extern int data_ov001_020abac8[];
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void *, int, int, int, int, void *);
}

void HUD::RenderSilverStars()
{
    int sl = 0;
    unsigned short n = (unsigned short)(short)data_0209f310[data_0209f250[0]];
    int sb;
    if (sl < n) {
        sb = 0x10;
        do {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (void *)data_ov001_020abac8, sb, 0xa, -1, 1, (void *)0);
            sl++;
            sb += 0x11;
        } while (sl < n);
    }
}
