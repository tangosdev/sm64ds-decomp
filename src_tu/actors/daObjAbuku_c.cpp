//cpp
/* Manually curated shadow translation unit for ov002/daObjAbuku_c.
 *
 * NOT ENROLLED and NOT CANONICAL: this readable C++ source is a research
 * artifact under src_tu/.  The seven licensed text functions are retained
 * from the legacy sources, but their class members now use the real
 * daObjAbuku_c : dActor_c declaration and the real dCcAc_c subobject.
 *
 * mwccarm 2004/b56 emits one .text section per function in reverse source
 * order.  Keep the ROM-high function first below.  The compiler-owned
 * destructor at the end intentionally emits D0, D1, D2, vtable, and RTTI;
 * tubuild inventory records those extra sections instead of hiding them.
 *
 * Legacy sources covered by the seven licensed text ranges (ROM order):
 *   [0] 0x020b3298  src/_ZN12daObjAbuku_cD1Ev.c
 *   [1] 0x020b32c8  src/_ZN12daObjAbuku_cD0Ev.c
 *   [2] 0x020b330c  src/unnamed/ov002/020b/func_ov002_020b330c.cpp
 *   [3] 0x020b3344  src/unnamed/ov002/020b/func_ov002_020b3344.c
 *   [4] 0x020b33dc  src/game/actors/daObjAbuku_c/_ZN12daObjAbuku_c8BehaviorEv.cpp
 *   [5] 0x020b3518  src/game/actors/daObjAbuku_c/_ZN12daObjAbuku_c13InitResourcesEv.cpp
 *   [6] 0x020b3568  src/game/actors/daObjAbuku_c/daObjAbuku_c_Spawn.c
 */

/* Real recovered class declarations and typed member/subobject access. */
#include "daObjAbuku_c.h"
#include "Player.h"
#include "Sound.h"

/* The collision probe still needs two TU-local POD views: RG's legacy
 * detection layout and a three-word Vector3 view.  Replacing the latter with
 * the shared Vector3 type changes this TU by adding an unlicensed destructor
 * helper, so it remains an evidenced, isolated shadow. */
/* Vector3 is provided by include/types.h through common.h.  The legacy
 * func_ov002_020b3344.c carried a private three-int shadow, but keeping that
 * duplicate here is illegal once the merged C++ TU includes the real shared
 * declaration (and would discard Vector3's recovered destructor semantics). */

/* shadow struct 'RG' */
struct RG { char a[0x14]; int detect[16]; };

/* This helper is intentionally POD.  The old C member used a local three-word
 * view; using the shared C++ Vector3 here would make mwccarm emit its vague-
 * linkage _ZN7Vector3D1Ev helper into this TU's .text, an extra symbol that
 * has no licensed range in this candidate. */
struct AbukuVector3 { int x, y, z; };

/* Actor factory descriptor at ov002:0x02108940.  The ROM stores one of these
 * behind each actor-table entry: a factory, two scheduling priorities, flags,
 * and three fixed-point culling ranges.  Kept TU-local until the common actor
 * headers recover the shared type without widening this shadow's dependency
 * surface. */
struct AbukuSpawnInfo {
    daObjAbuku_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char AbukuSpawnInfo_size_must_be_0x1c[
    sizeof(AbukuSpawnInfo) == 0x1c ? 1 : -1];

/* shadow typedef 's64' */
typedef long long s64;

extern "C" {
extern int _ZTV12daObjAbuku_c[];
extern void func_ov002_020b330c(daObjAbuku_c *);
extern int func_ov002_020b3344(daObjAbuku_c *);
extern void *data_020a0eac;
extern void *_ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void *);
extern void _ZN7dCcAc_cC1Ev(void *);
extern void _ZN7dCcAc_cD1Ev(void *);
extern void _ZN8dActor_cD2Ev(void *);
extern int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *, dActor_c *, Fix12i, Fix12i, unsigned int, unsigned int);
extern void _ZN9dBgCh_GndC1Ev(struct RG*);
extern void _ZN5dBgCh19StartDetectingWaterEv(struct RG*);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct RG*, const void*, void*);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct RG*);
extern int SurfaceInfo_TestFlag0x20(int* p);
extern void _ZN9dBgCh_GndD1Ev(struct RG*);
extern short data_02082214[];
extern void _Z14ApproachLinearRiii(int *p, int b, int c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int, unsigned int, Fix12i, Fix12i, Fix12i, const Vector3_16 *, void *);
}

extern "C" daObjAbuku_c *daObjAbuku_c_Spawn();

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjAbuku_c_Spawn, 0x020b3568, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daObjAbuku_c_Spawn
/* recovered: vtable identified, declarations from a shared header. Was
 * Bubble_Spawn -- renamed to match the RTTI class name
 * (_ZTS12daObjAbuku_c), see include/daObjAbuku_c.h. */
/* vtable identified: VT0 = _ZTV12daObjAbuku_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
daObjAbuku_c *daObjAbuku_c_Spawn(void)
{
    daObjAbuku_c *p = (daObjAbuku_c *)_ZN7fBase_cnwEj(276);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        /* A compiler-emitted Itanium vtable names its -2/-1 preamble at
         * _ZTV; the object vptr is the first function slot, +8 bytes. */
        *(int *)p = (int)&_ZTV12daObjAbuku_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* 0x02108940..0x0210895c, immediately before the vtable object. */
extern "C" AbukuSpawnInfo Bubble_SpawnInfo = {
    daObjAbuku_c_Spawn,
    0x0123,
    0x00a1,
    0,
    0x00060000,
    0x00200000,
    0x01000000,
    0
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12daObjAbuku_c13InitResourcesEv, 0x020b3518, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c13InitResourcesEv
/* recovered: renamed to Class_Method, RTTI class fields named. Was
 * func_ov002_020b3518, vtable slot 0. */
/* daObjAbuku_c::InitResources - recovered from vtable slot identity */
int daObjAbuku_c::InitResources() {
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
      &mdCcAc_c, this, 0x96000, 0x96000, 0x100002, 0);
  mLifeTimer = 0x12c;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12daObjAbuku_c8BehaviorEv, 0x020b33dc, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daObjAbuku_c8BehaviorEv
/* recovered: renamed to Class_Method, declarations from a shared header.
 * Was func_ov002_020b33dc, vtable slot 6. */
/* daObjAbuku_c::Behavior - recovered from vtable slot identity */
int daObjAbuku_c::Behavior()
{
    mSwayAngle += 0x400;
    int v = *(volatile unsigned short*)(&mSwayAngle);
    int x = v >> 4;
    short tv = data_02082214[2*x + 1];
    mHorzSpeed = (int)(((s64)mDriftSpeed * tv + 0x800) >> 12);
    _Z14ApproachLinearRiii(&mDriftSpeed, 0x6000, 0x332);
    /* The ROM passes a null collision pointer here; the separately owned
     * dCcAc_c is cleared/updated below, not passed to UpdatePos. */
    UpdatePos((dCc_c *)0);
    unsigned int id = mdCcAc_c.otherOwner;
    if (id) {
        dActor_c *found = dActor_c::FindWithID(id);
        if (found) {
            int b = (found->actorID == 0xbf);
            if (b) {
                static_cast<Player *>(found)->Heal(0x300);
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
/* ROM ordinal 3 -- func_ov002_020b3344, 0x020b3344, size 0x98 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* C-linkage helper; its receiver is the recovered class. */
int func_ov002_020b3344(daObjAbuku_c *c){
  struct RG rg;
  AbukuVector3 v;
  _ZN9dBgCh_GndC1Ev(&rg);
  _ZN5dBgCh19StartDetectingWaterEv(&rg);
  int x = c->mPosX;
  int y = c->mPosY;
  int z = c->mPosZ;
  int yk = y + 0x1f4000;
  v.x = x;
  v.y = yk;
  v.z = z;
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, c);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg)) {
    if (SurfaceInfo_TestFlag0x20(rg.detect)) {
      _ZN9dBgCh_GndD1Ev(&rg);
      return 1;
    }
  }
  _ZN9dBgCh_GndD1Ev(&rg);
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020b330c, 0x020b330c, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
void func_ov002_020b330c(daObjAbuku_c *c){
  Sound::PlayBank3(0xb, *reinterpret_cast<const Vector3 *>(
      reinterpret_cast<const char *>(c) + 0x74));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(2, c->mPosX, c->mPosY, c->mPosZ);
  c->MarkForDestruction();
}
}

/* -------------------------------------------------------------------------- */
/* Compiler-owned destructor variants.                                      */
/*                                                                            */
/* mwccarm emits the complete (D1), deleting (D0), and base (D2) variants    */
/* from this one class definition.  D2 and the class vtable/RTTI are kept in  */
/* the object inventory even though the ROM text map currently owns only D0   */
/* and D1; hiding those emissions would make this TU unsafe to promote.       */
/* -------------------------------------------------------------------------- */
daObjAbuku_c::~daObjAbuku_c() {}
