//cpp
// @symbol _ZN15RecRoomCupboard13InitResourcesEv
/* recovered: real class form -- the compiler spells the mangled name.
 *
 * dCcAcPos_c::Init is CALLED, not defined, through its mangled name with int
 * parameters: its ROM name carries by-value Fix12<int> arguments, which
 * mwccarm passes differently from a plain int at the call site, so spelling
 * the true types breaks the byte match. Every other InitResources in the tree
 * that touches this function does the same (notes/mwccarm-codegen.md 6az).
 */
#include "RecRoomCupboard.h"

extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, void *offset, int radius, int height,
    unsigned int flags, unsigned int vulnFlags);

s32 RecRoomCupboard::InitResources()
{
    int i;
    dCcAcPos_c *collider = mColliders;
    for (i = 0; i < 5; i++) {
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            collider, this, &mPosX, 0x6e000, 0x8c000, 0x4800002, 0);
        collider++;
    }
    return 1;
}
