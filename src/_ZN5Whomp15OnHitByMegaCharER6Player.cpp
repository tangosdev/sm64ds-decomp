//cpp
// @symbol _ZN5Whomp15OnHitByMegaCharER6Player
// recovered name: Whomp::OnHitByMegaChar
/* Whomp::OnHitByMegaChar - vtable slot 27, overriding
 * dActor_c::OnHitByMegaChar(Player&). Migrated to a real member: the raw
 * shadow-struct offsets resolve onto fields already recovered in
 * include/Whomp.h (mIsKing 0x414, inherited mDeathState 0x10c, inherited
 * mCamSpacePosX 0x74, mMovingMeshCollider 0x418) and the callees onto real
 * (non-virtual) members of Player/dActor_c/fBase_c. */
#include "Whomp.h"
#include "Player.h"

extern "C" {
extern void func_02012694(int a, void* v);
extern void func_ov079_02123d4c(void* out, void* c);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
}

void Whomp::OnHitByMegaChar(Player &player)
{
    if (mIsKing != 0) return;
    if (mDeathState == 8) return;
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    Vector3 pos;
    func_ov079_02123d4c(&pos, this);
    Vector3 dust;
    pos.y = pos.y + 0x28000;
    dust.x = pos.x;
    dust.y = pos.y;
    dust.z = pos.z;
    PoofDustAt(dust);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(5, pos.x, pos.y, pos.z);
    MarkForDestruction();
    if (mMovingMeshCollider.IsEnabled() != 0)
        mMovingMeshCollider.Disable();
}
