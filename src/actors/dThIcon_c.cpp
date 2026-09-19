//cpp
/* dThIcon_c -- the polymorphic touch-screen icon base, ov001.
 *
 * RTTI names dThIcon_c. Both virtuals are defined out of line here;
 * constructor and destructor are inline in include/dThIcon_c.h, so this
 * TU anchors the vague-linkage group and emits no C1/C2/D0/D1/D2.
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
