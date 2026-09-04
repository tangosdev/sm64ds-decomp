//cpp
/* ov027 / daPgDfdr_c -- the penguin-race defender of the ov027 set
 * (registry profile PENGUIN_DEFENDER, actor 258, RTTI name at ov027:0x021139f0).
 *
 * One translation unit for the whole ROM run 0x021118c8..0x021120c4: the five
 * class methods, the eight file-local helpers that sit between the destructor
 * pair and CleanupResources, the destructor pair itself and the factory, plus
 * the .data run 0x021139e4..0x02113b10 that holds the class's RTTI pair, its
 * profile descriptor, its nine-step waypoint table and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order. `.data` objects, by contrast,
 * emit in source order, which is why the profile descriptor and the waypoint
 * table are written here in the order the ROM holds them.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daPgDfdr_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */
#include "decl_common.h"
#include "decl_Player.h"

#include "daPgDfdr_c.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

struct BMD_File;
struct BTP_File;

/* The state-callback dispatch shape the two trampolines share. The
 * per-function files each spelled it locally; it is one type here. `pp` is
 * mStateTable at 0x3cc -- func_ov027_02111d70 points it at a row of
 * data_ov027_02113ce4, and the two trampolines call through its two words. */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x3cc]; PMF *pp; };

/* The rows func_ov027_02111d70 selects between. */
typedef struct Elem { int a, b, c, d; } Elem;

/* Reconciled from the sixteen per-function copies; where two of them spelled
 * the same symbol differently, one spelling is chosen here, since the whole TU
 * is now one file. Every one of these is a ROM symbol spelled by its final
 * mangled name, so it needs C linkage: seen as C++ the compiler would mangle
 * the already-mangled spelling a second time and emit a reference nothing
 * defines. Symbols decl_common.h already declares -- func_ov027_02111cfc,
 * func_ov027_02111d70, data_ov027_021130e8, data_ov027_02112ca4 and
 * _ZTV10daPgDfdr_c -- are deliberately absent, and the two definitions below
 * keep decl_common.h's signatures. */
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned);
extern void  _ZN10dBgActor_cC2Ev(void *self);
extern void  _ZN9ModelAnimC1Ev(void *self);
extern void  _ZN15TextureSequenceC1Ev(void *self);
extern void  _ZN7dCcAc_cC1Ev(void *self);

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *o, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *o, void *f, int i, int fx, unsigned j);
extern void  _ZN15TextureSequence6UpdateER15ModelComponents(void *o, void *mc);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *o, void *kcl, void *m, int fx, short s, void *clps);
extern void  _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *o, void *m, short s);
extern void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *o, void *act, int a, int b, unsigned c, unsigned d);
extern void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *o, void *f, int a, int b, unsigned int c);
extern void  _ZN13SharedFilePtr7ReleaseEv(void *fp);
extern void  _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
extern void  _ZN9Animation7AdvanceEv(void *a);
extern void  _ZN5dCc_c5ClearEv(void *a);
extern void  _ZN5dCc_c6UpdateEv(void *a);
extern void  _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);

extern void  Matrix4x3_FromRotationY(void *m, short angle);
extern int   DecIfAbove0_Byte(void *p);
extern int   _Z14ApproachLinearRsss(short *r, short t, short s);
extern void  func_0201267c(int a, char *b);
extern void  func_020393d4(void *p, int v);

/* None of these is in a decl_*.h; the per-function files declared them
 * locally. All of them live outside this TU's .data claim. */
extern char  data_ov027_02113c6c;
extern char  data_ov027_02113c7c;
extern char  data_ov027_02113c94;
extern int   data_ov027_02113c74[];
extern void *data_ov027_02113c84[];
extern int   data_ov027_02113c8c[];
extern Elem  data_ov027_02113ce4[];

/* Defined below, later in the file than their callers. */
void func_ov027_02111994(char *c);
void func_ov027_02111d38(void *self);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 15 -- daPgDfdr_c_classInit, 0x0211207c, size 0x48              */
/* ------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daPgDfdr_c through its ROM
 * RTTI string, its 0x3dc allocation, the vtable the factory installs and the
 * PENGUIN_DEFENDER registry profile; later EAD lineage supplies classInit.
 * Exact original spelling is not preserved. */
// @symbol daPgDfdr_c_classInit
extern "C" int *daPgDfdr_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(988);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x02113a88 and +8 is what reaches the
         * 0x02113a90 address point the ROM stores. The addend-0 spelling was
         * right only while the class merely IMPORTED its vtable. */
        p[0] = (int)&_ZTV10daPgDfdr_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN15TextureSequenceC1Ev((char *)p + 0x384);
        _ZN7dCcAc_cC1Ev((char *)p + 0x398);
    }
    return p;
}

/* ------------------------------------------------------------------------- */
/* g_profile_PENGUIN_DEFENDER, 0x02113a00, size 0x1c                          */
/* ------------------------------------------------------------------------- */
/* .data objects emit in SOURCE order, so the descriptor is written directly
 * after the factory it names -- which is where the ROM's run puts it, between
 * _ZTS (0x021139f0) and the waypoint table (0x02113a1c). */
struct PgDfdrSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x0102 == actor 258 */
    s16 drawOrder;                  /* 0x0074 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char PgDfdrSpawnInfo_size_must_be_0x1c[
    sizeof(PgDfdrSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_PENGUIN_DEFENDER
extern "C" PgDfdrSpawnInfo g_profile_PENGUIN_DEFENDER = {
    daPgDfdr_c_classInit, 0x0102, 0x0074, 0x00800002,
    0x000a0000, 0x009c4000, 0x01000000, 0x01000000
};

/* ------------------------------------------------------------------------- */
/* data_ov027_02113a1c -- the nine-step waypoint table, 0x6c bytes            */
/* ------------------------------------------------------------------------- */
/* mStepIndex (0x3d9) runs 0..8 and wraps; each row gives the distance to walk,
 * the speed to walk it at, the facing to hold and the facing to turn to.
 * func_ov027_02111b2c reads all four fields and func_ov027_02111c48 only the
 * fourth, so mwcc pooled four separate row-base addresses (+0, +4, +8, +0xa)
 * and dsd, seeing four relocation targets, configured a symbol at each. This is
 * the one array those four names are views of.
 *
 * THIS IS WHAT CURRENTLY BLOCKS PROMOTION -- see the manifest note. */
struct PgDfdrStep {
    s32 distance;      /* +0x0 */
    s32 speed;         /* +0x4 */
    s16 angle;         /* +0x8 */
    s16 nextAngle;     /* +0xa */
};
typedef char PgDfdrStep_size_must_be_0xc[sizeof(PgDfdrStep) == 0xc ? 1 : -1];

// @symbol data_ov027_02113a1c
extern "C" PgDfdrStep data_ov027_02113a1c[9] = {
    { 0x00220000, 0x00005000, (s16)0xdd30, (s16)0xdd30 },
    { 0x0016c000, 0x0000a000, (s16)0xdd30, (s16)0xdd30 },
    { 0x000b6000, 0x00005000, (s16)0x5d30, (s16)0xdd30 },
    { 0x0016c000, 0x00005000, (s16)0xdd30, (s16)0xdd30 },
    { 0x0016c000, 0x0000a000, (s16)0xdd30, (s16)0xdd30 },
    { 0x000b6000, 0x00005000, (s16)0x5d30, (s16)0xdd30 },
    { 0x0016c000, 0x00005000, (s16)0xdd30, (s16)0xdd30 },
    { 0x0016c000, 0x0000a000, (s16)0xdd30, (s16)0xdd30 },
    { 0x007d0000, 0x0000a000, (s16)0x5d30, (s16)0x5d30 }
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 14 -- daPgDfdr_c::InitResources, 0x02111eb4, size 0x1c8        */
/* ------------------------------------------------------------------------- */
/* Loads the model into mModelAnim, its three animations and its BTP texture
 * sequence, hard-codes the object's world position and facing, hangs the KCL on
 * the mesh collider with dBgW::UpdatePosAndAngs as the update hook, disables
 * gravity (mVertAccel and mTerminalVelocity both zero), starts the actor
 * collider, and finally drops the object onto whatever ground a raycast 0x14000
 * above it finds -- falling back to that raised point when nothing is hit.
 *
 * dBgW_KcMbg::SetFile, TextureSequence::SetFile and dCcAc_c::Init all carry
 * Fix12<int> BY VALUE in their mangled names (wall 6az), so all three keep
 * scalar extern "C" declarations rather than becoming callable methods. */
// @symbol _ZN10daPgDfdr_c13InitResourcesEv
s32 daPgDfdr_c::InitResources()
{
    int i;
    void *f;
    Vector3 pos;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov027_02113c7c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, f, 1, -1);

    for (i = 0; i < 3; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov027_02112ca4[i]);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov027_02113c94);
    TextureSequence::Prepare(**(BMD_File**)(&data_ov027_02113c7c + 4),
                             **(BTP_File**)(&data_ov027_02113c94 + 4));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, *(void**)(&data_ov027_02113c94 + 4), 0, 0x1000, 0);

    mAngleY = (short)0xdd30;
    mPrevAngleY = mAngleY;
    mPosX = 0x6c4000;
    mPosY = 0xcb2000;
    mPosZ = 0x182bb8;
    func_ov027_02111994((char *)this);

    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov027_02113c6c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY, &data_ov027_021130e8);
    func_020393d4(&mMeshCollider, (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x82000, 0xc8000, 0x800004, 0);
    func_ov027_02111d70((char *)this, 1);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 13 -- daPgDfdr_c::Behavior, 0x02111e34, size 0x80              */
/* ------------------------------------------------------------------------- */
/* The two Animation::Advance calls are the same call on two different
 * sub-objects, which the raw offsets hid: mModelAnim's Animation base sits at
 * +0x50 (the multiple-inheritance offset include/ModelAnim.h documents), and
 * mTextureSequence's sits at offset 0. */
// @symbol _ZN10daPgDfdr_c8BehaviorEv
s32 daPgDfdr_c::Behavior()
{
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    func_ov027_02111cfc((char *)this);
    if (_ZN6Player16IsInsideOfCannonEv(_ZN8dActor_c13ClosestPlayerEv(this))) {
        mFlags &= ~2;
    } else {
        mFlags |= 2;
    }
    _ZN9Animation7AdvanceEv(static_cast<Animation *>(&mModelAnim));
    _ZN9Animation7AdvanceEv(static_cast<Animation *>(&mTextureSequence));
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    func_ov027_02111994((char *)this);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 12 -- daPgDfdr_c::Render, 0x02111e00, size 0x34                */
/* ------------------------------------------------------------------------- */
/* Sub is a stand-in for ModelAnim's own vtable, whose slot 5 is Render: this
 * header does not declare it callable, so the call still goes through a shadow
 * -- but through &mModelAnim rather than through a 0x320-byte pad. The
 * TextureSequence update takes mModelAnim's ModelComponents. */
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

// @symbol _ZN10daPgDfdr_c6RenderEv
s32 daPgDfdr_c::Render()
{
    _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mModelAnim.data);
    Sub *b = (Sub *)&mModelAnim; b->m(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 11 -- daPgDfdr_c::OnPendingDestroy, 0x02111dfc, size 0x4       */
/* ------------------------------------------------------------------------- */
/* The ROM body is empty: the override exists only to occupy the slot, so the
 * base's own handler does not run for this class. */
// @symbol _ZN10daPgDfdr_c16OnPendingDestroyEv
void daPgDfdr_c::OnPendingDestroy()
{
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 10 -- daPgDfdr_c::CleanupResources, 0x02111d8c, size 0x70      */
/* ------------------------------------------------------------------------- */
/* Releases five shared files -- one, then the three-entry table, then another
 * -- and takes the inherited mesh collider out of the collision world in
 * between, before the last release. The order is the ROM's. */
// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
s32 daPgDfdr_c::CleanupResources()
{
    int i;
    _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c7c);
    for (i = 0; i < 3; i++) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov027_02112ca4[i]);
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c94);
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c6c);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov027_02111d70, 0x02111d70, size 0x1c                */
/* ------------------------------------------------------------------------- */
/* Points mStateTable at row `idx` of the state table and enters that state. */
// @symbol func_ov027_02111d70
extern "C" void func_ov027_02111d70(void *self, int idx)
{
    *(Elem **)((char *)self + 0x3cc) = &data_ov027_02113ce4[idx];
    func_ov027_02111d38(self);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov027_02111d38, 0x02111d38, size 0x38                */
/* ------------------------------------------------------------------------- */
/* The state's ENTER callback: word 0 of the row. */
// @symbol func_ov027_02111d38
extern "C" void func_ov027_02111d38(void *self)
{
    C *c = (C *)self;
    PMF *p = c->pp;
    (c->**p)();
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov027_02111cfc, 0x02111cfc, size 0x3c                */
/* ------------------------------------------------------------------------- */
/* The state's PER-FRAME callback: word 1 of the row. */
// @symbol func_ov027_02111cfc
extern "C" void func_ov027_02111cfc(void *self)
{
    C *c = (C *)self;
    PMF *p = c->pp + 1;
    (c->**p)();
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov027_02111ca8, 0x02111ca8, size 0x54                */
/* ------------------------------------------------------------------------- */
/* State "wait", on entry: play the idle animation, arm the 0x14-frame timer,
 * clear the walking flag. */
// @symbol func_ov027_02111ca8
extern "C" int func_ov027_02111ca8(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, data_ov027_02113c84[1], 0, 0x1000, 0);
    *(int *)(c + 0x37c) = 0x1000;
    *(char *)(c + 0x3d8) = 0x14;
    *(int *)(c + 0x3d0) = 0;
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov027_02111c48, 0x02111c48, size 0x60                */
/* ------------------------------------------------------------------------- */
/* State "wait", per frame: once the timer runs out, turn towards the step's
 * next facing and, on arrival, enter the walking state. */
// @symbol func_ov027_02111c48
extern "C" int func_ov027_02111c48(char *c)
{
    if (DecIfAbove0_Byte(c + 0x3d8) == 0) {
        unsigned char idx = *(unsigned char *)(c + 0x3d9);
        short val = data_ov027_02113a1c[idx].nextAngle;
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), val, 0x514) != 0) {
            func_ov027_02111d70(c, 1);
        }
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov027_02111b2c, 0x02111b2c, size 0x11c               */
/* ------------------------------------------------------------------------- */
/* State "walk", on entry: load the current step's distance, speed and facing,
 * then pick the walk or the turn-and-walk animation depending on whether the
 * step turns. */
// @symbol func_ov027_02111b2c
extern "C" void func_ov027_02111b2c(char *c)
{
    *(int *)(c + 0x3d4) = data_ov027_02113a1c[*(unsigned char *)(c + 0x3d9)].distance;
    *(int *)(c + 0x98) = data_ov027_02113a1c[*(unsigned char *)(c + 0x3d9)].speed;
    *(short *)(c + 0x94) = data_ov027_02113a1c[*(unsigned char *)(c + 0x3d9)].angle;
    if (data_ov027_02113a1c[*(unsigned char *)(c + 0x3d9)].angle !=
        data_ov027_02113a1c[*(unsigned char *)(c + 0x3d9)].nextAngle) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, (void *)data_ov027_02113c8c[1], 0, 0x1000, 0);
        *(int *)(c + 0x37c) = 0x1000;
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, (void *)data_ov027_02113c74[1], 0, 0x1000, 0);
        *(int *)(c + 0x37c) = (int)(((s64)data_ov027_02113a1c[*(unsigned char *)(c + 0x3d9)].speed * 0x5000 + 0x800) >> 12);
    }
    *(int *)(c + 0x3d0) = 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov027_02111a28, 0x02111a28, size 0x104               */
/* ------------------------------------------------------------------------- */
/* State "walk", per frame: burn the remaining distance at the step's speed,
 * advance the step index and go back to waiting when it is spent, then move
 * and play a footstep sound on the animation frames that land. */
// @symbol func_ov027_02111a28
extern "C" int func_ov027_02111a28(char *c)
{
    int d = *(int *)(c + 0x3d4);
    if (d == 0) {
        unsigned char *p = (unsigned char *)(((int)c + 0x3d9));
        *p = *p + 1;
        if (*(unsigned char *)(c + 0x3d9) >= 9) *(unsigned char *)(c + 0x3d9) = 0;
        func_ov027_02111d70(c, 0);
        return 1;
    }
    {
        int v = *(int *)(c + 0x98);
        if (d < v) {
            *(int *)(c + 0x98) = d;
            *(int *)(c + 0x3d4) = 0;
        } else {
            int *q = (int *)(((int)c + 0x3d4));
            *q = *q - v;
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (void *)(c + 0x398));
    {
        int t = *(int *)(c + 0x378);
        int w = data_ov027_02113c74[1];
        int field = w ? *(int *)(c + 0x380) : *(int *)(c + 0x380);
        unsigned int id = (unsigned int)(t << 4) >> 0x10;
        if (field == w) {
            if (id == 0xa || id == 0x16) {
                func_0201267c(0xf3, c + 0x74);
            }
        } else if (field == data_ov027_02113c8c[1]) {
            if (id == 9 || id == 0x16) {
                func_0201267c(0xf3, c + 0x74);
            }
        }
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov027_02111994, 0x02111994, size 0x94                */
/* ------------------------------------------------------------------------- */
/* Rebuilds the collision matrix from the actor's facing and eighth-scale
 * position, copies it onto mClsnMat with the full-scale translation, and hands
 * the result to the mesh collider. */
// @symbol func_ov027_02111994
extern "C" void func_ov027_02111994(char *c)
{
    Matrix4x3_FromRotationY(c + 0x33c, *(short *)(c + 0x8e));
    *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x364) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
    *(struct Matrix4x3 *)(c + 0x2ec) = *(struct Matrix4x3 *)(c + 0x33c);
    *(int *)(c + 0x310) = *(int *)(c + 0x5c);
    *(int *)(c + 0x314) = *(int *)(c + 0x60);
    *(int *)(c + 0x318) = *(int *)(c + 0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c + 0x124, c + 0x2ec, *(short *)(c + 0x8e));
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN10daPgDfdr_cD1Ev  0x021118c8  size 0x5c  (complete-object dtor)       */
/*   _ZN10daPgDfdr_cD0Ev  0x02111924  size 0x70  (deleting destructor)        */
/* ------------------------------------------------------------------------- */
/* Defined in include/daPgDfdr_c.h's class body, and deliberately NOT repeated
 * out of line here: out of line mwccarm emits D2, D0, D1, and the cartridge
 * holds D1, D0 with no D2 at all. The body is the own vtable store, then (in
 * reverse declaration order) the destructors of the three members this class
 * declares -- dCcAc_c, TextureSequence, ModelAnim -- then dBgActor_c's inlined
 * teardown. D0 additionally returns the object to its heap through the inline
 * operator delete it inherits from dActor_c, which is why nothing here
 * mentions a heap. */

// @symbol _ZN10daPgDfdr_cD0Ev
// @symbol _ZN10daPgDfdr_cD1Ev
