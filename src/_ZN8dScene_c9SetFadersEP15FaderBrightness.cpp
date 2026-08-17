//cpp
/* dScene_c::SetFaders(FaderBrightness*) at 0x0202e1e0, 0x8c bytes.
 *
 * Installs a fader, carrying the outgoing one's end state across so the screen
 * does not jump: if the fader being replaced was sitting at the start, snap the
 * new one to the start; if it was at the end, snap it to the end; if it was
 * mid-fade, leave the new one alone.
 *
 * WHAT THIS FILE USED TO SAY. It declared a local `struct dScene_c` with ten
 * invented virtuals named v00..v24, took its argument as `dScene_c *thiz`, and
 * called `thiz->v24()` and `thiz->v20()`. Every part of that was wrong in a way
 * the bytes could not see:
 *
 *   - the argument is a FaderBrightness, which the mangled name has said all
 *     along, and dScene_c::SetAndStopColorFader confirms by loading a FaderColor
 *     into r0 and branching straight here;
 *   - so this is a STATIC member -- r0 is the parameter, not a `this`;
 *   - and v20/v24 are vtable slots 8 and 9, which are SetToEnd and SetToStart.
 *
 * Slots 8 and 9 are why this file could not be migrated with the rest of the
 * class: include/Fader.h declared seven slots and the ROM has ten, so there was
 * no way to spell these calls through the real type. Fixed in #1259, which is
 * what this file is the proof of.
 *
 * The other object, at 0x0209f5bc, is the currently-installed fader and is the
 * same type -- SetFaders is the only thing that writes it, and writes exactly
 * this argument. Slots 5 and 6 on it are IsAtStart and IsAtEnd.
 */
#include "dScene_c.h"
#include "FaderBrightness.h"

extern "C" {
/* Two globals holding the same pointer. 0x0209f5bc is the one this function
   reads back; 0x0209d4ac is read by 0x02018efc, which calls AdvanceFade (slot 2)
   on it -- which is the evidence that it holds a fader and not, as this file
   used to have it, a dScene_c. */
extern FaderBrightness *data_0209f5bc;
extern FaderBrightness *data_0209d4ac;
}

void dScene_c::SetFaders(FaderBrightness *fader)
{
    if (data_0209f5bc) {
        if (data_0209f5bc->IsAtStart()) {
            fader->SetToStart();
        } else if (data_0209f5bc->IsAtEnd()) {
            fader->SetToEnd();
        }
    }
    data_0209f5bc = fader;
    data_0209d4ac = fader;
}
