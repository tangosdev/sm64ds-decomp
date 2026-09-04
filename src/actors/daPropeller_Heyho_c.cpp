//cpp
/* HAND-ASSEMBLED translation unit -- ov070/daPropeller_Heyho_c (27 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x0211f000  src/_ZN19daPropeller_Heyho_cD1Ev.cpp
 *   [1] 0x0211f048  src/_ZN19daPropeller_Heyho_cD0Ev.cpp
 *   [2] 0x0211f0a4  src/func_ov070_0211f0a4.cpp
 *   [3] 0x0211f100  src/func_ov070_0211f100.cpp
 *   [4] 0x0211f368  src/func_ov070_0211f368.cpp
 *   [5] 0x0211f450  src/func_ov070_0211f450.c
 *   [6] 0x0211f48c  src/func_ov070_0211f48c.c
 *   [7] 0x0211f5f0  src/func_ov070_0211f5f0.cpp
 *   [8] 0x0211f62c  src/func_ov070_0211f62c.c
 *   [9] 0x0211f694  src/func_ov070_0211f694.cpp
 *   [10] 0x0211f6e0  src/func_ov070_0211f6e0.c
 *   [11] 0x0211fa80  src/func_ov070_0211fa80.c
 *   [12] 0x0211fae4  src/func_ov070_0211fae4.c
 *   [13] 0x0211fd60  src/func_ov070_0211fd60.c
 *   [14] 0x0211fd98  src/func_ov070_0211fd98.c
 *   [15] 0x0211ffa8  src/func_ov070_0211ffa8.cpp
 *   [16] 0x02120020  src/FlyGuy_ChangeState.cpp
 *   [17] 0x02120070  src/func_ov070_02120070.cpp
 *   [18] 0x02120150  src/_ZN19daPropeller_Heyho_c16CleanupResourcesEv.cpp
 *   [19] 0x021201bc  src/_ZN19daPropeller_Heyho_c16OnPendingDestroyEv.cpp
 *   [20] 0x021201c0  src/_ZN19daPropeller_Heyho_c6RenderEv.cpp
 *   [21] 0x02120210  src/_ZN19daPropeller_Heyho_c8BehaviorEv.cpp
 *   [22] 0x021203b4  src/_ZN19daPropeller_Heyho_c13InitResourcesEv.cpp
 *   [23] 0x021204e4  src/_ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv.cpp
 *   [24] 0x021204ec  src/_ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player.cpp
 *   [25] 0x02120518  src/_ZN19daPropeller_Heyho_c13OnYoshiTryEatEv.cpp
 *   [26] 0x02120520  src/FlyGuy_Spawn.c
 */

/* The cartridge preserves this descriptor's C-ABI symbol but not its source
 * spelling.  Keep that evidence-bounded alias while giving the table its real
 * field layout; all seven scalar words and the factory relocation are covered
 * by the TU's data claim. */
extern "C" int *daPropeller_Heyho_c_classInit(void);

struct PropellerHeyhoSpawnInfo {
    int *(*spawn)();
    short behaviorPriority;
    short renderPriority;
    unsigned flags;
    int rangeOffsetY;
    int range;
    int drawDistance;
    int untrackDistance;
};

typedef char PropellerHeyhoSpawnInfo_size_must_be_0x1c[
    sizeof(PropellerHeyhoSpawnInfo) == 0x1c ? 1 : -1];

extern "C" PropellerHeyhoSpawnInfo FlyGuy_SpawnInfo = {
    daPropeller_Heyho_c_classInit,
    0x00e8,
    0x0057,
    0x10000003,
    0x00064000,
    0x000c8000,
    0x01000000,
    0x01000000
};

struct V3w { int w[3]; };  /* array-wrapper: C++ scalarizes a plain struct copy; this form keeps the C front end's ldm/stm block copy */
struct V3h { short h[3]; };

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- daPropeller_Heyho_c_classInit, 0x02120520, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daPropeller_Heyho_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV19daPropeller_Heyho_c */
/* Reconstructed source-style name: SM64DS proves daPropeller_Heyho_c through
 * RTTI, allocation size, vtable identity, and the PROPELLER_HEYHO registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: FlyGuy_Spawn. */
int *daPropeller_Heyho_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1000);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)&_ZTV19daPropeller_Heyho_c[2]; /* +8: this TU defines the vtable */
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN19daPropeller_Heyho_c13OnYoshiTryEatEv, 0x02120518, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13OnYoshiTryEatEv
#include "daPropeller_Heyho_c.h"
/* recovered: renamed to Class_Method */
s32 daPropeller_Heyho_c::OnYoshiTryEat() {
    return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player, 0x021204ec, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player
// recovered name: FlyGuy_OnTurnIntoEgg
/* daPropeller_Heyho_c::OnTurnIntoEgg -- vtable slot 19, verified against ov070
 * relocs.txt: _ZTV19daPropeller_Heyho_c (0x02123168) + 0x4c -> 0x021204ec, exactly this
 * placeholder's former address (former name func_ov070_021204ec).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov070).
 */
#include "daPropeller_Heyho_c.h"
#include "Player.h"

int daPropeller_Heyho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv, 0x021204e4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv
#include "daPropeller_Heyho_c.h"
// recovered name: FlyGuy_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daPropeller_Heyho_c::OnAimedAtWithEgg - recovered from vtable slot identity */
s32 daPropeller_Heyho_c::OnAimedAtWithEgg() {
    return 176128;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN19daPropeller_Heyho_c13InitResourcesEv, 0x021203b4, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"
#include "SharedFilePtr.h"
extern SharedFilePtr data_ov070_02123530;
extern SharedFilePtr data_ov070_02123520;
extern SharedFilePtr data_ov070_02123518;
extern SharedFilePtr data_ov070_02123510;
extern SharedFilePtr data_ov070_02123528;
extern SharedFilePtr data_ov070_02123508;
extern SharedFilePtr data_ov070_02123500;
extern char data_ov070_0212359c[];
extern "C" {
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, dActor_c* a, int r, int h, Vector3_16* p, Vector3_16* q);
extern int FlyGuy_ChangeState(void* c, void* p);
}

int daPropeller_Heyho_c::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x300, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov070_02123530), 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123520);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123518);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123510);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123528);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123508);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov070_02123500);
    unk_3e0 = param1 & 0xff;
    if (unk_3e0 == 0xff) unk_3e0 = 0;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0x110, (dActor_c*)((char*)this), 0x3c000, 0x32000, 0x200000, 0x7eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x144, (dActor_c*)((char*)this), 0x50000, 0x3c000, 0, 0);
    unk_108 = 1;
    unk_10a = 1;
    unk_3c0 = mPosX;
    unk_3c4 = mPosY;
    unk_3c8 = mPosZ;
    FlyGuy_ChangeState(((char*)this), &data_ov070_0212359c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN19daPropeller_Heyho_c8BehaviorEv, 0x02120210, size 0x1a4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"

/* This file used to open with `struct dEnemyBase_c { char pad[0x800]; };` and work a
 * `char *c` through raw offsets. daPropeller_Heyho_c.h now supplies the real chain, so the
 * stand-in is gone and every offset is a named field.
 *
 * dEnemyBase_c::UpdateYoshiEat is still reached by its mangled name -- unlike
 * UpdateDeath and UpdateWMClsn, it is not declared in dEnemyBase_c.h yet.
 */
extern "C" {
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(dEnemyBase_c *thiz, dBgCh_Actr *c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
}

int daPropeller_Heyho_c::Behavior()
{
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(this, &mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov070_02120070((char *)this);
        return 1;
    }

    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        func_ov070_02120070((char *)this);
        return 1;
    }

    if (mCurrentState != (State *)data_ov070_021235cc) {
        DecIfAbove0_Short((unsigned short *)&mStateTimer);
    }
    DecIfAbove0_Short(&unk_3cc);

    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) {
            (this->*(q->mMain))();
        }
    }

    {
        /* Gravity, clamped at terminal velocity. unk_0ac is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int v = mVertSpeed + mVertAccel;
        int hi = mTerminalVelocity;
        if (v >= hi) {
            hi = v;
        }
        int tmp = unk_0ac;
        mVertSpeed = hi;
        unk_0ac = tmp;
    }

    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);

    if (mCurrentState != (State *)data_ov070_021235bc) {
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
    }

    func_ov070_02120070((char *)this);

    if (mCurrentState != (State *)data_ov070_021235bc) {
        func_ov070_0211f100((char *)this);
    }

    mdCcAc_c.Clear();
    {
        char *p = (char *)ClosestPlayer();
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            mdCcAc_c.Update();
        }
    }

    mModelAnim.speed = 0x1000;
    mModelAnim.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN19daPropeller_Heyho_c6RenderEv, 0x021201c0, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daPropeller_Heyho_c.h"
struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void Target(int);
};

int daPropeller_Heyho_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    Obj *o = (Obj*)((char *)&mModelAnim);
    o->Target(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN19daPropeller_Heyho_c16OnPendingDestroyEv, 0x021201bc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
#include "daPropeller_Heyho_c.h"

void daPropeller_Heyho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN19daPropeller_Heyho_c16CleanupResourcesEv, 0x02120150, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daPropeller_Heyho_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 7 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "daPropeller_Heyho_c.h"
#include "SharedFilePtr.h"

extern "C" {
}

int daPropeller_Heyho_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov070_02123530)->Release();
    ((SharedFilePtr *)&data_ov070_02123520)->Release();
    ((SharedFilePtr *)&data_ov070_02123518)->Release();
    ((SharedFilePtr *)&data_ov070_02123510)->Release();
    ((SharedFilePtr *)&data_ov070_02123528)->Release();
    ((SharedFilePtr *)&data_ov070_02123508)->Release();
    ((SharedFilePtr *)&data_ov070_02123500)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov070_02120070, 0x02120070, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_02120070
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);
extern int data_020a0e68[];

typedef struct { int w[12]; } M48;

void func_ov070_02120070(char* c)
{
    Vector3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(M48*)(c + 0x31c) = *(M48*)data_020a0e68;
    Matrix4x3_FromTranslation(data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xa000) >> 3, *(int*)(c + 0x64) >> 3);
    *(M48*)(c + 0x38c) = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x38c, 0x76000, 0x320000, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- FlyGuy_ChangeState, 0x02120020, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol FlyGuy_ChangeState
struct C; typedef int (C::*PMF)();
struct C { char pad[0x3bc]; PMF *pp; };
extern "C" int FlyGuy_ChangeState(void *vc, void *vp) { C *c = (C *)vc; PMF *p = (PMF *)vp; c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov070_0211ffa8, 0x0211ffa8, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211ffa8
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern unsigned int RandomIntInternal(void* s);
extern int data_0209e650[];
int func_ov070_0211ffa8(char* c){
  *(short*)(c+0x300+0xe6) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0xf) << 0xc);
  *(short*)(c+0x100) = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x32);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)((int *)&data_ov070_02123520)[1], 0, 0x1000, 0);
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov070_0211fd98, 0x0211fd98, size 0x210 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fd98
extern "C" {  /* .c-derived member: C linkage for the whole block */
typedef short s16;

extern int Vec3_Dist(void *a, void *b);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *p);
extern short Vec3_HorzAngle(void *a, void *b);
extern void ApproachAngle(s16 *dst, s16 target, int a, int b, int c);
extern short Vec3_VertAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(void *dst, short a, short b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void _Z14ApproachLinearRiii(void *dst, int a, int b);
extern int FlyGuy_ChangeState(void *c, void *p);
extern void *_ZN8dActor_c22ClosestNonVanishPlayerEv(void *c);

extern int data_020a0e68[];
extern char data_ov070_0212359c[];
extern char data_ov070_021235ac[];

int func_ov070_0211fd98(char *c)
{
    int in[3];
    int out[3];
    Vector3 t;
    void *p;

    in[0] = 0; in[1] = 0; in[2] = 0;
    out[0] = 0; out[1] = 0; out[2] = 0;

    if (Vec3_Dist(c + 0x5c, c + 0x3c0) > 0x1f4000 ||
        _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144)) {
        *(s16 *)(c + 0x300 + 0xe6) = Vec3_HorzAngle(c + 0x5c, c + 0x3c0);
        if (*(unsigned short *)(c + 0x100) < 0x14)
            *(unsigned short *)(c + 0x100) = 0x14;
    }
    ApproachAngle((s16 *)(c + 0x94), *(s16 *)(c + 0x300 + 0xe6), 0xa, 0x200, 0x100);

    _Z14ApproachLinearRsss(c + 0x92, Vec3_VertAngle(c + 0x5c, c + 0x3c0), 0x100);

    ApproachAngle((s16 *)(c + 0x96),
                  (*(s16 *)(c + 0x94) - *(s16 *)(c + 0x300 + 0xe6)) / 2,
                  0xa, 0x100, 0x50);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16 *)(c + 0x92));
    MulVec3Mat4x3(in, data_020a0e68, out);

    _Z14ApproachLinearRiii(c + 0xa4, out[0], 0x1000);
    _Z14ApproachLinearRiii(c + 0xa8, out[1], 0x800);
    _Z14ApproachLinearRiii(c + 0xac, out[2], 0x1000);

    if (*(unsigned short *)(c + 0x100) == 0) {
        FlyGuy_ChangeState(c, data_ov070_0212359c);
        return 1;
    }
    if (*(unsigned short *)(c + 0x300 + 0xcc) != 0)
        return 1;
    if (Vec3_Dist(c + 0x5c, c + 0x3c0) < 0x5dc000) {
        p = _ZN8dActor_c22ClosestNonVanishPlayerEv(c);
        if (p) {
            int *pos = (int *)(((int)p + 0x5c));
            t.x = pos[0];
            t.y = pos[1];
            t.z = pos[2];
            if (Vec3_Dist(c + 0x5c, &t) < 0x3e8000)
                FlyGuy_ChangeState(c, data_ov070_021235ac);
        }
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov070_0211fd60, 0x0211fd60, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fd60
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
/* (data_ov070_02123520: SharedFilePtr view declared earlier in this TU) */
int func_ov070_0211fd60(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x300, ((void**)&data_ov070_02123520)[1], 0, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_0211fae4, 0x0211fae4, size 0x27c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fae4
extern "C" {  /* .c-derived member: C linkage for the whole block */

extern void *_ZN8dActor_c22ClosestNonVanishPlayerEv(void *self);
extern short Vec3_HorzAngle(void *v0, void *v1);
extern short Vec3_VertAngle(void *v1, void *v0);
extern void ApproachAngle(s16 *cur, s16 target, int a, int b, int c);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern int Vec3_Dist(void *a, void *b);
extern unsigned int RandomIntInternal(void *seed);
extern int FlyGuy_ChangeState(void *c, void *p);

extern signed char data_0209f2f8;
extern char data_ov070_021235dc[];
extern int data_020a0e68[];
extern char data_ov070_0212359c[];
extern int data_0209e650[];
extern char data_ov070_021235cc[];
extern char data_ov070_0212358c[];

int func_ov070_0211fae4(void *arg)
{
    char *c = (char *)arg;
    char *player;
    Vector3 vin;
    Vector3 vb;
    Vector3 vc;
    Vector3 vd;

    player = (char *)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (player == 0) {
        if (data_0209f2f8 != 0x16) {
            *(int *)(c + 0x3c0) = *(int *)(c + 0x5c);
            *(int *)(c + 0x3c4) = *(int *)(c + 0x60);
            *(int *)(c + 0x3c8) = *(int *)(c + 0x64);
            *(int *)(((int)c + 0x3c4)) += 0xc8000;
        }
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        *(short *)(c + 0x92) = 0;
        *(short *)(c + 0x100) = 0;
        *(int *)(c + 0xa8) = 0;
        FlyGuy_ChangeState(c, data_ov070_021235dc);
        return 1;
    }

    vin.x = 0;
    vin.y = 0;
    vin.z = 0;

    {
    int *q = (int *)(((int)player + 0x5c));
    vb.x = q[0];
    vb.y = q[1];
    vb.z = q[2];
    }
    vb.y += 0xc8000;
    vc.x = vb.x;
    vc.y = vb.y;
    vc.z = vb.z;

    *(short *)(c + 0x3e6) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &vc);
    ApproachAngle((short *)(c + 0x94), *(short *)(c + 0x3e6), 1, 0x500, 0x500);

    vd.x = vb.x;
    vd.y = vb.y;
    vd.z = vb.z;
    ApproachAngle((short *)(c + 0x92), Vec3_VertAngle((Vector3 *)(c + 0x5c), &vd), 1, 0x500, 0x500);

    ApproachAngle((short *)(c + 0x96),
                  (*(short *)(c + 0x94) - *(short *)(c + 0x3e6)) / 2,
                  0xa, 0x100, 0x50);

    vin.z = 0xf000;
    Matrix4x3_FromRotationY(data_020a0e68, *(short *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short *)(c + 0x92));
    MulVec3Mat4x3(&vin, data_020a0e68, c + 0xa4);

    if (Vec3_Dist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x3c0)) > 0x5dc000) {
        FlyGuy_ChangeState(c, data_ov070_0212359c);
        return 1;
    }

    {
    int dist = Vec3_Dist((Vector3 *)(c + 0x5c), &vb);
    int thresh = 0x258000;
    if (data_0209f2f8 == 0x16) thresh = 0x384000;
    if (dist < thresh) {
        *(int *)(c + 0xa4) = 0;
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0xac) = 0;
        if (*(int *)(c + 0x3e0) == 0 ||
            (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 1) == 0) {
            FlyGuy_ChangeState(c, data_ov070_021235cc);
        } else {
            FlyGuy_ChangeState(c, data_ov070_0212358c);
        }
    }
    }

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_0211fa80, 0x0211fa80, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211fa80
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int _ZN8dActor_c23HorzAngleToCPlayerOrAngEv(void *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
int func_ov070_0211fa80(char *c) {
    *(int *)(c + 0x3dc) = 0;
    *(short *)(c + 0x100) = 0x3f;
    *(int *)(c + 0x3d8) = 0;
    *(short *)(c + 0x3e6) = _ZN8dActor_c23HorzAngleToCPlayerOrAngEv(c);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)((int *)&data_ov070_02123518)[1], 0x40000000, 0x1000, 0);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_0211f6e0, 0x0211f6e0, size 0x3a0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f6e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;


/* (data_ov070_02123510: SharedFilePtr view declared earlier in this TU) */
extern s8 data_0209f2f8;
extern char data_ov070_0212359c[];
extern char data_ov070_021235dc[];
extern s32 data_0209f32c;
extern int data_020a0e68[];

/* (ApproachAngle: this file's own int-target view, declared inside the function body) */
extern int _ZN9Animation8FinishedEv(void* thiz);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
extern int FlyGuy_ChangeState(void* c, void* p);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* thiz);
extern void* _ZN8dActor_c22ClosestNonVanishPlayerEv(void* thiz);
extern short Vec3_VertAngle(void* v1, void* v0);
extern int Vec3_Dist(void* a, void* b);
extern u16 DecIfAbove0_Short(u16* p);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, s16 angX);
extern void MulVec3Mat4x3(void* v, void* m, void* res);

int func_ov070_0211f6e0(char* c)
{
    extern int ApproachAngle(s16* angle, int target, int step, int maxDelta, int minDelta); /* byte-load-bearing: int target */
    char* player;
    Vector3 tmp;
    Vector3 v;
    Vector3 aim;
    s16 vAngle;
    s16 half;
    s32 z;

    ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x3e6), 0x100, 0x1000, 0x1000);
    ApproachAngle((s16*)(c + 0x96), 0, 0x100, 0x1000, 0x1000);

    if (_ZN9Animation8FinishedEv(c + 0x350)) {
        if (*(s32*)(c + 0x3d8) == 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)((int *)&data_ov070_02123510)[1], 0, 0x1000, 0);
            *(s32*)(c + 0x3d8) = 1;
        }
        if (*(s32*)(c + 0x3dc) == 1) {
            if (data_0209f2f8 != 0x16)
                *(s32 *)(c + 0x3c4) += 0x12c000;
            *(s32*)(c + 0x3d8) = 0;
            *(u16*)(c + 0x3cc) = 0x5a;
            FlyGuy_ChangeState(c, &data_ov070_0212359c);
            return 1;
        }
    }

    if (*(u16*)(c + 0x100) == 0 || _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144)) {
        if (data_0209f2f8 != 0x16) {
            *(s32*)(c + 0x3c0) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3c4) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x64);
        }
        *(u16*)(c + 0x92) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    player = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (player == 0) {
        if (data_0209f2f8 != 0x16) {
            *(s32*)(c + 0x3c0) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3c4) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x64);
        }
        *(u16*)(c + 0x92) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    if (*(u8*)(player + 0x706) != 0 && data_0209f32c > *(s32*)(c + 0x60)) {
        if (data_0209f2f8 != 0x16) {
            *(s32*)(c + 0x3c0) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3c4) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x64);
            *(s32 *)(c + 0x3c4) += 0xc8000;
        }
        *(s32*)(c + 0x5c) = *(s32*)(c + 0x68);
        *(s32*)(c + 0x60) = *(s32*)(c + 0x6c);
        *(s32*)(c + 0x64) = *(s32*)(c + 0x70);
        *(u16*)(c + 0x92) = 0;
        *(u16*)(c + 0x100) = 0;
        *(u32*)(c + 0xa8) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    *(V3w*)&tmp = *(V3w*)(player + 0x5c);  /* array-wrapper keeps the ldm/stm block copy under -lang c++ */
    z = 0;
    v.x = z;
    v.y = z;
    v.z = z;
    tmp.y = *(int *)(player + 0x644);
    if (data_0209f2f8 == 0x16)
        tmp.y += 0x32000;
    else
        tmp.y += 0x47000;
    {
        int tx = tmp.x;
        int ty = tmp.y;
        int tz = tmp.z;
        aim.x = tx;
        aim.y = ty;
        aim.z = tz;
    }
    vAngle = Vec3_VertAngle((Vector3*)(c + 0x5c), &aim);
    ApproachAngle((s16*)(c + 0x92), vAngle, 0xa, 0x200, 0x100);

    v.z = 0x11000;
    if (*(s32*)(c + 0x60) <= *(s32*)((char*)&tmp + 4) + 0x5000 ||
        *(s32*)(c + 0x60) <= *(s32*)(player + 0x60) + 0x5000 ||
        Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3c0)) > 0x5dc000) {
        DecIfAbove0_Short((u16*)(c + 0x100));
        v.z = 0x9000;
    }

    half = (*(s16*)(c + 0x94) - *(s16*)(c + 0x3e6)) / 2;
    ApproachAngle((s16*)(c + 0x96), half, 0xa, 0x100, 0x50);

    Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16*)(c + 0x92));
    MulVec3Mat4x3(&v, data_020a0e68, (Vector3*)(c + 0xa4));

    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_0211f694, 0x0211f694, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f694
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
int func_ov070_0211f694(char *c) {
    *(int*)(c+0x3d8) = 0;
    if (*(int*)(c+0x3dc) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, (void*)((int *)&data_ov070_02123508)[1], 0x40000000, 0x1000, 0);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_0211f62c, 0x0211f62c, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f62c
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int _ZN9Animation8FinishedEv(void *p);
extern signed char data_0209f2f8;
extern int FlyGuy_ChangeState(void *c, void *p);
extern char data_ov070_0212359c[];

int func_ov070_0211f62c(char *c)
{
    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        if (data_0209f2f8 != 0x16)
            *(int *)(c + 0x3c4) += 0x12c000;
        *(int *)(c + 0x3d8) = 0;
        *(short *)(c + 0x3cc) = 0x5a;
        FlyGuy_ChangeState(c, &data_ov070_0212359c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_0211f5f0, 0x0211f5f0, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f5f0
struct BCA_File;
/* (ModelAnim: real header type in scope) */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);


/* (data_ov070_02123500: SharedFilePtr view declared earlier in this TU) */

extern "C" int func_ov070_0211f5f0(char *c) {
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov070_02123500)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0x300), file, 0x40000000, 0x1000, flags);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_0211f48c, 0x0211f48c, size 0x164 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov070_0211f48c
/* recovered: shared common types */
#include "common.h"
char* _ZN8dActor_c13ClosestPlayerEv(void* self);
short Vec3_HorzAngle(void* a, void* b);
/* (ApproachAngle: this file's own int-target view, declared inside the function body) */
int _ZNK9Animation12WillHitFrameEi(void* a, int f);
short Vec3_VertAngle(void* a, void* b);
void* _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(void* self, void* pos, void* vel, int a, int b, unsigned int d);
void func_02012694(int a, void* p);
int _ZN9Animation8FinishedEv(void* a);
int FlyGuy_ChangeState(void* c, void* p);
extern char data_ov070_0212359c[];

#define M(p) (p)

int func_ov070_0211f48c(char* c) {
    extern void ApproachAngle(short* p, int target, int a, int b, int limit); /* byte-load-bearing: int target */
    char* pl;
    struct Vector3_16 vel;
    struct Vector3 posbuf;
    struct Vector3 fp;
    struct Vector3 tmp;

    pl = _ZN8dActor_c13ClosestPlayerEv(c);
    if ((unsigned)(*(int*)(c+0x358) << 4) >> 0x10 >= 0xd)
        goto hitframe;

    if (pl != 0) {
        *(V3w*)&posbuf = *(V3w*)(pl+0x5c);
        tmp.x = posbuf.x;
        tmp.y = posbuf.y;
        tmp.z = posbuf.z;
        *(short*)(c + 0x3e6) = Vec3_HorzAngle(c+0x5c, &tmp);
    }
    ApproachAngle((short*)(c+0x94), *(short*)(c + 0x3e6), 0xa, 0x400, 0x200);

hitframe:
    if (_ZNK9Animation12WillHitFrameEi(c+0x350, 0xd) != 0) {
        *(V3h*)&vel = *(V3h*)(c+0x8c);
        if (pl != 0) {
            int *base = (int *)(int)M(pl + 0x5c);
            fp.x = base[0];
            fp.y = base[1];
            fp.z = base[2];
            vel.x = Vec3_VertAngle(c+0x5c, &fp);
        }
        _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(c, c+0x5c, &vel, 0x1e000, 0xa000, 1);
        func_02012694(0x105, c+0x74);
    }
    if (_ZN9Animation8FinishedEv(c+0x350) != 0) {
        *(int*)(c+0x358) = 0;
        *(short*)(c + 0x3cc) = 0x5a;
        FlyGuy_ChangeState(c, data_ov070_0212359c);
    }
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_0211f450, 0x0211f450, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f450
extern "C" {  /* .c-derived member: C linkage for the whole block */
short func_ov070_0211f450(char *c) {
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
    *(int *)(c + 0xa8) = 0x32000;
    *(int *)(c + 0x9c) = -0x5000;
    *(short *)(c + 0x100) = 3;
    *(int *)(c + 0xb0) = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_0211f368, 0x0211f368, size 0xe8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f368
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;

extern "C" unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e, void* f, void* g);
extern "C" unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, Fix12i c, Fix12i d, Fix12i e, void* f);
extern "C" void ApproachAngle(short* v, short a, int b, int c, int d);
extern "C" void _Z14ApproachLinearRsss(void* v, short a, short b);

extern "C" int func_ov070_0211f368(char* c)
{
    if (*(int*)(c + 0x3d8) != 0) {
        Vector3 v;
        int x, y, z;
        x = *(int*)(c + 0x5c);
        z = *(int*)(c + 0x64);
        y = *(int*)(c + 0x60) + 0x50000;
        ((int*)&v)[0] = x;
        ((int*)&v)[1] = y;
        ((int*)&v)[2] = z;
        *(unsigned*)(c + 0x3d0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned*)(c + 0x3d0), 0x13a, v.x, v.y, v.z, 0, 0);
        *(unsigned*)(c + 0x3d4) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned*)(c + 0x3d4), 0x13b, v.x, v.y, v.z, 0);
    }
    ApproachAngle((short*)(c + 0x8c), -0x4000, 0xa, 0x200, 0x100);
    _Z14ApproachLinearRsss((short*)(c + 0x8c), -0x4000, 0x200);
    if (*(unsigned short*)(c + 0x100) == 0)
        func_ov070_0211f0a4(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_0211f100, 0x0211f100, size 0x268 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f100
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct BCA_File;

extern char data_ov070_021235bc[];
extern char data_ov070_021235cc[];
/* (data_ov070_02123528: SharedFilePtr view declared earlier in this TU) */

extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern int FlyGuy_ChangeState(void* c, void* p);
extern int func_ov002_020aea30(void* c, void* p, int a, int b);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* c, void* clsn, void* player);
extern void _ZN6Player10SpinBounceE5Fix12IiE(void* p, s32 f);
extern void _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(void* enemy, void* actor, char* s);
extern void _ZN6Player16IncMegaKillCountEv(void* p);
extern void func_02012694(int a, void* b);
extern int _ZN6Player9IsOnShellEv(void* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u32 b, u32 c, u32 d);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix, unsigned int j);
}

extern "C" void func_ov070_0211f100(char* c)
{
    void* r5;
    s32 r4;

    if (*(s32*)(c + 0x134) == 0)
        return;
    r5 = _ZN8dActor_c10FindWithIDEj(*(s32*)(c + 0x134));
    if (!r5)
        return;

    r4 = *(s32*)(c + 0x130);
    if (r4 & 0x40000) {
        *(s32*)(c + 0x3d8) = 1;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }
    if (r4 & 0x20) {
        *(s32*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, r5, 0, 1);
        return;
    }
    if (r4 & 0x67c0) {
        *(s32*)(c + 0x3d8) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    {
        int isBf = (int)(*(u16*)((char*)r5 + 0xc) == 0xbf);
        if (!isBf)
            return;
    }
    if (*(u8*)((char*)r5 + 0x6fb) != 0)
        return;

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, r5)) {
        _ZN6Player10SpinBounceE5Fix12IiE(r5, 0x28000);
        *(s32*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, r5, 0, 1);
        return;
    }

    if (r4 & 0x10) {
        _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(c, r5, (char*)0);
        _ZN6Player16IncMegaKillCountEv(r5);
        func_02012694(0x1d, c + 0x74);
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    if (*(u8*)((char*)r5 + 0x6f9) == 1 || _ZN6Player9IsOnShellEv(r5) == 1) {
        *(s32*)(c + 0x3d8) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235bc);
        return;
    }

    {
        Vector3 v;
        v.x = *(s32*)(c + 0x5c);
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r5, &v, 2, 0xc000, 1, 0, 1);
    }
    if (*(s32*)(c + 0x3dc) != 0)
        return;
    if (*(s32*)(c + 0x3bc) != (s32)&data_ov070_021235cc)
        return;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)((int *)&data_ov070_02123528)[1], 0x40000000, 0x1000, 0);
    *(s32*)(c + 0x3d8) = 1;
    *(s32*)(c + 0x3dc) = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov070_0211f0a4, 0x0211f0a4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov070_0211f0a4
/* recovered: shared common types */
#include "common.h"


/* (dActor_c: real header type in scope; SpawnCoins goes through the mangled extern below) */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *, Vector3 const &, unsigned int, int, short);


extern "C" int func_ov070_0211f0a4(void *c) {
    dActor_c *a = (dActor_c *)c;
    a->SmallPoofDust();
    Vector3 pos;
    pos.x = *(int *)((char *)c + 0x5c);
    pos.y = *(int *)((char *)c + 0x60);
    pos.z = *(int *)((char *)c + 0x64);
    unsigned int coins = *(unsigned char *)((char *)c + 0x10a) + 1;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(a, pos, coins, 0xa000, 0);
    a->KillAndTrackInDeathTable();
}
