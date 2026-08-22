//cpp
// @symbol _ZN10dBgActor_c14KillByMegaCharER6Player
/* dBgActor_c::KillByMegaChar -- shake the ground at this actor's position, then
 * launch it away from the player and take its mesh collider out of the world.
 *
 * Earthquake carries Fix12<int> in its mangled name and is not declared in
 * include/dActor_c.h, so the call stays an extern-C mangled free function.
 * Vec3_HorzAngle has no header either.
 *
 * The player's position is reached by offset, not by member: the parameter is a
 * `Player &' that include/dBgActor_c.h only forward-declares, and pulling in
 * Player.h here to name three words would put a 0x5c0-byte class definition into
 * a translation unit that has no other use for it.
 */
#include "dBgActor_c.h"

extern "C" {
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(dBgActor_c *self, Vector3 *v, int mag);
s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
}

void dBgActor_c::KillByMegaChar(Player &player_)
{
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &v, 0x5dc000);
    unk_31c = 1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x14000;
    mVertSpeed = 0x1e000;
    mPrevAngleY = Vec3_HorzAngle((Vector3 *)((char *)&player_ + 0x5c), (Vector3 *)&mPosX);
    unk_31d = 0x1e;
    if (mMeshCollider.IsEnabled() == 0)
        return;
    mMeshCollider.Disable();
}
