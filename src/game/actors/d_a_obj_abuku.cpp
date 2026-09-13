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
 *   [2] 0x020b330c  func_ov002_020b330c
 *   [3] 0x020b3344  func_ov002_020b3344
 *   [4] 0x020b33dc  daObjAbuku_c::Behavior
 *   [5] 0x020b3518  daObjAbuku_c::InitResources
 *   [6] 0x020b3568  daObjAbuku_c_classInit
 */

#include "daObjAbuku_c.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"

/* POD stand-in: a real Vector3 local emits vague-linkage ~Vector3 into this
 * text-only TU. */
struct AbukuVector3 { int x, y, z; };

struct AbukuSpawnInfo {
    daObjAbuku_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char AbukuSpawnInfo_size_must_be_0x1c[
    sizeof(AbukuSpawnInfo) == 0x1c ? 1 : -1];

typedef long long s64;

extern "C" {
void func_ov002_020b330c(daObjAbuku_c *);
int func_ov002_020b3344(daObjAbuku_c *);
/* Fix12i-by-value Init homes on the stack (6az); keep the scalar ABI. */
int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *, dActor_c *, Fix12i, Fix12i, unsigned int, unsigned int);
int SurfaceInfo_TestFlag0x20(int* p);
extern short data_02082214[];
void _Z14ApproachLinearRiii(int *p, int b, int c);
unsigned short DecIfAbove0_Short(unsigned short *p);
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, const Vector3_16 *, void *);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
void _ZN6Player4HealEi(dActor_c *player, int amount);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjAbuku_c_classInit
extern "C" daObjAbuku_c *daObjAbuku_c_classInit()
{
    return new daObjAbuku_c();
}

extern "C" AbukuSpawnInfo g_profile_OBJ_ABUKU = {
    daObjAbuku_c_classInit,
    0x0123,
    0x00a1,
    0,
    0x00060000,
    0x00200000,
    0x01000000,
    0
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c13InitResourcesEv
int daObjAbuku_c::InitResources() {
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
      &mdCcAc_c, this, 0x96000, 0x96000, 0x100002, 0);
  mLifeTimer = 0x12c;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c8BehaviorEv
int daObjAbuku_c::Behavior()
{
    mSwayAngle += 0x400;
    int v = *(volatile unsigned short*)(&mSwayAngle);
    int x = v >> 4;
    short tv = data_02082214[2*x + 1];
    mHorzSpeed = (int)(((s64)mDriftSpeed * tv + 0x800) >> 12);
    _Z14ApproachLinearRiii(&mDriftSpeed, 0x6000, 0x332);
    UpdatePos((dCc_c *)0);
    unsigned int id = mdCcAc_c.otherOwner;
    if (id) {
        dActor_c *found = dActor_c::FindWithID(id);
        if (found) {
            int b = (found->actorID == 0xbf);
            if (b) {
                _ZN6Player4HealEi(found, 0x300);
                func_ov002_020b330c(this);
            }
        }
    }
    if (DecIfAbove0_Short((unsigned short*)(&mLifeTimer)) == 0 || func_ov002_020b3344(this)) {
        func_ov002_020b330c(this);
    }
    mParticle = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticle, 1, mPosX, mPosY, mPosZ, 0, 0);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {
int func_ov002_020b3344(daObjAbuku_c *c){
  dBgCh_Gnd rg;
  AbukuVector3 v;
  rg.StartDetectingWater();
  int x = c->mPosX;
  int y = c->mPosY;
  int z = c->mPosZ;
  int yk = y + 0x1f4000;
  v.x = x;
  v.y = yk;
  v.z = z;
  rg.SetObjAndPos(*reinterpret_cast<const Vector3 *>(&v), c);
  if (rg.DetectClsn()) {
    if (SurfaceInfo_TestFlag0x20((int*)&rg.surface)) {
      return 1;
    }
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
extern "C" {
void func_ov002_020b330c(daObjAbuku_c *c){
  Sound::PlayBank3(0xb, *reinterpret_cast<const Vector3 *>(&c->mCamSpacePosX));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(2, c->mPosX, c->mPosY, c->mPosZ);
  c->MarkForDestruction();
}
}
