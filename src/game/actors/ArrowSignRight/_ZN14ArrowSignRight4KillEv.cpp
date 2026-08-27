//cpp
// @symbol _ZN14ArrowSignRight4KillEv
/* ArrowSignRight::Kill() at ov098 0x02137ccc, 0x74 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV14ArrowSignRight (ov098 0x0213c3d8) relocates
 * its word at +0x7c to 0x02137ccc, and _ZTV10dBgActor_c carries
 * _ZN10dBgActor_c4KillEv at the same slot, so this is this class's own override of
 * dBgActor_c's Kill. Slot 30 (+0x78) is the main-module 0x02010124 in both
 * tables, which is what makes 31 the first slot where they differ.
 *
 * The file used to carry `// recovered name: ArrowSignRight_Kill` alongside
 * `daObjYajirusi_c::Kill`. daObjYajirusi_c IS this class's RTTI name, so the
 * two agreed here; the comment is gone anyway because the vtable, not a
 * comment, is the source.
 *
 * Same shape as dBgActor_c::Kill, with three differences the ROM dictates: the
 * particle is 0xe rather than 0xa, it spawns 0x28000 -- forty 20.12 units --
 * above the sign rather than a hundred, and the poof is DisappearPoofDustAt
 * (particles 0x127/0x128) rather than PoofDustAt.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays spelled
 * as its mangled name -- its parameters are Fix12<int> BY VALUE and declaring
 * the true types changes how the caller passes them. Both points are argued in
 * full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "ArrowSignRight.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void ArrowSignRight::Kill()
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
    MarkForDestruction();
}
