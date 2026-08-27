//cpp
// @symbol _ZN16daObjFallBlock_c4KillEv
/* daObjFallBlock_c::Kill() at ov098 0x0213a17c, 0xc0 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV16daObjFallBlock_c (ov098 0x0213c5bc) carries
 * 0x0213a17c at slot 31 and _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv at the
 * same slot, so this is this class's own override of dBgActor_c's Kill. The
 * header's own note lists the same six own-slots independently: 6, 9, 16, 17,
 * 27 and 31.
 *
 * The block plays its poof where it stands, 0x32000 -- fifty 20.12 units --
 * above itself, then does NOT destroy itself: it teleports back to mRestPos,
 * clears mReady and sets mStateTimer to 0x1e. That is the respawn, which is
 * why there is no MarkForDestruction here and why the other thirteen slot-31
 * overrides in this family end with one.
 *
 * dActor_c id 0x8b gets particle 0x49 instead of 0x48.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays spelled
 * as its mangled name -- its parameters are Fix12<int> BY VALUE and declaring
 * the true types changes how the caller passes them. Both points are argued in
 * full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "daObjFallBlock_c.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void daObjFallBlock_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x32000;
    u32 id = 0x48;
    /* THE INTERMEDIATE IS LOAD-BEARING. `if (actorID == 0x8b)` folds the test
       into the conditional move -- `cmp r1,#0x8b; moveq r0,#0x49`, two
       instructions. The ROM materialises the comparison into a register first
       and then tests THAT: cmp/moveq #1/movne #0/cmp #0/movne, five. Writing
       the int is what asks for the second shape. */
    int isBigBlock = (actorID == 0x8b);
    if (isBigBlock) {
        id = 0x49;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(id, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    mStateTimer = 0x1e;
    mPosX = mRestPos.x;
    mPosY = mRestPos.y;
    mPosZ = mRestPos.z;
    mReady = 0;
}
