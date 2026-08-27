//cpp
// @symbol _ZN8IceBlock4KillEv
/* IceBlock::Kill() at ov081 0x02127cf4, 0xa4 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV8IceBlock (ov081 0x02128cc4) relocates its word
 * at +0x7c to 0x02127cf4, and _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv at the
 * same slot, so this is this class's own override of dBgActor_c's Kill. Slot 30
 * (+0x78) is the main-module 0x02010124 in both tables, which is what makes 31
 * the first slot where they differ.
 *
 * The file used to carry `// recovered name: IceBlock_Kill` alongside
 * `daObjIceBlock_c::Kill`. daObjIceBlock_c IS this class's RTTI name, so the
 * two agreed here; the comment is gone anyway because the vtable, not a
 * comment, is the source.
 *
 * Unlike the rest of the family this one plays its sound FIRST, before reading
 * the position, and it spawns three shatter particles -- 0x74, 0x75, 0x76 --
 * at one point 0x96000 (one hundred and fifty 20.12 units) above itself rather
 * than a single one. The call to func_ov081_02127be0 between the poof and the
 * destruction is the block handing its cargo off: it respawns the actor parked
 * in unk_364 as a free-standing one. _ZN8IceBlock8BehaviorEv calls the same
 * helper, which is still an unnamed placeholder in ov081 and is left that way
 * here -- naming it is a separate change.
 *
 * The second Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six. Particle::System::NewSimple stays spelled
 * as its mangled name -- its parameters are Fix12<int> BY VALUE and declaring
 * the true types changes how the caller passes them. Both points are argued in
 * full in src/_ZN10dBgActor_c4KillEv.cpp. */
#include "IceBlock.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);
extern "C" void func_ov081_02127be0(void *self);

void IceBlock::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x96000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x74, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x75, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x76, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    func_ov081_02127be0((char *)this);
    MarkForDestruction();
}
