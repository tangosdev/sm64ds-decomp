//cpp
/* HAND-ASSEMBLED translation unit -- ov070/daKrpa_c (25 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02121118  src/_ZN8daKrpa_cD1Ev.cpp
 *   [1] 0x02121160  src/_ZN8daKrpa_cD0Ev.cpp
 *   [2] 0x021211bc  src/_ZN8daKrpa_c13OnYoshiTryEatEv.cpp
 *   [3] 0x021211c4  src/func_ov070_021211c4.cpp
 *   [4] 0x02121298  src/func_ov070_02121298.cpp
 *   [5] 0x02121310  src/func_ov070_02121310.cpp
 *   [6] 0x021213cc  src/func_ov070_021213cc.c
 *   [7] 0x02121438  src/func_ov070_02121438.cpp
 *   [8] 0x021214f8  src/func_ov070_021214f8.cpp
 *   [9] 0x02121548  src/func_ov070_02121548.c
 *   [10] 0x0212156c  src/func_ov070_0212156c.c
 *   [11] 0x021216b8  src/func_ov070_021216b8.c
 *   [12] 0x02121710  src/func_ov070_02121710.cpp
 *   [13] 0x021217ac  src/func_ov070_021217ac.c
 *   [14] 0x0212180c  src/func_ov070_0212180c.cpp
 *   [15] 0x02121848  src/func_ov070_02121848.cpp
 *   [16] 0x02121880  src/func_ov070_02121880.c
 *   [17] 0x0212189c  src/_ZN8daKrpa_c16CleanupResourcesEv.cpp
 *   [18] 0x021218c0  src/_ZN8daKrpa_c16OnPendingDestroyEv.cpp
 *   [19] 0x021218c4  src/_ZN8daKrpa_c6RenderEv.cpp
 *   [20] 0x021218f4  src/_ZN8daKrpa_c8BehaviorEv.cpp
 *   [21] 0x02121914  src/_ZN8daKrpa_c13InitResourcesEv.cpp
 *   [22] 0x02121a64  src/func_ov070_02121a64.c
 *   [23] 0x02121ae0  src/func_ov070_02121ae0.c
 *   [24] 0x02121af8  src/daKrpa_c_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- daKrpa_c_Spawn, 0x02121af8, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daKrpa_c_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8daKrpa_c */
int *daKrpa_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(944);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV8daKrpa_c[2]; /* +8: this TU defines the vtable */
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x138);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x160);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1a0);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov070_02121ae0, 0x02121ae0, size 0x18 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov070_02121ae0(void *c, int a, int b, int d)
{
    *(int *)((char *)c + 4) = a;
    *(int *)((char *)c + 8) = b;
    *(int *)c = d;
    *(int *)((char *)c + 0xc) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov070_02121a64, 0x02121a64, size 0x7c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern unsigned int __aeabi_uidiv(unsigned int a, unsigned int b);

int func_ov070_02121a64(void* vc)
{
    char* c = (char*)vc;
    switch (*(int*)c) {
    case 0:
        if (*(unsigned int*)(c + 0xc) < *(unsigned int*)(c + 8)) {
            unsigned int* p = (unsigned int*)(((int)c + 0xc));
            *p = *p + 1;
        }
        break;
    case 1:
        {
            unsigned int* p = (unsigned int*)(((int)c + 0xc));
            *p = *p + 1;
            *p = *p % *(unsigned int*)(c + 8);
        }
        break;
    }
    return (*(unsigned int**)(c + 4))[*(unsigned int*)(c + 0xc)];
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN8daKrpa_c13InitResourcesEv, 0x02121914, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKrpa_c.h"
/* was `typedef int Fix12;` -- collides with the real Fix12<> template, which
   daKrpa_c.h now reaches via Model.h. The typedef WAS int, so spelling it
   int below is byte-neutral. */
struct RG { char pad[0x44]; int f44; char pad2[8]; };
struct Blk { int w[12]; };

extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* self);
}
extern "C" {
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void* self, void* actor, const struct Vector3* v, int a, int b, unsigned int c, unsigned int d);
}
extern "C" {
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void* self, void* actor, int a, int b, void* v, int c);
}
extern "C" {
extern void _ZN9dBgCh_GndC1Ev(struct RG* rg);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct RG* rg, const struct Vector3* v, void* a);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct RG* rg);
extern void func_ov070_02121310(char* c);
extern void _ZN9dBgCh_GndD1Ev(struct RG* rg);
}

extern struct Blk IDENTITY_MATRIX4X3;

int daKrpa_c::InitResources()
{
    struct RG rg;
    struct Vector3 v;
    void* bmd;
    int t;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov070_02123698);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, bmd, 1, 1);
    if (!_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel))
        return 0;

    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char*)this) + 0x160, ((char*)this), &v, 0x32000, 0x64000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        ((char*)this) + 0x1a0, ((char*)this), 0x32000, 0x32000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    func_ov070_02121880(((char*)this), 0);

    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(struct Blk*)((char*)&mMatrix) = IDENTITY_MATRIX4X3;

    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, (struct Vector3*)((char*)&mPosX), ((char*)this));
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
        t = (mPosY - rg.f44) + 0x1e000;
    else
        t = 0x1f4000;
    mHeightAboveGnd = t;
    func_ov070_02121310(((char*)this));
    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN8daKrpa_c8BehaviorEv, 0x021218f4, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKrpa_c.h"
extern "C" {
extern void func_ov070_02121310(char* c);
}

int daKrpa_c::Behavior()
{
    func_ov070_0212180c(((char*)this));
    func_ov070_02121310(((char*)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN8daKrpa_c6RenderEv, 0x021218c4, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daKrpa_c.h"
struct RenderBase { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };
struct RenderSub : RenderBase { };
struct RenderView { char p1[0xd4]; RenderSub sub; };  /* was C/Sub/Base; renamed: another member's shadow C has a different layout */

int daKrpa_c::Render()
{
  ((RenderView*)this)->sub.M((char*)&mScaleX);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN8daKrpa_c16OnPendingDestroyEv, 0x021218c0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c16OnPendingDestroyEv

#include "daKrpa_c.h"

void daKrpa_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN8daKrpa_c16CleanupResourcesEv, 0x0212189c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c16CleanupResourcesEv

#include "daKrpa_c.h"
#include "SharedFilePtr.h"

int daKrpa_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov070_02123698)->Release();  /* declared void* earlier in this TU; same object */
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov070_02121880, 0x02121880, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern char data_ov070_021236ac;
extern void func_ov070_02121848(char *c);
void func_ov070_02121880(void *vc, int a) {
    char *c = (char *)vc;
    *(int *)(c + 0x39c) = (int)&data_ov070_021236ac + (a << 4);
    func_ov070_02121848(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov070_02121848, 0x02121848, size 0x38 */
/* -------------------------------------------------------------------------- */
struct C; typedef void (C::*PMF)();
struct C { char pad[0x39c]; PMF *pp; };
extern "C" void func_ov070_02121848(char *vc) { C *c = (C *)vc; PMF *p = c->pp; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov070_0212180c, 0x0212180c, size 0x3c */
/* -------------------------------------------------------------------------- */
/* (struct C / PMF: defined once at ordinal 15 above) */
extern "C" void func_ov070_0212180c(char *vc) { C *c = (C *)vc; PMF *p = c->pp + 1; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov070_021217ac, 0x021217ac, size 0x60 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void func_ov070_02121ae0(void*, int, int, int);
extern void* data_ov070_02122404;
int func_ov070_021217ac(char* c){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, &data_ov070_021234c4, 0, 0x1000, 0);
    func_ov070_02121ae0(c + 0x38c, (int)&data_ov070_02122404, 0x64, 1);
    *(unsigned char*)(c + 0x3ac) = 0x73;
    *(int*)(c + 0x3a0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_02121710, 0x02121710, size 0x9c */
/* -------------------------------------------------------------------------- */
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov070_02121880(void* c, int a);
extern void _ZN9Animation7AdvanceEv(void* a);
extern int func_ov070_02121a64(void* p);
extern void func_ov070_02121298(char* c);
extern void func_ov070_021211c4(char* c);
extern void _ZN5dCc_c5ClearEv(void* cc);
extern void _ZN5dCc_c6UpdateEv(void* cc);
extern int data_0209f32c;
int func_ov070_02121710(char* c) {
  int r;
  if (*(int*)(c + 0x3a4) != 0) {
    if (*(int*)(c + 0x60) > data_0209f32c) {
      if (DecIfAbove0_Byte((unsigned char*)(c + 0x3ac)) == 0)
        func_ov070_02121880(c, 1);
    }
  } else {
    *(unsigned char*)(c + 0x3ac) = 0x73;
  }
  _ZN9Animation7AdvanceEv(c + 0x124);
  r = func_ov070_02121a64(c + 0x38c);
  *(int*)(c + 0x80) = r;
  *(int*)(c + 0x84) = r;
  *(int*)(c + 0x88) = r;
  func_ov070_02121298(c);
  func_ov070_021211c4(c);
  _ZN5dCc_c5ClearEv(c + 0x160);
  _ZN5dCc_c6UpdateEv(c + 0x160);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_021216b8, 0x021216b8, size 0x58 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void func_ov070_02121ae0(void*, int, int, int);
extern void* data_ov070_021234dc;
extern void* data_ov070_021222e8;
int func_ov070_021216b8(void* c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0xd4, &data_ov070_021234dc, 0x40000000, 0x1000, 0);
    func_ov070_02121ae0((char*)c + 0x38c, (int)&data_ov070_021222e8, 0x47, 0);
    *(int*)((char*)c + 0x3a0) = 1;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_0212156c, 0x0212156c, size 0x14c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov070_0212156c
/* recovered: shared common types */
#include "common.h"
extern short data_02082214[];
void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, struct Vector3 *pos, void *v16, int d, int e);
void func_0201267c(int a, void *p);
void func_ov070_02121880(void *c, int i);
void _ZN9Animation7AdvanceEv(void *thiz);
int func_ov070_02121a64(void *p);
void func_ov070_02121298(char *c);
void func_ov070_021211c4(char *c);
void _ZN5dCc_c5ClearEv(void *thiz);
void _ZN5dCc_c6UpdateEv(void *thiz);

int func_ov070_0212156c(char *c){
  if(*(int*)(c+0x398) == 0x1e){
    struct Vector3 pos;
    int idx = (int)*(unsigned short*)(c+0x8e) >> 4;
    int s = data_02082214[idx*2+1];
    int cn = data_02082214[idx*2];
    int offZ = (int)(((long long)s * 0x50000 + 0x800) >> 12);
    int offX = (int)(((long long)cn * 0x50000 + 0x800) >> 12);
    int x = *(int*)(c+0x5c) + offX;
    int z = *(int*)(c+0x64) + offZ;
    int y = *(int*)(c+0x60) - 0x29000;
    ((int*)&pos)[0] = x;
    ((int*)&pos)[2] = z;
    ((int*)&pos)[1] = y;
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10f, 0, &pos, c+0x8c, *(signed char*)(c+0xcc), -1);
    func_0201267c(0x105, c+0x74);
  }
  if(*(int*)(c+0x398) == *(int*)(c+0x394)){
    func_ov070_02121880(c, 0);
  }
  _ZN9Animation7AdvanceEv(c+0x124);
  {
    int r = func_ov070_02121a64(c+0x38c);
    *(int*)(c+0x80) = r;
    *(int*)(c+0x84) = r;
    *(int*)(c+0x88) = r;
  }
  func_ov070_02121298(c);
  func_ov070_021211c4(c);
  _ZN5dCc_c5ClearEv(c+0x160);
  _ZN5dCc_c6UpdateEv(c+0x160);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_02121548, 0x02121548, size 0x24 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN5dCc_c5ClearEv(void *);
int func_ov070_02121548(char *c)
{
    _ZN5dCc_c5ClearEv((char *)c + 0x160);
    *(int *)(c + 0x3a0) = 2;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_021214f8, 0x021214f8, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" void _ZN8dActor_c8PoofDustEv(void *c);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void *c);
extern "C" int func_ov070_021214f8(char *c)
{
    int flags;
    int b;
    flags = *(int*)(c + 0xb0);
    b = (flags & 0x20000) != 0;
    if (!b) {
        b = (flags & 0x40000) != 0;
        if (!b) {
            _ZN8dActor_c8PoofDustEv(c);
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_02121438, 0x02121438, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02121438
// recovered name: Amp_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daBrq_c::Kill - recovered from vtable slot identity */
extern "C" {
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
}

extern "C" int func_ov070_02121438(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    int* p_b0 = (int*)(((int)c + 0xb0));
    *p_b0 = *p_b0 & ~1;
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    *(unsigned char*)(c + 0x3ac) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, &data_ov070_021234c4, 0, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    *(int*)(c + 0x3a0) = 3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_021213cc, 0x021213cc, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *);
extern unsigned char DecIfAbove0_Byte(unsigned char *);
extern void _ZN8dActor_c8PoofDustEv(void *);
extern void _ZN7fBase_c18MarkForDestructionEv(void *);
int func_ov070_021213cc(char *c){
 *(short*)(c+0x8c)=*(short*)(c+0x8c)-0x1000;
 _ZN9Animation7AdvanceEv((char*)c+0x124);
 _ZN8dActor_c9UpdatePosEP5dCc_c(c,(char*)c+0x160);
 dBgCh_Actr_UpdateDiscreteNoLava_veneer((char*)c+0x1a0);
 if(_ZNK10dBgCh_Actr13JustHitGroundEv((char*)c+0x1a0)==0){
   if(DecIfAbove0_Byte((unsigned char*)c+0x3ac)!=0) goto end;
 }
 _ZN8dActor_c8PoofDustEv(c);
 _ZN7fBase_c18MarkForDestructionEv(c);
end:
 return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_02121310, 0x02121310, size 0xbc */
/* -------------------------------------------------------------------------- */
extern "C" void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int f, int t, unsigned int u);

extern "C" void func_ov070_02121310(char *thiz)
{
    if (*(int*)(thiz + 0x3a0) == 3) {
        Matrix4x3_FromRotationXYZExt(thiz + 0xf0,
            *(short*)(thiz + 0x8c),
            *(short*)(thiz + 0x8e),
            *(short*)(thiz + 0x90));
    } else {
        Matrix4x3_FromRotationY(thiz + 0xf0, *(short*)(thiz + 0x8e));
    }
    *(int*)(thiz + 0x114) = *(int*)(thiz + 0x5c) >> 3;
    *(int*)(thiz + 0x118) = *(int*)(thiz + 0x60) >> 3;
    *(int*)(thiz + 0x11c) = *(int*)(thiz + 0x64) >> 3;
    *(int*)(thiz + 0x380) = *(int*)(thiz + 0x5c) >> 3;
    *(int*)(thiz + 0x384) = *(int*)(thiz + 0x60) >> 3;
    *(int*)(thiz + 0x388) = *(int*)(thiz + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        thiz, thiz + 0x138, thiz + 0x35c,
        *(int*)(thiz + 0x80) * 0x46,
        *(int*)(thiz + 0x3a8), 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_02121298, 0x02121298, size 0x78 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern char* _ZN8dActor_c22ClosestNonVanishPlayerEv();
extern int Vec3_Dist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(short* p, short target, short step);
}

extern "C" void func_ov070_02121298(char* c){
    char* p = _ZN8dActor_c22ClosestNonVanishPlayerEv();
    if (p == 0) {
        *(int*)(c + 0x3a4) = 0;
        return;
    }
    if (Vec3_Dist(c + 0x5c, p + 0x5c) >= 0x2bc000) {
        *(int*)(c + 0x3a4) = 0;
        return;
    }
    *(char**)(c + 0x3a4) = p;
    _Z14ApproachLinearRsss((short*)(c + 0x8e), Vec3_HorzAngle(c + 0x5c, p + 0x5c), 0x800);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_021211c4, 0x021211c4, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_021211c4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _ZN6Player16IncMegaKillCountEv(void* thiz);

void func_ov070_021211c4(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x184);
    if (id == 0) return;
    char* o = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (o == 0) return;
    int b = (*(unsigned short*)(o + 0xc) == 0xbf);
    if (b == 0) return;
    int b2 = ((*(int*)(c + 0xb0) & 0x20000) != 0);
    if (b2 != 0) {
        func_ov070_02121880(c, 2);
        return;
    }
    if ((*(int*)(c + 0x180) & 0x10) == 0) return;
    *(short*)(c + 0x94) = Vec3_HorzAngle((void*)(o + 0x5c), (void*)(c + 0x5c));
    *(short*)(c + 0x8e) = (short)(*(short*)(c + 0x94) + 0x8000);
    _ZN6Player16IncMegaKillCountEv(o);
    func_ov070_02121880(c, 3);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8daKrpa_c13OnYoshiTryEatEv, 0x021211bc, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c13OnYoshiTryEatEv

#include "daKrpa_c.h"

int daKrpa_c::OnYoshiTryEat()
{
    return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8daKrpa_cD0Ev, 0x02121160, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_cD0Ev

#include "daKrpa_c.h"

/* (no separate definition: the single ~daKrpa_c() below emits the D0 and
 * D1 variants together; mwccarm orders the variant group itself.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8daKrpa_cD1Ev, 0x02121118, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_cD1Ev

#include "daKrpa_c.h"

daKrpa_c::~daKrpa_c()
{
}

