//cpp
/* Production translation unit for ov002/daObjAbuku_c (bubbles).
 *
 * RTTI ov002:0x02108924 names daObjAbuku_c; overlay_actors.md maps profile
 * BUBBLE (291). Not daObjLava_c / LAVA_BUBBLE (#2513).
 *
 * mwccarm 2004/b56 emits one .text section per function in reverse source
 * order. Keep the ROM-high factory first. The inline destructor in
 * daObjAbuku_c.h emits retail D1 then D0 and no D2.
 *
 *   [0] 0x020b3298  _ZN12daObjAbuku_cD1Ev
 *   [1] 0x020b32c8  _ZN12daObjAbuku_cD0Ev
 *   [2] 0x020b330c  daObjAbuku_c::Pop
 *   [3] 0x020b3344  daObjAbuku_c::IsNearWaterSurface
 *   [4] 0x020b33dc  daObjAbuku_c::Behavior
 *   [5] 0x020b3518  daObjAbuku_c::InitResources
 *   [6] 0x020b3568  daObjAbuku_c_classInit
 *
 * Notes:
 * - data_02082214 is the link name (arm9 symbols.txt kind:data(any));
 *   no named alias exists. Sway lookup off (mSwayAngle >> 4).
 * - g_profile_OBJ_ABUKU is a SpawnInfo, the shared 0x1c actor-profile
 *   layout. The manifest proves the descriptor,
 *   then deadstrip-discards it (canonical home outside .text).
 * - mSwayAngle volatile-u16 reload + >> 4, and the actorID == 0xbf
 *   int-then-if, kept in matched form.
 */

#include "daObjAbuku_c.h"
#include "SpawnInfo.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "Particle__System.h"

/* arm9 C helpers with no shared header. */
extern "C" {
int SurfaceInfo_TestFlag0x20(int *p);
extern short data_02082214[];
unsigned short DecIfAbove0_Short(unsigned short *p);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjAbuku_c_classInit
extern "C" daObjAbuku_c *daObjAbuku_c_classInit() {
    return new daObjAbuku_c();
}

extern "C" SpawnInfo<daObjAbuku_c> g_profile_OBJ_ABUKU = {
    daObjAbuku_c_classInit, 0x0123, 0x00a1, 0, 0x00060000, 0x00200000, 0x01000000, 0};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c13InitResourcesEv
int daObjAbuku_c::InitResources() {
    mdCcAc_c.Init(this, 0x96000, 0x96000, 0x100002, 0);
    mLifeTimer = 0x12c;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c8BehaviorEv
int daObjAbuku_c::Behavior() {
    mSwayAngle += 0x400;
    int v = *(volatile unsigned short *)(&mSwayAngle);
    int x = v >> 4;
    short tv = data_02082214[2 * x + 1];
    mHorzSpeed = FX_MUL(mDriftSpeed, tv);
    ApproachLinear(mDriftSpeed, 0x6000, 0x332);
    UpdatePos((dCc_c *)0);
    unsigned int id = mdCcAc_c.otherOwner;
    if (id) {
        dActor_c *found = dActor_c::FindWithID(id);
        if (found) {
            int b = (found->actorID == 0xbf);
            if (b) {
                ((Player *)found)->Heal(0x300);
                Pop();
            }
        }
    }
    if (DecIfAbove0_Short((unsigned short *)(&mLifeTimer)) == 0 || IsNearWaterSurface()) {
        Pop();
    }
    mParticle = (int)Particle::System::New(mParticle, 1, mPosX, mPosY, mPosZ, 0, 0);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c18IsNearWaterSurfaceEv
/* Water probe cast down from 500 units above the bubble: it hits only once
   the surface is within that reach, and the bubble pops there. */
int daObjAbuku_c::IsNearWaterSurface() {
    dBgCh_Gnd rg;
    Vector3 probe;
    rg.StartDetectingWater();
    int x = mPosX;
    int y = mPosY;
    int z = mPosZ;
    int yk = y + 0x1f4000;
    probe.x = x;
    probe.y = yk;
    probe.z = z;
    rg.SetObjAndPos(probe, this);
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20((int *)&rg.surface)) {
            return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c3PopEv
void daObjAbuku_c::Pop() {
    Sound::PlayBank3(0xb, mCamSpacePos);
    Particle::System::NewSimple(2, mPosX, mPosY, mPosZ);
    MarkForDestruction();
}
