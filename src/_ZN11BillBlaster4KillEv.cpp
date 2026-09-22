//cpp
// @symbol _ZN11BillBlaster4KillEv
/* BillBlaster::Kill() at ov079 0x02126e58, 0x74 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV11BillBlaster (ov079 0x02127fb8) carries
 * 0x02126e58 at slot 31 -- config/arm9/overlays/ov079/relocs.txt has
 * `from:0x02128034 kind:load to:0x02126e58` -- while _ZTV10dBgActor_c's word at the
 * same slot (from:0x0210aeb4) relocates to 0x020ee55c, _ZN10dBgActor_c4KillEv. Two
 * different addresses in the same slot is what an override IS, so this function
 * is this class's own Kill and not an inherited one.
 *
 * Same shape as dBgActor_c::Kill with this class's own constants: particle 0x8d
 * rather than 0xa, spawned 0x96000 -- 150 units of 20.12 -- above the blaster
 * rather than 100, and bank-3 sound 0xf rather than 0x41. The blaster does
 * destroy itself, so the tail is MarkForDestruction.
 *
 * The second Vector3 is copied MEMBERWISE on purpose: Vector3 declares a
 * destructor (types.h), so a whole-object assignment compiles to an ldm/stm
 * pair, four instructions where the ROM has six. Particle::System::NewSimple
 * stays spelled as its mangled name -- its parameters are Fix12<int> BY VALUE
 * and declaring the true types changes how the caller passes them. Both points
 * are argued in full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "BillBlaster.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void BillBlaster::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x96000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8d, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0xf, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
