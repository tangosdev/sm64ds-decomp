//cpp
/* An area-transition trigger box -- ov002/daChRoom_c.
 *
 * ROM RTTI names daChRoom_c; overlay_actors.md VIRTUAL_DOOR(348). Ugly RTTI
 * name is final. mwccarm emits ordinary function sections in reverse source
 * order; keep the factory first. The inline destructor in daChRoom_c.h emits
 * retail D1 then D0 and no D2.
 *
 * daChRoom_c_classInit is reconstructed (RTTI daChRoom_c, CH_ROOM registry).
 * Retail does not store that spelling. Historical alias daChRoom_c_Spawn.
 *
 * deslop
 * Leftover: Vec3_Sub / Vec3_RotateYAndTranslate have no owning header;
 *   tree-wide parameter spellings disagree.
 * Leftover: data_0209f394 is the player table, data_0209f250 the current
 *   player index. Typed here as dActor_c* -- Behavior only uses
 *   dActor_c::mPosX and dActor_c::mAreaId. Player.h is out of scope.
 * Leftover: data_020a0ebc is an arm9 scratch transform; the tree disagrees
 *   on its type (char / int / Vector3 / Triple).
 */

#include "daChRoom_c.h"

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

// @symbol _ZN10daChRoom_c13InitResourcesEv
int daChRoom_c::InitResources()
{
  mScaleX=(((param1&0xf)+1)*0x64000)>>1;
  mScaleY=(((param1>>4&0xf)+1)*0x64000);
  mAngleY=-mAngleY;
  return 1;
}

// @symbol _ZN10daChRoom_c8BehaviorEv
int daChRoom_c::Behavior()
{
    dActor_c* obj;
    Vector3 d;
    Vector3 r;
    int v;

    obj = data_0209f394[data_0209f250];
    Vec3_Sub(&d, (Vector3*)&obj->mPosX, (Vector3*)&mPosX);
    Vec3_RotateYAndTranslate(&r, &data_020a0ebc, mAngleY, &d);

    v = r.x;
    if (v < 0) v = -v;
    if (v < mScaleX) {
        if (r.y > -0x96000) {
            if (r.y < mScaleY) {
                int z = r.z;
                int az = (z < 0) ? -z : z;
                if (az > 0x64000 && az < 0x190000) {
                    int area = (z < 0) ? mAngleX : mAngleZ;
                    obj->mAreaId = (char)area;
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
