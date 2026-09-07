//cpp
// @symbol _ZN16daObjKaitendai_c8BehaviorEv
#include "daObjKaitendai_c.h"
/* daObjKaitendai_c::Behavior -- vtable slot 6, ov002 0x020b6718.
 *
 * Attributed by the vtable, not by a `recovered name:` comment -- the
 * pre-migration file carried none. daObjKaitendai_c's own vtable (ov002
 * 0x021091d4) differs from dBgActor_c's in slots 6, 9, 16 and 17 only, and this
 * address is slot 6, so the function belongs to this class and not to any of
 * its five leaves, which merely inherit it. include/daObjKaitendai_c.h's
 * banner already reads this exact function to conclude the class has no
 * fields: everything it touches is dActor_c's.
 *
 * The turntable step itself: advance the spin by the per-frame increment held
 * in mPrevAngleZ, then publish it as the actor's facing angle. The two
 * dBgActor_c helpers are non-virtual, so an unqualified call is already the
 * direct `bl` the ROM has.
 *
 * THE `s16 *p` IS LOAD-BEARING, not a leftover. `mPrevAngleY += mPrevAngleZ;`
 * recomputes the address for the store and comes out 0x50 against the ROM's
 * 0x54; taking the address once is what makes mwccarm CSE it into a register,
 * which is what the cartridge does. Same lever notes/mwccarm-codegen.md
 * records for compound assignment, arrived at from the other side.
 *
 * dBgActor_c::IsClsnInRangeOnScreen is NOT declared in include/dBgActor_c.h: its
 * ROM name carries by-value Fix12<int> parameters, which mwccarm passes
 * differently from the `int` this call site needs, so it stays an extern "C"
 * of the mangled symbol, the way every other caller in the tree spells it. */

extern "C" int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *c, int a, int b);

s32 daObjKaitendai_c::Behavior()
{
    s16 *p = &mPrevAngleY;
    *p = *p + mPrevAngleZ;
    mAngleY = mPrevAngleY;
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}
