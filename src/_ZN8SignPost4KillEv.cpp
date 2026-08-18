//cpp
// @symbol _ZN8SignPost4KillEv
/* SignPost::Kill() at ov002 0x020bb3b8, 0x74 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV8SignPost (ov002 0x02109af8, and the same
 * address as _ZTV15daObjTatefuda_c) carries 0x020bb3b8 at vtable + 0x7c, which
 * is slot 31, while _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv at the same slot
 * and both tables carry dActor_c's 0x020100dc at slot 30. So this is this class's
 * own override of the one virtual dBgActor_c adds. Read out of
 * config/arm9/overlays/ov002/relocs.txt.
 *
 * The signpost does not destroy itself. It plays its particle 0x28000 -- forty
 * 20.12 units -- above where it stands, poofs, plays the break sound and then
 * tails into func_ov002_020bae9c, this class's own still-unnamed reset routine.
 * That is why there is no MarkForDestruction here, unlike dBgActor_c::Kill.
 *
 * The trailing call's return value is dropped: the ROM does `bl`, then the
 * epilogue and `bx lr` with nothing written to r0 in between, which is what a
 * void method calling an int function compiles to.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays spelled
 * as its mangled name -- its parameters are Fix12<int> BY VALUE and declaring
 * the true types changes how the caller passes them. Both points are argued in
 * full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "SignPost.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

/* This class's own reset routine, still unnamed and still under its func_ov002_
   symbol. It returns int; Kill drops it. */
extern "C" int func_ov002_020bae9c(void *self);

void SignPost::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x28000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xe, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    DisappearPoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    func_ov002_020bae9c(this);
}
