//cpp
/* HAND-ASSEMBLED translation unit -- ov070/daKpFr_c (21 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02121b48  src/_ZN8daKpFr_cD1Ev.cpp
 *   [1] 0x02121b88  src/_ZN8daKpFr_cD0Ev.cpp
 *   [2] 0x02121bdc  src/_ZN8daKpFr_c13OnYoshiTryEatEv.cpp
 *   [3] 0x02121be4  src/func_ov070_02121be4.cpp
 *   [4] 0x02121c8c  src/func_ov070_02121c8c.c
 *   [5] 0x02121cbc  src/func_ov070_02121cbc.c
 *   [6] 0x02121d50  src/func_ov070_02121d50.cpp
 *   [7] 0x02121e14  src/func_ov070_02121e14.cpp
 *   [8] 0x02121eb0  src/func_ov070_02121eb0.c
 *   [9] 0x02121ef8  src/func_ov070_02121ef8.c
 *   [10] 0x02121f18  src/func_ov070_02121f18.cpp
 *   [11] 0x02121fb0  src/func_ov070_02121fb0.c
 *   [12] 0x02121fd0  src/func_ov070_02121fd0.cpp
 *   [13] 0x0212200c  src/func_ov070_0212200c.cpp
 *   [14] 0x02122044  src/func_ov070_02122044.c
 *   [15] 0x02122060  src/_ZN8daKpFr_c16CleanupResourcesEv.cpp
 *   [16] 0x02122068  src/_ZN8daKpFr_c16OnPendingDestroyEv.cpp
 *   [17] 0x0212206c  src/_ZN8daKpFr_c6RenderEv.cpp
 *   [18] 0x02122104  src/_ZN8daKpFr_c8BehaviorEv.cpp
 *   [19] 0x02122124  src/_ZN8daKpFr_c13InitResourcesEv.cpp
 *   [20] 0x021221fc  src/daKpFr_c_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- daKpFr_c_Spawn, 0x021221fc, size 0x48 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daKpFr_c_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8daKpFr_c */
int *daKpFr_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV8daKpFr_c[2]; /* +8: this TU defines the vtable */
        _ZN11ShadowModelC1Ev((char *)p + 0xd4);
        _ZN7dCcAc_cC1Ev((char *)p + 0xfc);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x130);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN8daKpFr_c13InitResourcesEv, 0x02122124, size 0xd8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKpFr_c.h"
struct M48 { int w[12]; };
extern "C" {
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern int IDENTITY_MATRIX4X3[];
}

int daKpFr_c::InitResources()
{
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0xfc, ((char*)this), 0x37000, 0x78000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x130, ((char*)this), 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x400;
    mTerminalVelocity = -0x5000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov070_02122044(((char*)this), 0);
    *(struct M48*)((char*)&mMatrix) = *(struct M48*)IDENTITY_MATRIX4X3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN8daKpFr_c8BehaviorEv, 0x02122104, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKpFr_c.h"

int daKpFr_c::Behavior()
{
    func_ov070_02121fd0(((char*)this));
    func_ov070_02121e14(((char*)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN8daKpFr_c6RenderEv, 0x0212206c, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daKpFr_c.h"
extern "C" {
extern int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned int a, unsigned int b, int c, int d, int e, void* f);
}

int daKpFr_c::Render()
{
  int b = (mFlags & 0x40000) != 0;
  if (b) return 1;
  mParticle1 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle1, 0x7f, mPosX, mPosY + 0x4b000, mPosZ, 0);
  mParticle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      mParticle2, 0x80, mPosX, mPosY + 0x4b000, mPosZ, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN8daKpFr_c16OnPendingDestroyEv, 0x02122068, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c16OnPendingDestroyEv

#include "daKpFr_c.h"

void daKpFr_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN8daKpFr_c16CleanupResourcesEv, 0x02122060, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c16CleanupResourcesEv

#include "daKpFr_c.h"

int daKpFr_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov070_02122044, 0x02122044, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
typedef struct { int a, b, c, d; } Item16;

extern Item16 data_ov070_021236ec[];
extern void func_ov070_0212200c(void *self);

void func_ov070_02122044(void *vself, int idx)
{
    char *self = (char *)vself;
    *(Item16 **)(self + 0x31c) = &data_ov070_021236ec[idx];
    func_ov070_0212200c(self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov070_0212200c, 0x0212200c, size 0x38 */
/* -------------------------------------------------------------------------- */
struct C; typedef void (C::*PMF)();
struct C { char pad[0x31c]; PMF *pp; };
extern "C" void func_ov070_0212200c(void *vc) { C *c = (C *)vc; PMF *p = c->pp; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_02121fd0, 0x02121fd0, size 0x3c */
/* -------------------------------------------------------------------------- */
/* (struct C / PMF: defined once at ordinal 13 above) */
extern "C" void func_ov070_02121fd0(char *vc) { C *c = (C *)vc; PMF *p = c->pp + 1; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_02121fb0, 0x02121fb0, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov070_02121fb0
// recovered name: FlameChomp_Kill
/* recovered: renamed to Class_Method */
/* daKrpa_c::Kill - recovered from vtable slot identity */
int func_ov070_02121fb0(char *p)
{
    *(int *)(p + 0x98) = 40960;
    *(char *)(p + 0x32c) = 105;
    *(int *)(p + 0x320) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_02121f18, 0x02121f18, size 0x98 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov070_02121c8c(void* c);
extern void* _ZN8dActor_c22ClosestNonVanishPlayerEv(void* c);
extern short Vec3_HorzAngle(void* v0, void* v1);
extern void _Z14ApproachLinearRsss(short* p, short t, short step);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* c, void* cc);
extern void func_ov070_02121be4(void* c);
extern void func_ov070_02121d50(void* c, void* p);
extern void func_ov070_02121cbc(char* c);
extern void _ZN5dCc_c5ClearEv(void* cc);
extern void _ZN5dCc_c6UpdateEv(void* cc);
int func_ov070_02121f18(char* c) {
  char* p;
  if (DecIfAbove0_Byte((unsigned char*)(c + 0x32c)) == 0)
    func_ov070_02121c8c(c);
  p = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
  if (p) {
    short ang = Vec3_HorzAngle(c + 0x5c, p + 0x5c);
    _Z14ApproachLinearRsss((short*)(c + 0x8e), ang, 0x180);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
  }
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0xfc);
  func_ov070_02121be4(c);
  func_ov070_02121d50(c, c + 0x130);
  func_ov070_02121cbc(c);
  _ZN5dCc_c5ClearEv(c + 0xfc);
  _ZN5dCc_c6UpdateEv(c + 0xfc);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_02121ef8, 0x02121ef8, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN5dCc_c5ClearEv(void *);
int func_ov070_02121ef8(char *c)
{
    _ZN5dCc_c5ClearEv((char *)c + 0xfc);
    *(int *)(c + 0x320) = 1;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_02121eb0, 0x02121eb0, size 0x48 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void func_ov070_02121c8c(void *t);
int func_ov070_02121eb0(void *c) {
    int r2 = *(int *)((char *)c + 0xb0);
    int r1 = (r2 & 0x20000) ? 1 : 0;
    if (r1 == 0) {
        r1 = (r2 & 0x40000) ? 1 : 0;
        if (r1 == 0)
            func_ov070_02121c8c(c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_02121e14, 0x02121e14, size 0x9c */
/* -------------------------------------------------------------------------- */
struct dBgCh_Gnd { char pad[0x44]; int hit; char pad2[0x8]; };
extern "C" {
extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* r);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* r, void* v, void* a);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* r);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* m, int f1, int f2, unsigned int j);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* r);
void func_ov070_02121e14(char* c) {
  dBgCh_Gnd rg;
  int f;
  *(int*)(c + 0x310) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x314) = *(int*)(c + 0x60) >> 3;
  *(int*)(c + 0x318) = *(int*)(c + 0x64) >> 3;
  _ZN9dBgCh_GndC1Ev(&rg);
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, c + 0x5c, c);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0)
    f = (*(int*)(c + 0x60) - rg.hit) + 0x1e000;
  else
    f = 0x12c000;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0xd4, c + 0x2ec, 0x64000, f, 0xf);
  _ZN9dBgCh_GndD1Ev(&rg);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_02121d50, 0x02121d50, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02121d50
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

/* (dBgCh_Actr: real header type in scope; the legacy shadow's extra methods
 * are spelled by their mangled names below, same symbols, same calls) */
struct SurfaceInfo;
namespace cstd { int fdiv(int a, int b); }

extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void* c);
extern "C" SurfaceInfo* _ZNK10dBgCh_Actr14GetFloorResultEv(const void*);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const SurfaceInfo*, Vector3&);

extern "C" void func_ov070_02121d50(void* vself, void* vclsn) {
    int* self = (int*)vself;
    dBgCh_Actr* clsn = (dBgCh_Actr*)vclsn;
    Vector3 n;
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo*)((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4), n);
        if (n.y != 0) {
            int a = (int)(((long long)n.x * self[0x29] + 0x800) >> 12);
            int b = (int)(((long long)n.z * self[0x2b] + 0x800) >> 12);
            self[0x2a] = -(cstd::fdiv(a + b, n.y) + 0x8000);
        }
    }
    if (clsn->IsOnWall())
        func_ov070_02121c8c(self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_02121cbc, 0x02121cbc, size 0x94 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void func_ov070_02122044(void* c, int a);
extern void _ZN6Player4BurnEv(char* p);
extern void func_ov070_02121c8c(void* c);

void func_ov070_02121cbc(char* r4){
  char* found;
  int b;
  unsigned int id = *(unsigned int*)(r4 + 0x120);
  if (id == 0) return;
  found = _ZN8dActor_c10FindWithIDEj(id);
  if (found == 0) return;
  b = (int)(*(unsigned short*)(found + 0xc) == 0xbf);
  if (b == 0) return;
  b = (int)((*(int*)(r4 + 0xb0) & 0x20000) != 0);
  if (b != 0) {
    func_ov070_02122044(r4, 1);
    return;
  }
  if (*(unsigned char*)(found + 0x6fb) != 0) return;
  _ZN6Player4BurnEv(found);
  func_ov070_02121c8c(r4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_02121c8c, 0x02121c8c, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void func_02012694(int id, void *pos);
extern void _ZN8dActor_c13SmallPoofDustEv(void *c);
extern void _ZN7fBase_c18MarkForDestructionEv(void *c);
void func_ov070_02121c8c(void *c)
{
    func_02012694(0x166, (char*)c + 0x74);
    _ZN8dActor_c13SmallPoofDustEv(c);
    _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_02121be4, 0x02121be4, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02121be4
/* recovered: shared common types */
#include "common.h"

/* (dBgCh_Gnd: defined once at ordinal 7 above; declarations below match
 * that block's extern "C" signatures exactly -- C linkage cannot overload) */
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *thiz);

extern "C" void func_ov070_02121be4(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct dBgCh_Gnd rg;
    struct Vector3 v;

    if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x130)) return;

    _ZN9dBgCh_GndC1Ev(&rg);
    {
        int z = *(int *)(c + 0x64);
        int y = *(int *)(c + 0x60) + 0x1e000;
        int x = *(int *)(c + 0x5c);
        v.x = x;
        v.y = y;
        v.z = z;
    }
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, (void *)c);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) == 0 ||
        *(int *)((char *)&rg + 0x44) < *(int *)(c + 0x60) - 0x32000) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
    }
    _ZN9dBgCh_GndD1Ev(&rg);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8daKpFr_c13OnYoshiTryEatEv, 0x02121bdc, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_c13OnYoshiTryEatEv

#include "daKpFr_c.h"

int daKpFr_c::OnYoshiTryEat()
{
    return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8daKpFr_cD0Ev, 0x02121b88, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_cD0Ev

#include "daKpFr_c.h"

/* (no separate definition: the single ~daKpFr_c() below emits the D0 and
 * D1 variants together; mwccarm orders the variant group itself.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8daKpFr_cD1Ev, 0x02121b48, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKpFr_cD1Ev

#include "daKpFr_c.h"

