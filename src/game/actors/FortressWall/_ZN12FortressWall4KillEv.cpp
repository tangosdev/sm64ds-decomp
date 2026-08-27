//cpp
// @symbol _ZN12FortressWall4KillEv
/* FortressWall::Kill() at ov079 0x02127280, 0x60 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV12FortressWall (ov079 0x021280b0) carries
 * 0x02127280 at slot 31 -- config/arm9/overlays/ov079/relocs.txt has
 * `from:0x0212812c kind:load to:0x02127280` -- while _ZTV10dBgActor_c's word at the
 * same slot (from:0x0210aeb4) relocates to 0x020ee55c, _ZN10dBgActor_c4KillEv.
 * Different addresses in the same slot: this is the class's own override.
 *
 * The odd one out of this family. It plays the sound FIRST, spawns particle
 * 0x121 at the wall's own position with no vertical offset, and does NOT call
 * PoofDustAt at all. Then it branches on the actor id: id 0x30 is the breakable
 * fortress wall (src/FortressWallBreakable_Spawn.c), which only sets mBroken and
 * stays alive; every other id falls through to MarkForDestruction.
 *
 * THE `int b` INTERMEDIATE IS LOAD-BEARING, the same way it is in
 * src/_ZN16daObjFallBlock_c4KillEv.cpp: `if (actorID == 0x30)` folds the test
 * into one cmp/branch, while the ROM materialises the comparison into a register
 * and then tests THAT. Writing the int is what asks for the second shape.
 *
 * Particle::System::NewSimple stays spelled as its mangled name -- its
 * parameters are Fix12<int> BY VALUE and declaring the true types changes how
 * the caller passes them; argued in full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "FortressWall.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void FortressWall::Kill()
{
    Sound::PlayBank3(0xf, *(Vector3 *)&mCamSpacePosX);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x121, mPosX, mPosY, mPosZ);
    int isBreakable = (actorID == 0x30);
    if (isBreakable) {
        mBroken = 1;
        return;
    }
    MarkForDestruction();
}
