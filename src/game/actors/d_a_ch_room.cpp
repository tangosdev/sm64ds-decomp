//cpp
/* An area-transition trigger box -- ov002/daChRoom_c (VIRTUAL_DOOR).
 *
 * deslop
 * Leftover: Vec3_Sub / Vec3_RotateYAndTranslate have no owning header;
 *   tree-wide parameter spellings disagree.
 * Leftover: data_0209f394 is the player table, data_0209f250 the current
 *   player index. Typed here as dActor_c* -- Behavior only uses
 *   dActor_c::mPosX and dActor_c::mAreaId. Player.h is out of scope.
 * Leftover: data_020a0ebc is an arm9 scratch transform; the tree disagrees
 *   on its type (char / int / Vector3 / Triple).
 * Leftover: D1/D0 come from the header's inline dtor -- writing them out
 *   of line here ICEs mwccarm (ELFgen.c:483) beside a real D1, and emits
 *   D0 before D1 plus a homeless D2.
 */

#include "daChRoom_c.h"
#include "SpawnInfo.h"

extern "C" {
extern void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
extern void Vec3_RotateYAndTranslate(Vector3* out, void* m, s16 ang, Vector3* in);
extern void ChangeArea(int);
extern u8 data_0209f250;
extern dActor_c* data_0209f394[];
extern char data_020a0ebc;
}

// @symbol daChRoom_c_classInit
extern "C" daChRoom_c *daChRoom_c_classInit()
{
    return new daChRoom_c();
}

extern "C" SpawnInfo<daChRoom_c> g_profile_CH_ROOM = {
    daChRoom_c_classInit,
    0x015c,
    0x015c,
    0,
    0,
    0,
    0,
    0
};

// @symbol _ZN10daChRoom_c13InitResourcesEv
int daChRoom_c::InitResources()
{
    mScaleX = (((param1 & 0xf) + 1) * 0x64000) >> 1;
    mScaleY = (((param1 >> 4 & 0xf) + 1) * 0x64000);
    mAngleY = -mAngleY;
    return 1;
}

// @symbol _ZN10daChRoom_c8BehaviorEv
int daChRoom_c::Behavior()
{
    dActor_c* player;
    Vector3 delta;
    Vector3 local;
    int distX;

    player = data_0209f394[data_0209f250];
    Vec3_Sub(&delta, &player->mPos, &mPos);
    Vec3_RotateYAndTranslate(&local, &data_020a0ebc, mAngleY, &delta);

    distX = local.x;
    if (distX < 0) distX = -distX;
    if (distX < mScaleX) {
        if (local.y > -0x96000) {
            if (local.y < mScaleY) {
                int z = local.z;
                int distZ = (z < 0) ? -z : z;
                if (distZ > 0x64000 && distZ < 0x190000) {
                    int area = (z < 0) ? mAngleX : mAngleZ;
                    player->mAreaId = (char)area;
                    ChangeArea((char)area);
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN10daChRoom_c6RenderEv
int daChRoom_c::Render()
{
    return 1;
}

// @symbol _ZN10daChRoom_c16OnPendingDestroyEv
void daChRoom_c::OnPendingDestroy()
{
}

// @symbol _ZN10daChRoom_c16CleanupResourcesEv
int daChRoom_c::CleanupResources()
{
    return 1;
}
