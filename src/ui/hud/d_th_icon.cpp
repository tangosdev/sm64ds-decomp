//cpp
/* dThIcon_c -- the polymorphic touch-screen icon base, ov001.
 *
 * RTTI names dThIcon_c. Both virtuals are defined out of line here;
 * constructor and destructor are inline in include/dThIcon_c.h, so this
 * TU anchors the vague-linkage group and emits no C1/C2/D0/D1/D2.
 * The unit spans ov001 .text 0x020ab54c..0x020ab5f8: Render, Behavior,
 * then the icon placement helper func_ov001_020ab5b0.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S. mwccarm emits one .text
 * section per function, and with codegen deferred -- the default, which
 * this unit leaves alone -- every body is generated at end of file in
 * the reverse of source order. Behavior before Render puts Render first
 * in the object. No optimisation pragma is needed.
 *
 * deslop
 * Leftover: unk_00c is the countdown this TU subtracts data_0208ee44
 *   from; unk_010 flips 0/1 when it expires; unk_011 latches to 1 when
 *   unk_014 >= 1. Original names are not in the cartridge. Naming fans
 *   out to derived Renders (ov004 TouchIcon, ov006 betIcon, ov075
 *   icon_c).
 * Leftover: unk_004 / unk_006 / unk_008 / unk_00a / unk_01c / unk_020
 *   are unused in this TU.
 * Leftover: data_0208ee44 stays the linker name (arm9 frame delta).
 */

#include "dThIcon_c.h"

extern "C" {

extern int data_0208ee44;

}

// @symbol func_ov001_020ab5b0
/* Place an icon: store its touch kind, centre and half extents, clear the
 * countdown, the blink and the held latch, arm it at once when the kind is
 * touchable (the latch Behavior applies), and start the countdown reload
 * at ten. The highest address in this unit, so first in the file. Callers
 * in ov004/ov006/ov075 pass the icon as a char *, so the entry keeps it. */
extern "C" void func_ov001_020ab5b0(char *p, int kind, short x, short y,
                                    short halfW, short halfH)
{
    dThIcon_c *icon = reinterpret_cast<dThIcon_c *>(p);
    icon->unk_014 = kind;
    icon->unk_004 = x;
    icon->unk_006 = y;
    icon->unk_008 = halfW;
    icon->unk_00a = halfH;
    icon->unk_00c = 0;
    icon->unk_010 = 0;
    icon->unk_012 = 0;
    if (icon->unk_014 >= 1)
        icon->unk_011 = 1;
    icon->unk_018 = 10;
}

// @symbol _ZN9dThIcon_c8BehaviorEv
void dThIcon_c::Behavior()
{
    if (unk_00c <= 0)
        return;

    unk_00c -= data_0208ee44;
    if (unk_00c > 0)
        return;

    if (unk_010 != 0)
        unk_010 = 0;
    else
        unk_010 = 1;

    if (unk_014 >= 1)
        unk_011 = 1;
}

// @symbol _ZN9dThIcon_c6RenderEv
void dThIcon_c::Render()
{
}
