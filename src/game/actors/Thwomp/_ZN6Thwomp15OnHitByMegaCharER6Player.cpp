//cpp
// @symbol _ZN6Thwomp15OnHitByMegaCharER6Player
#include "Thwomp.h"
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov091).
/* daDsn_c::OnHitByMegaChar - recovered from vtable slot identity. The
   draft cast `this` to a 30-entry vtable-shim struct and called its
   final slot (index 29); that is OnAimedAtWithEgg(), already named
   above, so this calls it through the real virtual dispatch instead.
   mPosX/mPosY/mPosZ are dActor_c's own named fields (dActor_c.h),
   inherited. */
extern "C" {
void _ZN6Player16IncMegaKillCountEv(void* self);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_02012694(int, void*);
}

void Thwomp::OnHitByMegaChar(Player &player)
{
    _ZN6Player16IncMegaKillCountEv(&player);
    Vector3 vec;
    vec.x = mPosX;
    vec.y = mPosY;
    vec.z = mPosZ;
    int ret = OnAimedAtWithEgg();
    vec.y += ret;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, vec.x, vec.y, vec.z);
    Vector3 vec2;
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    PoofDustAt(vec2);
    MarkForDestruction();
    func_02012694(0x1e, (char*)this + 0x74);
}
