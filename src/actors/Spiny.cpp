//cpp
/* HAND-ASSEMBLED translation unit -- ov077/Spiny (34 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02124b64  src/_ZN5SpinyD1Ev.cpp
 *   [1] 0x02124bb4  src/_ZN5SpinyD0Ev.c
 *   [2] 0x02124c18  src/_ZN5Spiny13OnYoshiTryEatEv.cpp
 *   [3] 0x02124c20  src/_ZN5Spiny16OnAimedAtWithEggEv.cpp
 *   [4] 0x02124c28  src/func_ov077_02124c28.cpp
 *   [5] 0x02124ce4  src/func_ov077_02124ce4.c
 *   [6] 0x02124d08  src/func_ov077_02124d08.cpp
 *   [7] 0x02124eb0  src/func_ov077_02124eb0.cpp
 *   [8] 0x021250a8  src/func_ov077_021250a8.cpp
 *   [9] 0x021251d0  src/func_ov077_021251d0.cpp
 *   [10] 0x02125290  src/func_ov077_02125290.c
 *   [11] 0x02125304  src/func_ov077_02125304.cpp
 *   [12] 0x021253a4  src/func_ov077_021253a4.cpp
 *   [13] 0x02125480  src/func_ov077_02125480.cpp
 *   [14] 0x02125550  src/func_ov077_02125550.cpp
 *   [15] 0x021256b4  src/func_ov077_021256b4.c
 *   [16] 0x02125830  src/func_ov077_02125830.c
 *   [17] 0x021258dc  src/func_ov077_021258dc.c
 *   [18] 0x02125908  src/func_ov077_02125908.c
 *   [19] 0x02125a0c  src/func_ov077_02125a0c.c
 *   [20] 0x02125a54  src/func_ov077_02125a54.c
 *   [21] 0x02125b1c  src/func_ov077_02125b1c.cpp
 *   [22] 0x02125bb4  src/func_ov077_02125bb4.c
 *   [23] 0x02125dd4  src/func_ov077_02125dd4.c
 *   [24] 0x02125e20  src/func_ov077_02125e20.cpp
 *   [25] 0x02125e5c  src/func_ov077_02125e5c.cpp
 *   [26] 0x02125e94  src/func_ov077_02125e94.c
 *   [27] 0x02125eb0  src/_ZN5Spiny16CleanupResourcesEv.c
 *   [28] 0x02125eec  src/_ZN5Spiny16OnPendingDestroyEv.c
 *   [29] 0x02125ef0  src/_ZN5Spiny6RenderEv.cpp
 *   [30] 0x02125f68  src/_ZN5Spiny8BehaviorEv.cpp
 *   [31] 0x02126058  src/_ZN5Spiny13InitResourcesEv.cpp
 *   [32] 0x02126194  src/_ZN5Spiny13OnTurnIntoEggER6Player.cpp
 *   [33] 0x021261f4  src/Spiny_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- Spiny_Spawn, 0x021261f4, size 0x58 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol Spiny_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV5Spiny */
int *Spiny_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1004);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV5Spiny[2]; /* +8: this TU defines the vtable */
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN9ModelAnimC1Ev((char *)p + 0x124);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN7dCcAc_cC1Ev((char *)p + 0x1b0);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1e4);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN5Spiny13OnTurnIntoEggER6Player, 0x02126194, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Spiny13OnTurnIntoEggER6Player
/* daTgz_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Gives the player 1 coin (as a cap-collection coin if Yoshi is wearing the cap,
 * otherwise as an egg coin), then marks this actor for destruction. */
#include "Spiny.h"
#include "Player.h"

int Spiny::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 1, 0);
    else
        player.RegisterEggCoinCount(1, 0, 0);
    MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN5Spiny13InitResourcesEv, 0x02126058, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Spiny13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct dActor_c;
struct Vector3_16;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" BCA_File *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned int c);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
namespace three_arg_e94 { extern "C" int func_ov077_02125e94(void *c, int a, int b); } /* InitResources's call byte-requires the third argument; the TU's file-scope view is (void*, int) */

extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127c14;
extern char IDENTITY_MATRIX4X3;

struct M48 { int w[12]; };

int Spiny::InitResources()
{
    BMD_File *bmd;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, bmd, 1, -1);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x124, bmd, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov077_02127c14);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0x124, *(BCA_File **)((char *)&data_ov077_02127c14 + 4), 0, 0x1000, 0);
    if (!_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel))
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x1b0, (dActor_c *)((char *)this), 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x1e4, (dActor_c *)((char *)this), 0x2d000, 0, (Vector3_16 *)((char *)&mPrevAngleX), (Vector3_16 *)((char *)&mAngleX));
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mDespawnTimer = 0x2c;
    three_arg_e94::func_ov077_02125e94(((char *)this), 0, 0x2c);
    *(M48 *)((char *)&mMatrix) = *(M48 *)&IDENTITY_MATRIX4X3;
    func_ov077_02125304(((char *)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN5Spiny8BehaviorEv, 0x02125f68, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Spiny8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
extern "C" {
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* c, int d);
unsigned char DecIfAbove0_Byte(unsigned char* p);
void _ZN7fBase_c18MarkForDestructionEv(void* c);
int func_ov077_02124c28(void* c);
void func_ov077_02125e20(void* c);
void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void* c, void* cyl);
void func_ov077_02125304(char* c);
void _ZN8dActor_c8PoofDustEv(void* c);
void func_02012694(int a, void* p);

extern signed char data_0209f2f8;
}

int Spiny::Behavior()
{
    int s = mState;
    if (s != 1 || _ZNK10dBgCh_Actr10IsOnGroundEv((char*)&mWithMeshClsn)) {
        s = mState;
        if (s != 4 && s != 5 && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000)) {
            if (DecIfAbove0_Byte((unsigned char*)((char*)&mDespawnTimer)) == 0) {
                _ZN7fBase_c18MarkForDestructionEv(((char*)this));
                return 1;
            }
            goto done;
        }
    }
    func_ov077_02124c28(((char*)this));
    func_ov077_02125e20(((char*)this));
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), ((char*)this) + 0x1b0);
    func_ov077_02125304(((char*)this));
    if (data_0209f2f8 == 0x1c && mPosY <= -0x1600000) {
        _ZN8dActor_c8PoofDustEv(((char*)this));
        func_02012694(0xc4, ((char*)this) + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(((char*)this));
    }
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN5Spiny6RenderEv, 0x02125ef0, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Spiny6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
struct Obj {
  virtual void m0(); virtual void m1(); virtual void m2();
  virtual void m3(); virtual void m4(); virtual void doit(int);
};

int Spiny::Render()
{
  if((mFlags & 0x40000) ? 1 : 0) return 1;
  int s=mState;
  if(s==0 || s==4) ((Obj*)((char *)&mModel))->doit(0);
  else ((Obj*)((char *)&mModelAnim))->doit(0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN5Spiny16OnPendingDestroyEv, 0x02125eec, size 0x4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void _ZN5Spiny16OnPendingDestroyEv(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN5Spiny16CleanupResourcesEv, 0x02125eb0, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
int _ZN5Spiny16CleanupResourcesEv(void)
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov077_02127b48);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov077_02127b38);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov077_02127c14);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov077_02125e94, 0x02125e94, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern char data_ov077_02127c28[];
extern void func_ov077_02125e5c(void* c);

void func_ov077_02125e94(void* vc, int i) {
    char* c = (char*)vc;
    *(char**)(c + 0x3d0) = data_ov077_02127c28 + (i << 4);
    func_ov077_02125e5c(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov077_02125e5c, 0x02125e5c, size 0x38 */
/* -------------------------------------------------------------------------- */
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3d0]; PMF *pp; };
extern "C" void func_ov077_02125e5c(void *vc) { C *c = (C *)vc; PMF *p = c->pp; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov077_02125e20, 0x02125e20, size 0x3c */
/* -------------------------------------------------------------------------- */
/* (struct C / PMF: defined once at ordinal 25 above) */
extern "C" void func_ov077_02125e20(void *vc) { C *c = (C *)vc; PMF *p = c->pp + 1; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov077_02125dd4, 0x02125dd4, size 0x4c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *);
int func_ov077_02125dd4(char *c)
{
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0x4000;
    *(int *)(c + 0xa8) = 0x19000;
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)c + 0x1e4);
    *(int *)(c + 0x3d8) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov077_02125bb4, 0x02125bb4, size 0x220 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov077_02125bb4
/* recovered: shared common types, declarations from a shared header */
#include "decl_Particle.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;



extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *p);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern void func_02012694(int id, void *pos);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int uniqueID, unsigned int effectID,
    Fix12i x, Fix12i y, Fix12i z,
    const void *dir, void *callback);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
extern void _ZN8dActor_c8PoofDustEv(void *c);
extern void _ZN7fBase_c18MarkForDestructionEv(void *c);
extern void func_ov077_02125e94(void *c, int a);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i x, Fix12i y, Fix12i z);
extern void func_0201267c(int id, void *pos);
extern void _ZN5dCc_c5ClearEv(void *p);
extern void _ZN5dCc_c6UpdateEv(void *p);

extern int data_0209f32c;

int func_ov077_02125bb4(char *c)
{
    int r4;
    int d;
    int x, y, z;
    struct Vector3 vec;

    *(short *)(c + 0x8c) = *(short *)(c + 0x8c) + 0x4e20;
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    func_ov077_021250a8(c);
    _ZN10dBgCh_Actr13SetLimMovFlagEv(c + 0x1e4);
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1e4);

    r4 = func_ov077_02124ce4(c);
    if (r4) {
        if (*(unsigned char *)(c + 0x3e4) == 0) {
            func_02012694(0xe2, c + 0x74);
            _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(
                *(int *)(c + 0x5c), data_0209f32c, *(int *)(c + 0x64));
            *(unsigned int *)(c + 0x3e0) =
                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(unsigned int *)(c + 0x3e0), 0x109,
                    *(int *)(c + 0x5c), data_0209f32c, *(int *)(c + 0x64),
                    0, 0);
        }
        *(int *)(c + 0x9c) = -0x400;
        *(int *)(c + 0xa0) = -0x5000;
        *(int *)(c + 0x98) = 0x2000;
    } else {
        *(int *)(c + 0x9c) = -0x2000;
        *(int *)(c + 0xa0) = -0x3c000;
        *(int *)(c + 0x98) = 0x4000;
    }
    *(unsigned char *)(c + 0x3e4) = (unsigned char)r4;

    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4)) {
        *(int *)(c + 0xa8) = (*(int *)(c + 0xa8) * -0x3c) / 100;
        d = *(int *)(c + 0x3dc) ? *(int *)(c + 0x60) - *(int *)(c + 0x3dc) : 0;
        if (d < -0xc8000) {
            _ZN8dActor_c8PoofDustEv(c);
            func_02012694(0x166, c + 0x74);
            _ZN7fBase_c18MarkForDestructionEv(c);
        } else if (*(int *)(c + 0xa8) < 0xa000) {
            *(int *)(c + 0xa8) = 0;
            *(short *)(c + 0x8c) = *(short *)(c + 0x92);
            *(short *)(c + 0x8e) = *(short *)(c + 0x94);
            *(short *)(c + 0x90) = *(short *)(c + 0x96);
            _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x1e4);
            *(unsigned int *)(c + 0xb0) |= 0x10000000u;
            func_ov077_02125e94(c, 1);
        } else {
            x = *(int *)(c + 0x5c);
            y = *(int *)(c + 0x60) + 0x28000;
            z = *(int *)(c + 0x64);
            ((int *)&vec)[0] = x;
            ((int *)&vec)[1] = y;
            ((int *)&vec)[2] = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, c + 0x74);
        }
    }

    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov077_02125b1c, 0x02125b1c, size 0x98 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* bca, int a, int f, unsigned int j);
extern void func_02035684(int* p, int v);
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
void func_ov077_02125b1c(char* c) {
  *(int*)(c + 0x9c) = -0x2000;
  *(int*)(c + 0xa0) = -0x3c000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x124, ((void**)&data_ov077_02127c14)[1], 0, 0x1000, 0);
  *(int*)(c + 0x180) = 0x1000;
  func_02035684((int*)(c + 0x1e4), 0x28000);
  *(int*)(c + 0x98) = 0x1b33;
  *(short*)(c + 0x3e6) = RandomIntInternal(&data_0209e650);
  *(unsigned char*)(c + 0x3e8) = RandomIntInternal(&data_0209e650);
  *(int*)(c + 0x3d8) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov077_02125a54, 0x02125a54, size 0xc8 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _Z14ApproachLinearRsss(short *a, short b, short c);
extern void _ZN9Animation7AdvanceEv(void *);
extern void func_ov077_02124eb0(void *c);
extern void _ZN8dActor_c8PoofDustEv(void *);
extern void func_02012694(int a, void *b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
extern void func_ov077_02124d08(void *c, void *p);
extern void _ZN5dCc_c5ClearEv(void *c);
extern void _ZN5dCc_c6UpdateEv(void *c);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov077_02125e94(void *c, int a);

int func_ov077_02125a54(char *c){
  int d;
  _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x3e6), 0x64);
  *(short*)(c + 0x94) = *(short*)(c + 0x8e);
  _ZN9Animation7AdvanceEv(c + 0x174);
  func_ov077_02124eb0(c);
  if(*(int*)(c + 0x3dc))
    d = *(int*)(c + 0x60) - *(int*)(c + 0x3dc);
  else
    d = 0;
  if(d < -0xc8000){
    _ZN8dActor_c8PoofDustEv(c);
    func_02012694(0x166, c + 0x74);
    _ZN7fBase_c18MarkForDestructionEv(c);
  }
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
  func_ov077_02124d08(c, c + 0x1e4);
  _ZN5dCc_c5ClearEv(c + 0x1b0);
  _ZN5dCc_c6UpdateEv(c + 0x1b0);
  if(DecIfAbove0_Byte((unsigned char*)(c + 0x3e8)) == 0)
    func_ov077_02125e94(c, 1);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov077_02125a0c, 0x02125a0c, size 0x48 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern short Vec3_HorzAngle(void*, void*);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *);
int func_ov077_02125a0c(char *c) {
    *(int*)(c+0x98) = 0x5000;
    *(int*)(c+0xa8) = 0xd000;
    short ang = (short)Vec3_HorzAngle((char*)*(int*)(c+0x3d4)+0x5c, c+0x5c);
    *(short*)(c+0x94) = ang;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(c+0x1e4);
    *(int*)(c+0x3d8) = 2;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov077_02125908, 0x02125908, size 0x104 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *p);
extern void _ZN8dActor_c8PoofDustEv(void *p);
extern void _ZN7fBase_c18MarkForDestructionEv(void *p);
extern void _ZN9Animation7AdvanceEv(void *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *p, void *cc);
extern void _ZN5dCc_c5ClearEv(void *p);
extern void _ZN5dCc_c6UpdateEv(void *p);
extern void func_02012694(int a, void *b);
extern void func_ov077_02125e94(void *p, int v);
extern void func_ov077_02124eb0(void *p);

int func_ov077_02125908(char *c)
{
    int v;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(c + 0x1e4);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4) != 0)
    {
        *(int *)(c + 0xa8) = *(int *)(c + 0xa8) * -50 / 100;
    }
    else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1e4) != 0)
    {
        *(int *)(c + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x1e4);
        *(int *)(c + 0x3d4) = 0;
        *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        func_ov077_02125e94(c, 1);
    }

    if (*(int *)(c + 0x3dc) != 0)
        v = *(int *)(c + 0x60) - *(int *)(c + 0x3dc);
    else
        v = 0;

    if (v < -0xc8000)
    {
        _ZN8dActor_c8PoofDustEv(c);
        func_02012694(0x166, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }

    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    func_ov077_02124eb0(c);
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov077_021258dc, 0x021258dc, size 0x2c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov077_021258dc
// recovered name: Lakitu_Kill
/* recovered: renamed to Class_Method */
/* daJgm_c::Kill - recovered from vtable slot identity */
extern void _ZN5dCc_c5ClearEv(void *);
int func_ov077_021258dc(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x1b0);
    *(int *)(c + 0x3d8) = 3;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov077_02125830, 0x02125830, size 0xac */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void func_ov077_02125e94(void *c, int v);

int func_ov077_02125830(char *c)
{
    if (((*(int *)(c + 0xb0) & 0x40000) ? 1 : 0) != 0)
    {
        char *p = *(char **)(c + 0xd0);
        int *src = (int *)(p + 0x5c);
        *(int *)(c + 0x5c) = src[0];
        *(int *)(c + 0x60) = src[1];
        *(int *)(c + 0x64) = src[2];
    }
    {
        int flags = *(int *)(c + 0xb0);
        if (((flags & 0x80000) ? 1 : 0) != 0)
        {
            func_ov077_02125e94(c, 4);
        }
        else if (((flags & 0x20000) ? 1 : 0) == 0 && ((flags & 0x40000) ? 1 : 0) == 0)
        {
            *(int *)(c + 0xd0) = 0;
            func_ov077_02125e94(c, 1);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov077_021256b4, 0x021256b4, size 0x17c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
#include "types.h"
// @symbol func_ov077_021256b4
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: shared common types */
#include "common.h"
extern s16 data_02082214[];
extern int _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void *self, struct Vector3 *a, struct Vector3 *out, int doStore);
#define LA(p) ((int)(p))
int func_ov077_021256b4(char *o)
{
    char *d0;
    int *a5c;
    int *a60;
    int *a64;
    int *src;
    int k;
    s16 s;
    s16 cval;
    s16 ang;
    struct Vector3 v;
    int one;
    int y;
    int z;
    int x;

    *(int *)LA(o + 0xb0) &= ~0x80000;
    *(int *)(o + 0x98) = 0xa000;
    *(int *)(o + 0xa8) = 0;

    d0 = *(char **)(o + 0xd0);
    a5c = (int *)LA(o + 0x5c);
    ang = *(s16 *)(d0 + 0x8e);
    *(s16 *)(o + 0x8c) = 0;
    *(s16 *)(o + 0x8e) = ang;
    *(s16 *)(o + 0x90) = 0;

    *(s16 *)(o + 0x94) = *(s16 *)(o + 0x8e);
    a60 = (int *)LA(o + 0x60);
    a64 = (int *)LA(o + 0x64);

    d0 = *(char **)(o + 0xd0);
    src = (int *)LA(d0 + 0x5c);
    one = 1;
    *(int *)(o + 0x5c) = src[0];
    *(int *)(o + 0x60) = src[1];
    *(int *)(o + 0x64) = src[2];

    k = ((int)*(u16 *)(o + 0x8e)) >> 4;
    s = data_02082214[k * 2];
    *a5c = *a5c + (int)(((s64)s * 0x3c000 + 0x800) >> 12);
    *a60 = *a60 + 0x85000;
    k = ((int)*(u16 *)(o + 0x8e)) >> 4;
    cval = data_02082214[k * 2 + 1];
    *a64 = *a64 + (int)(((s64)cval * 0x3c000 + 0x800) >> 12);

    d0 = *(char **)(o + 0xd0);

    {
        int ty = *(int *)(d0 + 0x60);
        int tz = *(int *)(d0 + 0x64);
        int ty2 = ty + 0x50000;
        int tx = *(int *)(d0 + 0x5c);
        v.x = tx; v.y = ty2; v.z = tz;
    }

    _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(o, &v, (struct Vector3 *)(o + 0x5c), one);

    *(int *)(o + 0xd0) = 0;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(o + 0x1e4);

    *(int *)(o + 0x3d8) = 4;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov077_02125550, 0x02125550, size 0x164 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02125550
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void* p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern void func_0201267c(int a, void* p);
extern void func_ov077_02125e94(void* c, int a);
extern void _ZN8dActor_c8PoofDustEv(void* c);
extern void func_02012694(int a, void* p);
extern void _ZN7fBase_c18MarkForDestructionEv(void* c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* c, void* p);
extern void _ZN5dCc_c5ClearEv(void* p);
extern void _ZN5dCc_c6UpdateEv(void* p);
}

extern "C" int func_ov077_02125550(char* c)
{
    Vector3 vec;
    int x, y, z;
    int d;

    *(short*)(c + 0x8c) = *(short*)(c + 0x8c) + 0x4e20;

    if (*(int*)(c + 0x98) >= *(int*)(c + 0x1fc) || *(int*)(c + 0xa8) >= *(int*)(c + 0x1fc)) {
        dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1e4);
    } else {
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(c + 0x1e4);
    }

    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4)) {
        *(int*)(c + 0xa8) = *(int*)(c + 0xa8) * -0x3c / 100;
        if (*(int*)(c + 0xa8) > 0x8000) {
            x = *(int*)(c + 0x5c);
            y = *(int*)(c + 0x60) + 0x28000;
            z = *(int*)(c + 0x64);
            ((int*)&vec)[0] = x;
            ((int*)&vec)[1] = y;
            ((int*)&vec)[2] = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, c + 0x74);
        } else {
            *(int*)(c + 0xa8) = 0;
            *(short*)(c + 0x8c) = 0;
            _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x1e4);
            func_ov077_02125e94(c, 1);
        }
    }

    d = *(int*)(c + 0x3dc) ? *(int*)(c + 0x60) - *(int*)(c + 0x3dc) : 0;
    if (d < -0xc8000) {
        _ZN8dActor_c8PoofDustEv(c);
        func_02012694(0x166, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    func_ov077_02124eb0(c);
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov077_02125480, 0x02125480, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02125480
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
}

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual int m();   // slot 0x74/4 = 29
};

extern "C" int func_ov077_02125480(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(short*)(c + 0x8c) = 0;
    *(short*)(c + 0x90) = 0;
    *(unsigned char*)(c + 0x3e8) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x124, *(void**)((char*)&data_ov077_02127c14 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x180) = 0x4000;
    Base* b = (Base*)c;
    int r1 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r1, *(int*)(c + 0x64));
    int r2 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r2, *(int*)(c + 0x64));
    *(int*)(c + 0x3d8) = 5;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov077_021253a4, 0x021253a4, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_021253a4
/* recovered: shared common types */
#include "common.h"

extern "C" {
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void*);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void*);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void*);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void*, const Vector3*, unsigned int, int, short);
extern void _ZN8dActor_c8PoofDustEv(void*);
extern void func_02012694(int a, void* b);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);

int func_ov077_021253a4(char* c)
{
    *(short*)(c + 0x8c) = *(short*)(c + 0x8c) - 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);

    if (*(int*)(c + 0x98) >= *(int*)(c + 0x1fc) || *(int*)(c + 0xa8) >= *(int*)(c + 0x1fc)) {
        dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1e4);
    } else {
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(c + 0x1e4);
    }

    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4) || DecIfAbove0_Byte((unsigned char*)(c + 0x3e8)) == 0) {
        Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, 1, 0x2000, 0);
        _ZN8dActor_c8PoofDustEv(c);
        func_02012694(0xc4, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov077_02125304, 0x02125304, size 0xa0 */
/* -------------------------------------------------------------------------- */
extern "C" {
void func_ov077_021251d0(void *t);
void func_ov077_02125290(char *t);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thisp, void *sm, void *mtx, int rad, int t, unsigned int j);
void func_ov077_02125304(char *c) {
    int b = (int)((*(int*)(c+0xb0) & 0x40000) != 0);
    if (b != 0) return;
    if (*(int*)(c+0x3d8) == 5) func_ov077_021251d0(c);
    else func_ov077_02125290(c);
    *(int*)(c+0x3c4) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x3c8) = *(int*)(c+0x60) >> 3;
    *(int*)(c+0x3cc) = *(int*)(c+0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x188, c+0x3a0, 0x50000, 0x320000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov077_02125290, 0x02125290, size 0x74 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
void func_ov077_02125290(char *t)
{
    int v = *(int *)(t + 0x3d8);
    int b = 1;
    char *m;
    if (v != 0) b = (v == 4);
    if (b) m = t + 0xf0;
    else m = t + 0x140;
    Matrix4x3_FromRotationZXYExt(m, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(m + 0x24) = *(int *)(t + 0x5c) >> 3;
    *(int *)(m + 0x28) = *(int *)(t + 0x60) >> 3;
    *(int *)(m + 0x2c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov077_021251d0, 0x021251d0, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_021251d0
/* recovered: shared common types */
#include "common.h"
extern "C" {
void Vec3_Asr(void* d, void* s, int sh);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);

extern Matrix4x3 data_020a0e68;
}
/* (Base: the identical 30-slot shadow defined once above, at ordinal 12's block) */
extern "C" void func_ov077_021251d0(void* c)
{
  char* r4 = (char*)c;
  int v[3];
  Vec3_Asr(v, r4+0x5c, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
  Base* b = (Base*)c;
  int r = b->m();
  Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, r >> 3, 0);
  Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
      *(short*)(r4+0x8c), *(short*)(r4+0x8e), *(short*)(r4+0x90));
  int r2 = b->m();
  Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-r2) >> 3, 0);
  *(Matrix4x3*)(r4+0x140) = data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov077_021250a8, 0x021250a8, size 0x128 */
/* -------------------------------------------------------------------------- */
extern "C" {
typedef struct dActor_c dActor_c;
typedef struct Player Player;
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void func_ov077_02125e94(void* c, int a);
short Vec3_HorzAngle(void* a, void* b);
void _ZN6Player16IncMegaKillCountEv(void* p);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, unsigned int a, int b, unsigned int d, unsigned int e, unsigned int f);

void func_ov077_021250a8(void* vc){
  char* c = (char*)vc;
  unsigned int id = *(unsigned int*)(c+0x1d4);
  if (id == 0) return;
  char* r4 = (char*)_ZN8dActor_c10FindWithIDEj(id);
  if (r4 == 0) return;
  int b1 = (int)(*(unsigned short*)(r4+0xc) == 0xbf);
  if (b1 == 0) return;
  int b2 = (int)((*(int*)(c+0xb0) & 0x20000) != 0);
  if (b2 != 0) {
    func_ov077_02125e94(c, 3);
    return;
  }
  int flags = *(int*)(c+0x1d0);
  if ((flags & 0x10) != 0) {
    *(short*)(c+0x94) = Vec3_HorzAngle((Vector3*)(r4+0x5c), (Vector3*)(c+0x5c));
    *(short*)(c+0x8e) = *(short*)(c+0x94) + 0x8000;
    _ZN6Player16IncMegaKillCountEv((Player*)r4);
    func_ov077_02125e94(c, 5);
    return;
  }
  if ((flags & 0x40000) != 0) return;
  Vector3 v;
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player*)r4, &v, 2, 0xc000, 1, 0, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov077_02124eb0, 0x02124eb0, size 0x1f8 */
/* -------------------------------------------------------------------------- */
extern "C" int _ZN8dActor_c7FindEggER5dCc_c(void *self, void *clsn); /* decl_Actor.h view */
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const Vector3 *pos, unsigned int a, int fix, short b);
extern "C" void _ZN8dActor_c8PoofDustEv(void *self);
extern "C" void func_02012694(int a, void *pos);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern "C" void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" void func_ov077_02125e94(void *c, int i);
extern "C" int _ZN6Player9IsOnShellEv(void *p);
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const Vector3 &pos);
extern "C" short Vec3_HorzAngle(void *a, void *b);
extern "C" void _ZN6Player16IncMegaKillCountEv(void *p);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const Vector3 *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);

extern "C" void func_ov077_02124eb0(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x1b0) != 0) {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, (const Vector3 *)v, 1, 0x2000, 0);
        _ZN8dActor_c8PoofDustEv(c);
        func_02012694(0xc4, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x1d4);
        if (id == 0)
            return;
        r4 = (unsigned char *)_ZN8dActor_c10FindWithIDEj(id);
    }
    if (r4 == 0)
        return;

    b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf);
    if (b == 0)
        return;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0) {
        func_ov077_02125e94(c, 3);
        return;
    }

    if ((*(int *)(c + 0x1d0) & 0x403c0)
        || _ZN6Player9IsOnShellEv(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        if (*(int *)(c + 0x3d8) != 1)
            return;
        _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, *(const Vector3 *)(c + 0x74));
        *(unsigned char **)(c + 0x3d4) = r4;
        func_ov077_02125e94(c, 2);
        return;
    }

    if (*(int *)(c + 0x1d0) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        _ZN6Player16IncMegaKillCountEv(r4);
        func_ov077_02125e94(c, 5);
        return;
    }

    if (*(int *)(c + 0x3d8) == 4)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, (const Vector3 *)v, 2, 0xc000, 1, 0, 1);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov077_02124d08, 0x02124d08, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02124d08
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;

struct dBgCh_Actr;
struct dActor_c;
struct dBgCh_Gnd { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };
struct dBgPi;
struct SurfaceInfo;

extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* p);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
extern "C" void _ZN5dBgCh19StartDetectingToxicEv(void* self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, const Vector3& v, void* actor);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
extern "C" void _ZN8dActor_c8PoofDustEv(void* self);
extern "C" void func_02012694(int a, void* b);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void* self);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);
extern "C" void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* self);
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vector3* out);
extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" s16 func_02010844(void* unused, Vector3* v, s16 angle);
extern "C" int _ZNK10dBgCh_Actr8IsOnWallEv(void* self);

extern "C" void func_ov077_02124d08(void* va, void* vw) {
    char* a = (char*)va; char* w = (char*)vw;
    dBgCh_Gnd rc;
    Vector3 pos;
    Vector3 normal;
    Vector3 wallnormal;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(w)) {
        _ZN9dBgCh_GndC1Ev(&rc);
        {
            int p60 = *(int*)(a+0x60);
            int pz = *(int*)(a+0x64);
            int py = p60 + 0xc8000;
            pos.x = *(int*)(a+0x5c);
            pos.y = py;
            pos.z = pz;
        }
        _ZN5dBgCh19StartDetectingToxicEv(&rc);
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rc, pos, a);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rc)) {
            if (func_02037e20(rc.floor) != 0 && *(int*)(a+0x60) < rc.floor[(0x44-0x14)/4]) {
                _ZN8dActor_c8PoofDustEv(a);
                func_02012694(0xc4, a+0x74);
                _ZN7fBase_c18MarkForDestructionEv(a);
                _ZN9dBgCh_GndD1Ev(&rc);
                return;
            }
            {
                void* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(w);
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr + 4, &normal);
            }
            if (normal.y != 0) {
                *(int*)(a+0xa8) = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * *(int*)(a+0xa4) + 0x800) >> 12)
                  + (int)(((long long)normal.z * *(int*)(a+0xac) + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
            *(s16*)(a+0x8c) = func_02010844(a, &normal, *(s16*)(a+0x8e));
            *(s16*)(a+0x90) = func_02010844(a, &normal, (s16)(*(s16*)(a+0x8e) - 0x4000));
        }
        _ZN9dBgCh_GndD1Ev(&rc);
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(w)) {
        void* wr = _ZNK10dBgCh_Actr13GetWallResultEv(w);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)wr + 4, &wallnormal);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov077_02124ce4, 0x02124ce4, size 0x24 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov077_02124ce4(void* vc){
  char* c = (char*)vc;
  int r1=*(int*)(c+0x3dc);
  if(r1==0) return 0;
  return r1 > *(int*)(c+0x60);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov077_02124c28, 0x02124c28, size 0xbc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02124c28
/* recovered: shared common types */
#include "common.h"

struct dActor_c;

namespace rg54 {  /* this member's 0x54-byte dBgCh_Gnd view is stack-frame-load-bearing;
                     ordinal 6's block uses a 0x50-byte view of the same name -- the namespace
                     keeps both, binding the same C symbols */
struct dBgCh_Gnd {
    char pad0[0x14];
    int m14[12];
    int m44;
    char pad48[0xc];
};
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd*);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd*, const Vector3&, dActor_c*);
extern "C" void _ZN5dBgCh19StartDetectingWaterEv(void*);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd*);
extern "C" int SurfaceInfo_TestFlag0x20(int* p);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd*);
extern "C" void _ZN5dBgCh18StopDetectingWaterEv(void*);
}

extern "C" int func_ov077_02124c28(void* vc)
{
    char* c = (char*)vc;
    if (*(int*)(c + 0x3dc) == 0) {
        rg54::dBgCh_Gnd rg;
        Vector3 pos;
        int r;
        rg54::_ZN9dBgCh_GndC1Ev(&rg);
        {
            int y = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int y2 = y + 0xc8000;
            int x = *(int*)(c + 0x5c);
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg54::_ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, pos, (dActor_c*)c);
        rg54::_ZN5dBgCh19StartDetectingWaterEv(&rg);
        if (rg54::_ZN9dBgCh_Gnd10DetectClsnEv(&rg) == 0) goto fail;
        r = rg54::SurfaceInfo_TestFlag0x20(rg.m14);
        if (r != 0) {
            *(int*)(c + 0x3dc) = rg.m44;
        } else {
        fail:
            rg54::_ZN9dBgCh_GndD1Ev(&rg);
            return 0;
        }
        rg54::_ZN5dBgCh18StopDetectingWaterEv(&rg);
        rg54::_ZN9dBgCh_GndD1Ev(&rg);
    }
    return *(int*)(c + 0x60) - *(int*)(c + 0x3dc);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN5Spiny16OnAimedAtWithEggEv, 0x02124c20, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Spiny16OnAimedAtWithEggEv
/* daTgz_c::OnAimedAtWithEgg -- vtable slot 29, recovered from vtable slot identity.
 * The ROM body ignores `this` and returns a constant. */
#include "Spiny.h"

int Spiny::OnAimedAtWithEgg()
{
    return 122880;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN5Spiny13OnYoshiTryEatEv, 0x02124c18, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5Spiny13OnYoshiTryEatEv
/* daTgz_c::OnYoshiTryEat -- vtable slot 18, recovered from vtable slot identity.
 * The ROM body ignores `this` and returns a constant. */
#include "Spiny.h"

int Spiny::OnYoshiTryEat()
{
    return 6;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN5SpinyD0Ev, 0x02124bb4, size 0x64 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol _ZN5SpinyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daTgz_c */
/* (no separate definition: the single ~Spiny() below emits the D0 and D1
 * variants together -- keeping this hand-mangled body alongside the real
 * destructor is the known mwccarm ICE (ELFgen.c:483). The legacy file
 * stored _ZTV7daTgz_c, the RTTI spelling of the address _ZTV5Spiny names.) */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN5SpinyD1Ev, 0x02124b64, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN5SpinyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then ~dActor_c. */
#include "Spiny.h"

