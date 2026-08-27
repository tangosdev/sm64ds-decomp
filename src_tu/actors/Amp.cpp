//cpp
/* HAND-ASSEMBLED translation unit -- ov070/Amp (19 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02120570  src/game/actors/Amp/_ZN3AmpD1Ev.cpp
 *   [1] 0x021205d0  src/game/actors/Amp/_ZN3AmpD0Ev.cpp
 *   [2] 0x02120644  src/unnamed/ov070/0212/func_ov070_02120644.cpp
 *   [3] 0x02120724  src/unnamed/ov070/0212/func_ov070_02120724.c
 *   [4] 0x021208a4  src/unnamed/ov070/0212/func_ov070_021208a4.c
 *   [5] 0x02120910  src/unnamed/ov070/0212/func_ov070_02120910.cpp
 *   [6] 0x021209e4  src/unnamed/ov070/0212/func_ov070_021209e4.cpp
 *   [7] 0x02120bf8  src/unnamed/ov070/0212/func_ov070_02120bf8.cpp
 *   [8] 0x02120cac  src/unnamed/ov070/0212/func_ov070_02120cac.cpp
 *   [9] 0x02120ce4  src/unnamed/ov070/0212/func_ov070_02120ce4.c
 *   [10] 0x02120d34  src/unnamed/ov070/0212/func_ov070_02120d34.cpp
 *   [11] 0x02120d70  src/unnamed/ov070/0212/func_ov070_02120d70.cpp
 *   [12] 0x02120da8  src/unnamed/ov070/0212/func_ov070_02120da8.c
 *   [13] 0x02120dc4  src/game/actors/Amp/_ZN3Amp16CleanupResourcesEv.cpp
 *   [14] 0x02120e20  src/game/actors/Amp/_ZN3Amp16OnPendingDestroyEv.cpp
 *   [15] 0x02120e24  src/game/actors/Amp/_ZN3Amp6RenderEv.cpp
 *   [16] 0x02120e8c  src/game/actors/Amp/_ZN3Amp8BehaviorEv.cpp
 *   [17] 0x02120eec  src/game/actors/Amp/_ZN3Amp13InitResourcesEv.cpp
 *   [18] 0x021210ac  src/game/actors/Amp/Amp_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- Amp_Spawn, 0x021210ac, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol Amp_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_TextureTransformer.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV3Amp */
int *Amp_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1076);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV3Amp[2]; /* +8: this TU defines the vtable */
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x138);
        _ZN15TextureSequenceC1Ev((char *)p + 0x188);
        _ZN18TextureTransformerC1Ev((char *)p + 0x19c);
        _ZN11ShadowModelC1Ev((char *)p + 0x1b0);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x1d8);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x218);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN3Amp13InitResourcesEv, 0x02120eec, size 0x1c0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3Amp13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Amp.h"
#include "TextureSequence.h"
struct SharedFilePtr;
struct BMD_File;
struct BTA_File;
struct dActor_c;
struct Vector3;
struct Vector3_16;

extern "C" void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(dActor_c *self, int a, int b, int c, int d);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, dActor_c *a, Vector3 const &b, int c, int d, unsigned int e, unsigned int f);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
extern "C" void func_ov070_02120da8(char *c, int a);
extern "C" void func_ov070_02120724(char *c);

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[];
extern SharedFilePtr data_ov070_021235ec;
extern BTA_File data_ov070_021231f4;
extern Vector3 data_ov070_0212365c;
extern char IDENTITY_MATRIX4X3;

struct M48 { int w[12]; };

int Amp::InitResources()
{
    BMD_File *bmd;
    bmd = (BMD_File *)Model::LoadFile(data_ov070_021235fc);
    mModelAnim.SetFile(bmd, 1, 1);
    bmd = (BMD_File *)Model::LoadFile(data_ov070_02123604);
    mModel.SetFile(bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        Animation::LoadFile(*data_ov070_021222e0[i]);
    }

    BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    BTP_File *btp = (BTP_File *)TextureSequence::LoadFile(data_ov070_021235ec);
    TextureSequence::Prepare(*bmd2, *btp);

    bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    TextureTransformer::Prepare(*bmd2, data_ov070_021231f4);

    if (!mShadowModel.InitCylinder())
        return 0;

    if ((unsigned char)((param1 >> 1) & 1)) {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x20d000, 0x1000000, 0xa28000);
    } else {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0, 0x2c1000, 0x1000000, 0xa28000);
    }

    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, data_ov070_0212365c, 0x2d000, 0x50000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x2d000, 0x2d000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    func_ov070_02120da8((char *)this, 1);

    *(M48 *)&mMat4x3 = *(M48 *)&IDENTITY_MATRIX4X3;

    func_ov070_02120724((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN3Amp8BehaviorEv, 0x02120e8c, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3Amp8BehaviorEv
#include "Amp.h"

extern "C" {
extern void func_ov070_02120d34(void *c);
extern void func_ov070_02120724(char *c);
}

int Amp::Behavior()
{
    func_ov070_02120d34(this);
    mCylinderOffset.y += data_ov070_0212365c.y;  /* was int[] view's [1]; same word */
    mdCcAcPos_c.SetPosRelativeToActor(mCylinderOffset);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    func_ov070_02120724((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN3Amp6RenderEv, 0x02120e24, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3Amp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Amp.h"

int Amp::Render()
{
    mModelAnim.Render(0);

    if (mState != 0 && mState != 2) {
        mTextureSequence.Update(mModel.data);
        mTextureTransformer.Update(mModel.data);
        mModel.Render((Vector3 *)&mScaleX);
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN3Amp16OnPendingDestroyEv, 0x02120e20, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3Amp16OnPendingDestroyEv

#include "Amp.h"

void Amp::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN3Amp16CleanupResourcesEv, 0x02120dc4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3Amp16CleanupResourcesEv

#include "Amp.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[2];
extern SharedFilePtr data_ov070_021235ec;

int Amp::CleanupResources()
{
    data_ov070_021235fc.Release();
    data_ov070_02123604.Release();

    int i = 0;
    do {
        data_ov070_021222e0[i]->Release();
        i++;
    } while (i < 2);

    data_ov070_021235ec.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_02120da8, 0x02120da8, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern char data_ov070_02123668;
extern void func_ov070_02120d70(void *c);
void func_ov070_02120da8(char *c, int a) {
    *(int *)(c + 0x41c) = (int)&data_ov070_02123668 + (a << 4);
    func_ov070_02120d70(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_02120d70, 0x02120d70, size 0x38 */
/* -------------------------------------------------------------------------- */
struct C; typedef void (C::*PMF)();
struct C { char pad[0x41c]; PMF *pp; };
extern "C" void func_ov070_02120d70(void *vc) { C *c = (C *)vc; PMF *p = c->pp; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_02120d34, 0x02120d34, size 0x3c */
/* -------------------------------------------------------------------------- */
/* (struct C / PMF: defined once at ordinal 11 above) */
extern "C" void func_ov070_02120d34(void *vc) { C *c = (C *)vc; PMF *p = c->pp + 1; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_02120ce4, 0x02120ce4, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov070_0212360c[];
int func_ov070_02120ce4(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov070_0212360c[1], 0x40000000, 0x1000, 0);
    *(unsigned char*)(c+0x430) = 0x3c;
    *(int*)(c+0x420) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_02120cac, 0x02120cac, size 0x38 */
/* -------------------------------------------------------------------------- */
/* (Animation: real header type in scope) */

extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" void func_ov070_02120da8(char *c, int a);

extern "C" int func_ov070_02120cac(char *c) {
    ((Animation *)(c + 0x124))->Advance();
    unsigned char r = DecIfAbove0_Byte((unsigned char *)(c + 0x430));
    if (r == 0) {
        func_ov070_02120da8(c, 1);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_02120bf8, 0x02120bf8, size 0xb4 */
/* -------------------------------------------------------------------------- */
typedef int Fix12i;
struct BCA_File; struct BTP_File; struct BTA_File;
/* (ModelAnim/TextureSequence/TextureTransformer: real header types in scope) */

extern "C" unsigned int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence*, BTP_File&, int, Fix12i, unsigned int);
extern "C" unsigned int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer*, BTA_File&, int, Fix12i, unsigned int);

struct D1 { int a; BCA_File* f; };
struct D2 { int a; BTP_File* f; };
extern D1 data_ov070_021235f4;
extern BTA_File data_ov070_021231f4;

extern "C" void func_ov070_02120bf8(char* thiz)
{
    char* c = thiz;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (ModelAnim*)(c + 0xd4), data_ov070_021235f4.f, 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        (TextureSequence*)(c + 0x188), *((D2 *)&data_ov070_021235ec)->f, 0, 0x1000, 0);
    *(int*)(c + 0x194) = 0x1000;
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        (TextureTransformer*)(c + 0x19c), data_ov070_021231f4, 0, 0x1000, 0);
    *(int*)(c + 0x1a8) = 0x2000;
    *(unsigned char*)(c + 0x430) = 0xf;
    *(int*)(c + 0x424) = 0;
    *(short*)(c + 0x42e) = 0;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    *(int*)(c + 0x420) = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_021209e4, 0x021209e4, size 0x214 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_021209e4
/* recovered: shared common types */
#include "common.h"
extern "C" {
typedef long long s64;


/* (Animation: real header type in scope) */

void _Z14ApproachLinearRiii(int *val, int target, int step);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, Vector3 *v, unsigned int d);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int func_ov070_02120644(char *c);
extern short data_02082214[];

int func_ov070_021209e4(char *c) {
    _Z14ApproachLinearRiii((int*)(c + 0x424), 1000, 20);

    unsigned char bit0 = (unsigned char)(*(int*)(c + 8) & 1);
    if (bit0) {
        *(short*)(c + 0x94) = (short)(*(short*)(c + 0x94) + *(int*)(c + 0x424));
    } else {
        *(short*)(c + 0x94) = (short)(*(short*)(c + 0x94) - *(int*)(c + 0x424));
    }

    *(short*)(c + 0x42c) = (short)(*(short*)(c + 0x42c) + 0xa00);

    unsigned char bit1 = (unsigned char)(((unsigned int)*(int*)(c + 8) >> 1) & 1);
    int ip = bit1 ? 0xb4000 : 0x168000;

    *(int*)(c + 0x404) = *(int*)(c + 0x5c);
    *(int*)(c + 0x408) = *(int*)(c + 0x60);
    *(int*)(c + 0x40c) = *(int*)(c + 0x64);

    {
        int idx = (*(unsigned short*)(c + 0x94)) >> 4;
        int cosv = data_02082214[idx * 2];
        *(int*)(c + 0x410) = (int)(((s64)ip * cosv + 0x800) >> 0xc);
    }
    {
        int idx = (*(unsigned short*)(c + 0x42c)) >> 4;
        int sinv = data_02082214[idx * 2];
        *(int*)(c + 0x414) = (int)(((s64)0x14000 * sinv + 0x800) >> 0xc);
    }
    {
        int idx = (*(unsigned short*)(c + 0x94)) >> 4;
        int sinv = data_02082214[idx * 2 + 1];
        *(int*)(c + 0x418) = (int)(((s64)ip * sinv + 0x800) >> 0xc);
    }

    AddVec3((Vector3*)(c + 0x404), (Vector3*)(c + 0x410), (Vector3*)(c + 0x404));

    *(short*)(c + 0x42e) = (short)(*(short*)(c + 0x42e) + 0x4000);

    {
        int idx = (*(unsigned short*)(c + 0x42e)) >> 4;
        int cosv = data_02082214[idx * 2];
        int scale = (int)(((s64)0x555 * cosv + 0x800) >> 0xc) + 0x800;
        *(int*)(c + 0x80) = scale;
        *(int*)(c + 0x84) = scale;
        *(int*)(c + 0x88) = scale;
    }

    ((Animation*)(c + 0x124))->Advance();
    ((Animation*)(c + 0x188))->Advance();
    ((Animation*)(c + 0x19c))->Advance();

    *(unsigned int*)(c + 0x428) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x428), 3, 0x183, (Vector3*)(c + 0x74), 0);

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x430)) == 0) {
        func_ov070_02120644(c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_02120910, 0x02120910, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02120910
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);

int func_ov070_02120910(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    *(int*)(((int)c + 0xb0)) &= ~1;
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x5c) = *(int*)(c + 0x404);
    *(int*)(c + 0x60) = *(int*)(c + 0x408);
    *(int*)(c + 0x64) = *(int*)(c + 0x40c);
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(unsigned char*)(c + 0x430) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void**)((char*)data_ov070_0212360c + 4), 0x40000000, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    *(int*)(c + 0x420) = 2;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_021208a4, 0x021208a4, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int _ZN9Animation7AdvanceEv(void *);
extern int _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *);
extern unsigned char DecIfAbove0_Byte(unsigned char *);
extern int _ZN8dActor_c8PoofDustEv(void *);
extern int _ZN7fBase_c18MarkForDestructionEv(void *);
int func_ov070_021208a4(char *c){
 *(short*)(c+0x8c)=*(short*)(c+0x8c)-0x1000;
 _ZN9Animation7AdvanceEv((char*)c+0x124);
 _ZN8dActor_c9UpdatePosEP5dCc_c(c,(char*)c+0x1d8);
 dBgCh_Actr_UpdateDiscreteNoLava_veneer((char*)c+0x218);
 if(_ZNK10dBgCh_Actr13JustHitGroundEv((char*)c+0x218)==0){
   if(DecIfAbove0_Byte((unsigned char*)c+0x430)!=0) goto end;
 }
 _ZN8dActor_c8PoofDustEv(c);
 _ZN7fBase_c18MarkForDestructionEv(c);
end:
 return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_02120724, 0x02120724, size 0x180 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov070_02120724
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;




extern void _ZN9dBgCh_GndC1Ev(void *rg);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *rg, struct Vector3 *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *rg);
extern void _ZN9dBgCh_GndD1Ev(void *rg);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *c, void *sm, void *mtx, int h, int g, unsigned int u);
extern signed char data_0209f2f8[];

struct RG { char pad[0x44]; int ground; char pad2[8]; };

void func_ov070_02120724(char *c)
{
    int g;

    if (*(int*)(c + 0x420) == 2) {
        Matrix4x3_FromRotationXYZExt(c + 0xf0,
            *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));
        *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
        *(int*)(c + 0x3f8) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x3fc) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x400) = *(int*)(c + 0x64) >> 3;
    } else {
        *(int*)(c + 0x114) = *(int*)(c + 0x404) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x408) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x40c) >> 3;
        *(struct Matrix4x3*)(c + 0x154) = *(struct Matrix4x3*)(c + 0xf0);
        *(int*)(c + 0x3f8) = *(int*)(c + 0x404) >> 3;
        *(int*)(c + 0x3fc) = *(int*)(c + 0x408) >> 3;
        *(int*)(c + 0x400) = *(int*)(c + 0x40c) >> 3;
    }

    g = 0x1f4000;
    if (data_0209f2f8[0] == 0x11) {
        struct RG rg;
        struct Vector3 pos;
        int y;
        pos.x = *(int*)(c + 0x5c);
        y = *(int*)(c + 0x60);
        pos.y = y;
        pos.z = *(int*)(c + 0x64);
        pos.y = y - 0xa000;
        _ZN9dBgCh_GndC1Ev(&rg);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, 0);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
            g = (*(int*)(c + 0x60) - rg.ground) + 0x28000;
        }
        _ZN9dBgCh_GndD1Ev(&rg);
    }

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x1b0, c + 0x3d4, 0x5a000, g, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov070_02120644, 0x02120644, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02120644
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _ZN6Player16IncMegaKillCountEv(void* thiz);
extern void func_ov070_02120da8(char* c, int n);
extern int _ZN6Player5ShockEj(void* thiz, unsigned int n);

int func_ov070_02120644(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0x1fc);
    if (id == 0) return 0;
    char* o = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (o == 0) goto ret0;
    {
        int b = (*(unsigned short*)(o + 0xc) == 0xbf);
        if (b != 0) goto cont;
    }
ret0:
    return 0;
cont:
    if (*(unsigned char*)(o + 0x6fb) != 0) return 0;
    if (*(int*)(c + 0x1f8) & 0x10) {
        *(short*)(c + 0x94) = Vec3_HorzAngle((Vector3*)(o + 0x5c), (Vector3*)(c + 0x5c));
        *(short*)(c + 0x8e) = (short)(*(short*)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(o);
        func_ov070_02120da8(c, 2);
    } else {
        if (_ZN6Player5ShockEj(o, 1) != 0)
            func_ov070_02120da8(c, 0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN3AmpD0Ev, 0x021205d0, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3AmpD0Ev

#include "Amp.h"

/* (no separate definition: the single ~Amp() below emits the D0 and D1
 * variants together; mwccarm orders the variant group itself.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN3AmpD1Ev, 0x02120570, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN3AmpD1Ev

#include "Amp.h"

Amp::~Amp()
{
}

