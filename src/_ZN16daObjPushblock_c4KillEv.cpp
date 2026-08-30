//cpp
// @symbol _ZN16daObjPushblock_c4KillEv
/* daObjPushblock_c::Kill() -- vtable slot 31, ov002 0x020b8c9c. Attributed by the
 * vtable: _ZTV16daObjPushblock_c carries this address at slot 31 -- vtable + 0x7c --
 * where _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv, and slot 30 carries
 * dActor_c's 0x020100dc in both tables. So this is daObjPushblock_c's own override of
 * the one virtual dBgActor_c adds.
 *
 * Same shape as dBgActor_c::Kill (its base's slot 31): spawn a poof particle
 * above the block, poof-dust at that point, play the break sound at
 * mCamSpacePos, then mark for destruction. Particle::System::NewSimple stays
 * spelled as its mangled name because its coordinates are Fix12<int> BY VALUE;
 * declaring the true types changes how the caller passes them and breaks the
 * bytes (notes/mwccarm-codegen.md 6az, and
 * src/_ZN8dActor_c10PoofDustAtERK7Vector3.cpp). */
#include "daObjPushblock_c.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void daObjPushblock_c::Kill()
{
    Vector3 vec;
    Vector3 vec2;
    vec.x = mPosX;
    vec.y = mPosY;
    vec.z = mPosZ;
    vec.y += 0x96000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, vec.x, vec.y, vec.z);
    vec2.x = vec.x;
    vec2.y = vec.y;
    vec2.z = vec.z;
    PoofDustAt(vec2);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
