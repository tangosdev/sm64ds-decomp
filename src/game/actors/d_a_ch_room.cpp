//cpp
/* An area-transition trigger box -- ov002/daChRoom_c (VIRTUAL_DOOR).
 *
 * Behavior turns the player's offset into the box's frame. Inside the box
 * and 100..400 units off its plane, it moves the player to the area on that
 * side: mAngle.x on the -z side, mAngle.z on the +z side.
 *
 * Notes:
 * - Vec3_Sub / Vec3_RotateYAndTranslate have no owning header; tree-wide
 *   parameter spellings disagree.
 * - gPlayers is the player table, gLocalPlayerId the current player
 *   index. Typed here as dActor_c *: Behavior only uses mPos and mAreaId.
 * - data_020a0ebc is arm9's zero Vector3 (0xc bytes of .bss after
 *   GAME_HEAP_PTR, constructed (0, 0, 0) in Heap.cpp); here it is the
 *   translation Vec3_RotateYAndTranslate adds.
 * - D1/D0 come from the header's inline dtor: writing them out of line here
 *   ICEs mwccarm (ELFgen.c:483) beside a real D1, and emits D0 before D1
 *   plus a homeless D2.
 */

#include "daChRoom_c.h"
#include "daPly_c.h"
#include "SpawnInfo.h"
#include "math/CalcLib.h"

/* arm9 C helpers with no shared header. */
extern "C" {
void Vec3_RotateYAndTranslate(Vector3 *out, void *m, s16 ang, const Vector3 *in);
void ChangeArea(int);
}

extern u8 gLocalPlayerId;
extern daPly_c *gPlayers[];
extern Vector3 data_020a0ebc;

daChRoom_c *daChRoom_c_classInit() {
    return new daChRoom_c();
}

extern "C" SpawnInfo<daChRoom_c> g_profile_CH_ROOM = {
    daChRoom_c_classInit, 0x015c, 0x015c, 0, 0, 0, 0, 0};

int daChRoom_c::InitResources() {
    mScale.x = (((param1 & 15) + 1) * 0x64000) >> 1;
    mScale.y = (((param1 >> 4 & 15) + 1) * 0x64000);
    mAngle.y = -mAngle.y;
    return 1;
}

int daChRoom_c::Behavior() {
    dActor_c *player;
    Vector3 delta;
    Vector3 local;
    int distX;

    player = gPlayers[gLocalPlayerId];
    Vec3_Sub(&delta, &player->mPos, &mPos);
    Vec3_RotateYAndTranslate(&local, &data_020a0ebc, mAngle.y, &delta);

    distX = local.x;
    if (distX < 0)
        distX = -distX;
    if (distX < mScale.x) {
        if (local.y > -0x96000) {
            if (local.y < mScale.y) {
                int z = local.z;
                int distZ = (z < 0) ? -z : z;
                if (distZ > 0x64000 && distZ < 0x190000) {
                    s8 area = (z < 0) ? mAngle.x : mAngle.z;
                    player->mAreaId = area;
                    ChangeArea(area);
                }
            }
        }
    }
    return 1;
}

int daChRoom_c::Render() {
    return 1;
}

void daChRoom_c::OnPendingDestroy() {}

int daChRoom_c::CleanupResources() {
    return 1;
}
