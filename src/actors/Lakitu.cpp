//cpp
/* HAND-ASSEMBLED translation unit -- ov077/Lakitu (32 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02123740  src/_ZN6LakituD1Ev.cpp
 *   [1] 0x02123798  src/_ZN6LakituD0Ev.cpp
 *   [2] 0x02123804  src/_ZN6Lakitu13OnYoshiTryEatEv.cpp
 *   [3] 0x0212380c  src/_ZN6Lakitu16OnAimedAtWithEggEv.cpp
 *   [4] 0x02123814  src/func_ov077_02123814.c
 *   [5] 0x02123880  src/func_ov077_02123880.c
 *   [6] 0x021238bc  src/func_ov077_021238bc.cpp
 *   [7] 0x0212390c  src/func_ov077_0212390c.cpp
 *   [8] 0x02123a1c  src/func_ov077_02123a1c.cpp
 *   [9] 0x02123a74  src/func_ov077_02123a74.cpp
 *   [10] 0x02123c6c  src/func_ov077_02123c6c.cpp
 *   [11] 0x02123d40  src/func_ov077_02123d40.cpp
 *   [12] 0x02123fcc  src/func_ov077_02123fcc.c
 *   [13] 0x02124038  src/func_ov077_02124038.cpp
 *   [14] 0x02124118  src/func_ov077_02124118.cpp
 *   [15] 0x021241ac  src/func_ov077_021241ac.c
 *   [16] 0x021242f0  src/func_ov077_021242f0.c
 *   [17] 0x02124394  src/func_ov077_02124394.c
 *   [18] 0x021243c0  src/func_ov077_021243c0.cpp
 *   [19] 0x021244d4  src/func_ov077_021244d4.cpp
 *   [20] 0x02124564  src/func_ov077_02124564.c
 *   [21] 0x02124698  src/func_ov077_02124698.c
 *   [22] 0x02124718  src/func_ov077_02124718.cpp
 *   [23] 0x02124754  src/func_ov077_02124754.cpp
 *   [24] 0x0212478c  src/func_ov077_0212478c.c
 *   [25] 0x021247a8  src/_ZN6Lakitu16CleanupResourcesEv.cpp
 *   [26] 0x02124824  src/_ZN6Lakitu16OnPendingDestroyEv.cpp
 *   [27] 0x02124828  src/_ZN6Lakitu6RenderEv.cpp
 *   [28] 0x021248b8  src/_ZN6Lakitu8BehaviorEv.cpp
 *   [29] 0x02124908  src/_ZN6Lakitu13InitResourcesEv.cpp
 *   [30] 0x02124aa4  src/_ZN6Lakitu13OnTurnIntoEggER6Player.cpp
 *   [31] 0x02124b04  src/Lakitu_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- Lakitu_Spawn, 0x02124b04, size 0x60 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol Lakitu_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_TextureSequence.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6Lakitu */
int *Lakitu_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1056);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV6Lakitu[2]; /* +8: this TU defines the vtable */
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0x138);
        _ZN11ShadowModelC1Ev((char *)p + 0x188);
        _ZN15TextureSequenceC1Ev((char *)p + 0x1b0);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x1c4);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x204);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN6Lakitu13OnTurnIntoEggER6Player, 0x02124aa4, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu13OnTurnIntoEggER6Player
/* daJgm_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Gives the player 5 coins (as cap-collection coins if Yoshi is wearing the cap,
 * otherwise as egg coins), then kills this actor and tracks it in the death table. */
#include "Lakitu.h"
#include "Player.h"

int Lakitu::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 5, 0);
    else
        player.RegisterEggCoinCount(5, 0, 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN6Lakitu13InitResourcesEv, 0x02124908, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
#include "TextureSequence.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, void *v, int b, int c, unsigned int d, unsigned int e);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, int b, int c, void *d, void *e);
extern void func_ov077_02123d40(void *c);
extern int data_ov077_02127b38[];
extern int data_ov077_02127b48[];
extern int data_ov077_02127b88[];
struct M48 { int w[12]; };
extern M48 IDENTITY_MATRIX4X3;
}

int Lakitu::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b50), 1, 1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x138, _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b48), 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov077_02127238[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void *)data_ov077_02127230[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        TextureSequence::Prepare(*(BMD_File *)data_ov077_02127b50[1], *(BTP_File *)((int *)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char *)this) + 0x1c4, ((char *)this), data_ov077_02127b88, 0x41000, 0x78000, 0x200002, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x204, ((char *)this), 0x2d000, 0x2d000, 0, 0);

    *(int *)(((int)((char *)this + 0x9c)) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    *(int *)(((int)((char *)this + 0xa0)) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(int *)(((int)((char *)this + 0x3f8)) & 0xFFFFFFFFFFFFFFFFLL) = *(int *)(((int)((char *)this + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(((int)((char *)this + 0x3fc)) & 0xFFFFFFFFFFFFFFFFLL) = *(int *)(((int)((char *)this + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    mSpawnPosZ = *(int *)(((int)((char *)this + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    unk_410 = 0;

    func_ov077_0212478c(((char *)this));
    *(M48 *)((char *)&mMatrix) = IDENTITY_MATRIX4X3;
    func_ov077_02123d40(((char *)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN6Lakitu8BehaviorEv, 0x021248b8, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
extern "C" {
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void*, int);
extern void func_ov077_02123d40(void*);
}

int Lakitu::Behavior()
{
  int v = param1;
  if(_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), v ? 0x1068000 : 0x7d0000)) return 1;
  func_ov077_02124718(((char *)this));
  func_ov077_02123d40(((char *)this));
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN6Lakitu6RenderEv, 0x02124828, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
struct ModelComponents;

int Lakitu::Render()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0)
        return 1;
    mTextureSequence.Update(*(ModelComponents *)((char *)this + 0xdc));
    mModelAnim.Render(0);
    if (mState == 1) {
        unsigned int v = ((unsigned int)(*(s32 *)((char *)this + 0x12c) << 4)) >> 0x10;
        if (v >= 0x19 && v <= 0x3a)
            mModel.Render(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN6Lakitu16OnPendingDestroyEv, 0x02124824, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu16OnPendingDestroyEv

#include "Lakitu.h"

void Lakitu::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN6Lakitu16CleanupResourcesEv, 0x021247a8, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu16CleanupResourcesEv

#include "Lakitu.h"
#include "SharedFilePtr.h"

int Lakitu::CleanupResources()
{
    ((SharedFilePtr *)data_ov077_02127b50)->Release();
    ((SharedFilePtr *)data_ov077_02127b48)->Release();
    ((SharedFilePtr *)data_ov077_02127b38)->Release();

    for (int i = 0; i < 2; i++)
        ((SharedFilePtr *)data_ov077_02127238[i])->Release();

    for (int i = 0; i < 2; i++)
        ((SharedFilePtr *)data_ov077_02127230[i])->Release();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov077_0212478c, 0x0212478c, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern char data_ov077_02127bc4[];
extern void func_ov077_02124754(void* c);

namespace two_arg_478c { extern "C" void func_ov077_0212478c(void* vc, int i) {
    char* c = (char*)vc;
    *(char**)(c + 0x3f0) = data_ov077_02127bc4 + (i << 4);
    func_ov077_02124754(c);
} }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov077_02124754, 0x02124754, size 0x38 */
/* -------------------------------------------------------------------------- */
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3f0]; PMF *pp; };
extern "C" void func_ov077_02124754(void *vc) { C *c = (C *)vc; PMF *p = c->pp; (c->**p)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov077_02124718, 0x02124718, size 0x3c */
/* -------------------------------------------------------------------------- */
/* (struct C / PMF: defined once at ordinal 23 above) */
extern "C" int func_ov077_02124718(void *vc) { C *c = (C *)vc; PMF *p = c->pp + 1; (c->**p)(); } /* int per decl_common; no value returned, matching the ROM */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov077_02124698, 0x02124698, size 0x80 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov077_02127b28[];
extern int data_ov077_02127b20[];
int func_ov077_02124698(char* c){
  *(int*)(c+0x98)=0xc000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, (void*)data_ov077_02127b28[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x1b0, (void*)data_ov077_02127b20[1], 0, 0x1000, 0);
  *(int*)(c+0x1bc)=0x1000;
  *(unsigned char*)(c+0x41c)=0x96;
  *(int*)(c+0x3f4)=0;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov077_02124564, 0x02124564, size 0x134 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov077_02124564
/* recovered: shared common types */
#include "common.h"
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
void* _ZN8dActor_c22ClosestNonVanishPlayerEv(void* p);
int Vec3_HorzDist(void* a, void* b);
int func_ov077_02123880(void* c);
namespace two_arg_478c { extern "C" void func_ov077_0212478c(void* c, int i); } /* decl_common carries a 1-arg view of this name that Lakitu::InitResources's call byte-requires; the namespace keeps this block's true 2-arg view out of its way (both bind the same C symbol) */
void func_ov077_0212390c(char* c);
void func_ov077_02123814(char* c);
void _ZN9Animation7AdvanceEv(void* p);
void func_ov077_02123a74(void* thiz);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* thiz, const struct Vector3* v);
void _ZN5dCc_c5ClearEv(void* p);
void _ZN5dCc_c6UpdateEv(void* p);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, const struct Vector3* pos, unsigned int e);

extern int data_ov077_02127b88[3];

int func_ov077_02124564(char* c) {
    *(int*)(c + 0x98) = 0xc000;
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x41c)) == 0) {
        char* p = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
        if (p == 0) goto store2;
        if (*(int*)(p + 0x60) >= *(int*)(c + 0x60)) goto store1;
        if (Vec3_HorzDist((struct Vector3*)(c + 0x5c), (struct Vector3*)(p + 0x5c)) >= 0x190000) goto store1;
        if (func_ov077_02123880(c) >= 4) goto store1;
        two_arg_478c::func_ov077_0212478c(c, 1);
        goto tail;
    store1:
        *(unsigned char*)(c + 0x41c) = 0x96;
        goto tail;
    store2:
        *(unsigned char*)(c + 0x41c) = 0x96;
    }
tail:
    func_ov077_0212390c(c);
    func_ov077_02123814(c);
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN9Animation7AdvanceEv(c + 0x1b0);
    func_ov077_02123a74(c);
    {
        struct Vector3 pos;
        pos.y = data_ov077_02127b88[1] + *(int*)(c + 0x414);
        pos.x = data_ov077_02127b88[0];
        pos.z = data_ov077_02127b88[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x1c4, &pos);
    }
    _ZN5dCc_c5ClearEv(c + 0x1c4);
    _ZN5dCc_c6UpdateEv(c + 0x1c4);
    *(int*)(c + 0x410) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(int*)(c + 0x410), 3, 0x182, (struct Vector3*)(c + 0x74), 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov077_021244d4, 0x021244d4, size 0x90 */
/* -------------------------------------------------------------------------- */
struct BCA_File;
struct BTP_File;
/* (ModelAnim/Animation/TextureSequence: real header types in scope. The calls
   below stay on the hand-mangled spellings with int in place of the by-value
   Fix12<int>, which mwccarm passes differently -- see notes/mwccarm-codegen.md
   6az; the real member declarations would break the byte match.) */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);


extern void *data_ov077_02127b40[];
extern void *data_ov077_02127b30[];

extern "C" void func_ov077_021244d4(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0xd4), (BCA_File *)data_ov077_02127b40[1], 0, 0x1000, 0);
    ((Animation *)(c + 0x124))->SetFlags(0x40000000);
    *(int *)(c + 0x130) = 0x1000;
    *(int *)(c + 0x12c) = 0;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((TextureSequence *)(c + 0x1b0), (void*)data_ov077_02127b30[1], 0, 0x1000, 0);
    ((Animation *)(c + 0x1b0))->SetFlags(0x40000000);
    *(int *)(c + 0x1bc) = 0x1000;
    *(int *)(c + 0x1b8) = 0;
    *(int *)(c + 0x3f4) = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov077_021243c0, 0x021243c0, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_021243c0
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;

extern "C" {
int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, struct Vector3*, void*, int, int);
void func_0201267c(int a, void* p);
int _ZN9Animation8FinishedEv(void* self);
namespace two_arg_478c { extern "C" void func_ov077_0212478c(void* c, int i); }
void func_ov077_0212390c(char* c);
void func_ov077_02123814(char* c);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov077_02123a74(void* c);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* self, const struct Vector3* v);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, const struct Vector3* v, unsigned int d);
extern int data_ov077_02127b88[];

int func_ov077_021243c0(char* c){
    if ((((unsigned int)*(int*)(c + 0x12c) << 4) >> 16) == 0x3a) {
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x104, 0, (struct Vector3*)(c + 0x404), (void*)(c + 0x8c),
            *(signed char*)(c + 0xcc), -1);
        func_0201267c(0xd2, c + 0x74);
    }
    if (_ZN9Animation8FinishedEv(c + 0x124)) {
        two_arg_478c::func_ov077_0212478c(c, 0);
    }
    func_ov077_0212390c(c);
    func_ov077_02123814(c);
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN9Animation7AdvanceEv(c + 0x1b0);
    func_ov077_02123a74(c);
    {
        int* d = data_ov077_02127b88;
        int off = *(int*)(c + 0x414);
        int z = d[2];
        int y = d[1] + off;
        int x = d[0];
        struct Vector3 v;
        v.x = x; v.y = y; v.z = z;
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x1c4, &v);
    }
    _ZN5dCc_c5ClearEv(c + 0x1c4);
    _ZN5dCc_c6UpdateEv(c + 0x1c4);
    *(int*)(c + 0x410) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int*)(c + 0x410), 3, 0x182, (const struct Vector3*)(c + 0x74), 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov077_02124394, 0x02124394, size 0x2c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN5dCc_c5ClearEv(void *);
int func_ov077_02124394(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x1c4);
    *(int *)(c + 0x3f4) = 2;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov077_021242f0, 0x021242f0, size 0xa4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace two_arg_478c { extern "C" void func_ov077_0212478c(void* c, int i); } /* decl_common carries a 1-arg view of this name that Lakitu::InitResources's call byte-requires; the namespace keeps this block's true 2-arg view out of its way (both bind the same C symbol) */

int func_ov077_021242f0(char *c)
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
            two_arg_478c::func_ov077_0212478c(c, 3);
        }
        else if (((flags & 0x20000) ? 1 : 0) == 0 && ((flags & 0x40000) ? 1 : 0) == 0)
        {
            two_arg_478c::func_ov077_0212478c(c, 0);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov077_021241ac, 0x021241ac, size 0x144 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
#include "types.h"
extern s16 data_02082214[];
#define LA(p) ((int)(p))

int func_ov077_021241ac(char *o)
{

    char *d0; int *a5c; int *a60; int *a64; int *src;
    char *p400; int three; int k; s16 s; s16 cval; int v7000; int v1e;

    *(int *)LA(o + 0xb0) &= ~0x80000;

    d0 = *(char **)(o + 0xd0);
    a5c = (int *)LA(o + 0x5c);
    *(int *)(o + 0x98) = *(int *)(d0 + 0x98) + 0xa000;

    d0 = *(char **)(o + 0xd0);
    a60 = (int *)LA(o + 0x60);
    a64 = (int *)LA(o + 0x64);
    *(s16 *)(o + 0x94) = *(s16 *)(d0 + 0x8e);

    d0 = *(char **)(o + 0xd0);

    v7000 = 0x7000;
    src = (int *)LA(d0 + 0x5c);
    *(int *)(o + 0x5c) = src[0];
    p400 = o + 0x400;
    v1e = 0x1e;
    *(int *)(o + 0x60) = src[1];
    *(int *)(o + 0x64) = src[2];
    three = 3;


    k = ((int)*(u16 *)(o + 0x94)) >> 4;
    s = data_02082214[k * 2];
    *a5c = *a5c + (int)(((s64)s * 0x50000 + 0x800) >> 12);
    *a60 = *a60 + 0x50000;
    k = ((int)*(u16 *)(o + 0x94)) >> 4;
    cval = data_02082214[k * 2 + 1];
    *a64 = *a64 + (int)(((s64)cval * 0x50000 + 0x800) >> 12);

    *(s16 *)(p400 + 0x1a) = (s16)v7000;
    *(u8 *)(o + 0x41c) = (u8)v1e;
    *(int *)(o + 0xd0) = 0;
    *(int *)(o + 0x3f4) = three;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov077_02124118, 0x02124118, size 0x94 */
/* -------------------------------------------------------------------------- */
/* (dCc_c/dActor_c: real header types in scope) */

void ApproachLinear2(short &v, short t, short step);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" void func_ov077_02123c6c(char *c, void *p);
extern "C" void func_ov077_02123a74(void *c);
namespace two_arg_478c { extern "C" void func_ov077_0212478c(void* c, int i); } /* decl_common carries a 1-arg view of this name that Lakitu::InitResources's call byte-requires; the namespace keeps this block's true 2-arg view out of its way (both bind the same C symbol) */

extern "C" int func_ov077_02124118(char *c)
{
    *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x8e) + *(short *)(c + 0x41a));
    ApproachLinear2(*(short *)(c + 0x41a), 0, 0x300);
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x1c4));
    func_ov077_02123c6c(c, (void *)(c + 0x204));
    func_ov077_02123a74(c);
    ((dCc_c *)(c + 0x1c4))->Clear();
    ((dCc_c *)(c + 0x1c4))->Update();
    if (!DecIfAbove0_Byte((unsigned char *)(c + 0x41c)))
        two_arg_478c::func_ov077_0212478c(c, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov077_02124038, 0x02124038, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02124038
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int n, const Vector3& v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
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

extern "C" int func_ov077_02124038(char* c)
{
    _ZN5Sound9PlayBank0EjRK7Vector3(9, *(Vector3*)(c + 0x74));
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x98) = 0xa000;
    *(int*)(c + 0xa8) = 0x28000;
    *(unsigned char*)(c + 0x41c) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, *(void**)((char*)data_ov077_02127b28 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x4000;
    Base* b = (Base*)c;
    int r1 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r1, *(int*)(c + 0x64));
    int r2 = b->m();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c + 0x5c), *(int*)(c + 0x60) + r2, *(int*)(c + 0x64));
    *(int*)(c + 0x3f4) = 4;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov077_02123fcc, 0x02123fcc, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *);
extern unsigned char DecIfAbove0_Byte(unsigned char *);
extern void func_ov077_02123a1c(char* c);
int func_ov077_02123fcc(char* c){
    *(short*)((char*)c+0x8c)=*(short*)((char*)c+0x8c)-0x1000;
    _ZN9Animation7AdvanceEv((char*)c+0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char*)c+0x1c4);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer((char*)c+0x204);
    if(!_ZNK10dBgCh_Actr13JustHitGroundEv((char*)c+0x204)){
        if(DecIfAbove0_Byte((unsigned char*)c+0x41c)) goto end;
    }
    func_ov077_02123a1c(c);
end:
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov077_02123d40, 0x02123d40, size 0x28c */
/* -------------------------------------------------------------------------- */
#include "types.h"
typedef struct Mtx43 { int w[12]; } Mtx43;

extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 angX);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(void* m, s16 angY);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* c, void* shadow, void* mtx, int rad, int height, u32 flags);
extern "C" void MulMat4x3Mat4x3(void* a, void* b, void* out);
extern "C" void Vec3_LslInPlace(void* v, int sh);
extern "C" void Vec3_Asr(void* d, void* s, int sh);
extern "C" void Matrix4x3_FromTranslation(void* m, int x, int y, int z);

extern Mtx43 data_020a0e68;

struct VObj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();  // slot 0x74 = index 29
};

extern "C" void func_ov077_02123d40(void* vc)
{
    char* c = (char*)vc;
    int b = (int)((*(u32*)(c + 0xb0) & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(c + 0xf0, *(s16*)(c + 0x8e));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = (*(int*)(c + 0x60) + *(int*)(c + 0x414)) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;

    if (*(int*)(c + 0x3f4) == 4) {
        data_020a0e68 = *(Mtx43*)(c + 0xf0);
        int y1 = ((VObj*)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c + 0x8c));
        int y2 = (-((VObj*)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        *(Mtx43*)(c + 0xf0) = data_020a0e68;
    }

    *(int*)(c + 0x3e4) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3e8) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3ec) = *(int*)(c + 0x64) >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x188, c + 0x3c0, 0x5a000, 0x320000, 0xf);

    if (*(int*)(c + 0x3f4) != 1)
        return;

    u32 t = (u32)(*(int*)(c + 0x12c) << 4) >> 0x10;
    if (t < 0x19)
        return;
    if (t > 0x3a)
        return;

    struct { int t[3]; int v[3]; } lv;
    lv.t[0] = -0x2000;
    lv.t[1] = 0x1800;
    lv.t[2] = 0x1000;

    data_020a0e68 = *(Mtx43*)(c + 0xf0);
    MulMat4x3Mat4x3(*(char**)(c + 0xe8) + 0x90, &data_020a0e68, &data_020a0e68);

    *(int*)(c + 0x404) = data_020a0e68.w[9];
    *(int*)(c + 0x408) = data_020a0e68.w[10];
    *(int*)(c + 0x40c) = data_020a0e68.w[11];

    Vec3_LslInPlace(c + 0x404, 3);

    Vec3_Asr(lv.v, c + 0x404, 3);

    Matrix4x3_FromTranslation(&data_020a0e68, lv.v[0], lv.v[1], lv.v[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, lv.t[0], lv.t[1], lv.t[2]);

    *(Mtx43*)(c + 0x154) = data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov077_02123c6c, 0x02123c6c, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02123c6c
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgCh_Actr.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

struct RG { char buf[0x54]; };
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(void* w);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* w);
extern int _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, struct Vector3* v);
extern int _ZN9dBgCh_GndC1Ev(struct RG* r);
extern int _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct RG* r, struct Vector3* v, void* a);
extern int _ZN5dBgCh19StartDetectingWaterEv(struct RG* r);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct RG* r);
extern int _ZN5dBgCh18StopDetectingWaterEv(struct RG* r);
extern int _ZN9dBgCh_GndD1Ev(struct RG* r);
void func_ov077_02123c6c(char* c, void* w){
  struct Vector3 nrm;
  struct Vector3 pos;
  struct RG rg;
  dBgCh_Actr_UpdateDiscreteNoLava_veneer(w);
  if (_ZNK10dBgCh_Actr8IsOnWallEv(w) != 0) {
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(w) + 4, &nrm);
    *(unsigned char*)(((int)c + 0x41d)) ^= 1;
  }
  pos.x = *(int*)(c+0x5c);
  pos.y = *(int*)(c+0x60);
  pos.z = *(int*)(c+0x64);
  pos.y += 0x64000;
  _ZN9dBgCh_GndC1Ev(&rg);
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &pos, 0);
  _ZN5dBgCh19StartDetectingWaterEv(&rg);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
    int yy = *(int*)((char*)&rg + 0x44) + 0x3c000;
    if (*(int*)(c+0x60) < yy) *(int*)(c+0x60) = yy;
  }
  _ZN5dBgCh18StopDetectingWaterEv(&rg);
  _ZN9dBgCh_GndD1Ev(&rg);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov077_02123a74, 0x02123a74, size 0x1f8 */
/* -------------------------------------------------------------------------- */
extern "C" int _ZN8dActor_c7FindEggER5dCc_c(void *self, void *clsn); /* decl_Actor.h view */
extern "C" void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const Vector3& pos);
extern "C" void func_ov077_02123a1c(char *c); /* aligned to the TU decl */
extern "C" void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(void *self, void *player);
extern "C" int _ZN6Player9IsOnShellEv(void *p);
extern "C" short Vec3_HorzAngle(void *a, void *b);
extern "C" void _ZN6Player16IncMegaKillCountEv(void *p);
extern "C" int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *self, void *clsn, void *player);
extern "C" void _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *pos, unsigned int a, int fix, unsigned int b, unsigned int cc, unsigned int d);
namespace two_arg_478c { extern "C" void func_ov077_0212478c(void* c, int i); } /* decl_common carries a 1-arg view of this name that Lakitu::InitResources's call byte-requires; the namespace keeps this block's true 2-arg view out of its way (both bind the same C symbol) */

extern "C" void func_ov077_02123a74(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    if (_ZN8dActor_c7FindEggER5dCc_c(c, c + 0x1c4) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, *(const Vector3*)(c + 0x74));
        func_ov077_02123a1c((char *)c);
        return;
    }

    {
        unsigned int id = *(unsigned int *)(c + 0x1e8);
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
        two_arg_478c::func_ov077_0212478c(c, 2);
        return;
    }

    if ((*(int *)(c + 0x1e4) & 0x66fe0)
        || _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(c, r4) != 0
        || _ZN6Player9IsOnShellEv(r4) != 0
        || *(unsigned char *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, *(const Vector3*)(c + 0x74));
        func_ov077_02123a1c((char *)c);
        return;
    }

    if (*(int *)(c + 0x1e4) & 0x10) {
        *(short *)(c + 0x94) = Vec3_HorzAngle(r4 + 0x5c, c + 0x5c);
        *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x94) + 0x8000);
        _ZN6Player16IncMegaKillCountEv(r4);
        two_arg_478c::func_ov077_0212478c(c, 4);
        return;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x1c4, r4) != 0) {
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        func_ov077_02123a1c((char *)c);
        return;
    }

    if (*(int *)(c + 0x3f4) == 3)
        return;

    {
        int v[3];
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, v, 2, 0xc000, 1, 0, 1);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov077_02123a1c, 0x02123a1c, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov077_02123a1c
/* recovered: shared common types */
#include "common.h"

extern "C"{
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void*,Vector3 const&,unsigned int,int,short);
void _ZN8dActor_c8PoofDustEv(void*);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
void func_ov077_02123a1c(char* c){
  Vector3 t; t.x=*(int*)(c+0x5c); t.y=*(int*)(c+0x60); t.z=*(int*)(c+0x64);
  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c,t,5,0x3000,0);
  _ZN8dActor_c8PoofDustEv(c);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov077_0212390c, 0x0212390c, size 0x110 */
/* -------------------------------------------------------------------------- */
extern "C" {
namespace no_arg_cnvp { extern "C" char* _ZN8dActor_c22ClosestNonVanishPlayerEv(); } /* this block byte-requires the zero-arg call (no r0 setup); the TU's file-scope view takes (void*) */
extern int Vec3_HorzDist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(void* a, short b, short c);
extern int func_ov077_021238bc(int unused, int x);
extern void _Z14ApproachLinearRiii(void* a, int b, int c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* a, void* b);
extern int _ZN9Spindrift8BehaviorEv(void* a, void* b);
void func_ov077_0212390c(char* c){
  char* p = no_arg_cnvp::_ZN8dActor_c22ClosestNonVanishPlayerEv();
  char* tgt;
  int r6;
  if(p != 0){
    r6 = (*(int*)(c+8) != 0) ? 0x1068000 : 0x7d0000;
    if(Vec3_HorzDist(c+0x3f8, p+0x5c) < r6){
      int d = *(int*)(c+0x3fc) - *(int*)(p+0x60);
      if(d < 0) d = -d;
      if(d < 0x5dc000){ tgt = p+0x5c; goto L64; }
    }
    tgt = c+0x3f8;
  } else {
    tgt = c+0x3f8;
  }
L64:
  {
    int hd = Vec3_HorzDist(c+0x5c, tgt);
    short ha = Vec3_HorzAngle(c+0x5c, tgt);
    _Z14ApproachLinearRsss(c+0x8e, ha, 0x5e8);
    if(*(unsigned char*)(c+0x41d) != 0){
      *(short*)(c+0x94) = *(short*)(c+0x8e) - func_ov077_021238bc((int)c, hd);
    } else {
      *(short*)(c+0x94) = *(short*)(c+0x8e) + func_ov077_021238bc((int)c, hd);
    }
    _Z14ApproachLinearRiii(c+0x60, *(int*)(tgt+4) + 0x12c000, 0x2000);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c+0x1c4);
    _ZN9Spindrift8BehaviorEv(c, c+0x204);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov077_021238bc, 0x021238bc, size 0x50 */
/* -------------------------------------------------------------------------- */
namespace cstd { int fdiv(int,int); }
extern "C" int func_ov077_021238bc(int unused, int x){
  if(x>0x190000) return 0;
  int q=cstd::fdiv(0x4000,0xc8000);
  long long m=(long long)q*x;
  m+=0x800;
  int r=(int)(m>>12);
  r=0x8000-r;
  return (short)r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov077_02123880, 0x02123880, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
namespace no_arg_3880 {  /* this definition byte-requires zero parameters; the TU's file-scope view is (void*) -- both bind the same C symbol */
extern "C" int func_ov077_02123880(void) {
  int r5 = 0;
  void* r1 = 0;
  do {
    r1 = (void*)_ZN8dActor_c15FindWithActorIDEjPS_(0x104, r1);
    if (r1) r5++;
  } while (r1);
  return r5;
}
}  /* namespace no_arg_3880 */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov077_02123814, 0x02123814, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern short data_02082214[];
void func_ov077_02123814(char* c){
  unsigned short v = *(unsigned short*)(c+0x418);
  short s = *(short*)((char*)data_02082214 + ((v>>4)<<2));
  long long prod = (long long)s * 0x27000;
  *(int*)(c+0x414) = (int)((prod + 0x800) >> 12);
  *(short*)(c+0x418) = (short)(*(short*)(c+0x418) + 0x700);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN6Lakitu16OnAimedAtWithEggEv, 0x0212380c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu16OnAimedAtWithEggEv
/* daJgm_c::OnAimedAtWithEgg -- vtable slot 29, recovered from vtable slot identity.
 * The ROM body ignores `this` and returns a constant. */
#include "Lakitu.h"

int Lakitu::OnAimedAtWithEgg()
{
    return 245760;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN6Lakitu13OnYoshiTryEatEv, 0x02123804, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6Lakitu13OnYoshiTryEatEv
/* daJgm_c::OnYoshiTryEat -- vtable slot 18, recovered from vtable slot identity.
 * The ROM body ignores `this` and returns a constant. */
#include "Lakitu.h"

int Lakitu::OnYoshiTryEat()
{
    return 6;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN6LakituD0Ev, 0x02123798, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6LakituD0Ev

#include "Lakitu.h"

/* (no separate definition: the single ~Lakitu() below emits the D0 and
 * D1 variants together; mwccarm orders the variant group itself.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN6LakituD1Ev, 0x02123740, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN6LakituD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then ~dActor_c. */
#include "Lakitu.h"

