//cpp
/* Genuine production translation unit for ov027/daPgDfdr_c (15 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder; see
 * notes/tu-reconstruction-pilot-report.md sec 3 for the one documented exception
 * (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM address
 * order):
 *   [0] 0x021118c8  src/_ZN10daPgDfdr_cD1Ev.cpp
 *   [1] 0x02111924  src/_ZN10daPgDfdr_cD0Ev.cpp
 *   [2] 0x02111994  src/func_ov027_02111994.cpp
 *   [3] 0x02111a28  src/func_ov027_02111a28.c
 *   [4] 0x02111b2c  src/func_ov027_02111b2c.cpp
 *   [5] 0x02111c48  src/func_ov027_02111c48.c
 *   [6] 0x02111ca8  src/func_ov027_02111ca8.cpp
 *   [7] 0x02111cfc  src/func_ov027_02111cfc.cpp
 *   [8] 0x02111d38  src/func_ov027_02111d38.cpp
 *   [9] 0x02111d70  src/func_ov027_02111d70.c
 *   [10] 0x02111d8c  src/_ZN10daPgDfdr_c16CleanupResourcesEv.cpp
 *   [11] 0x02111dfc  src/_ZN10daPgDfdr_c16OnPendingDestroyEv.cpp
 *   [12] 0x02111e00  src/_ZN10daPgDfdr_c6RenderEv.cpp
 *   [13] 0x02111e34  src/_ZN10daPgDfdr_c8BehaviorEv.cpp
 *   [14] 0x02111eb4  src/_ZN10daPgDfdr_c13InitResourcesEv.cpp
 *
 * WHAT CHANGED WITH THE PROMOTION, and neither change is cosmetic:
 *
 *  1. THE DESTRUCTOR MOVED INLINE INTO include/daPgDfdr_c.h. The two shards each
 *     carried their own out-of-line `daPgDfdr_c::~daPgDfdr_c() {}`; one TU cannot
 *     carry both, and writing it out-of-line once makes mwccarm emit D0 before
 *     D1 -- the reverse of the cartridge's 0x021118c8 (D1) then 0x02111924 (D0)
 *     -- plus a third D2 body with no ROM home. The inline definition gives the
 *     retail D1/D0 pair in ROM order and no D2. InitResources, the first
 *     DECLARED non-inline virtual, then becomes this class's key function, so
 *     this TU still emits the whole _ZTV/_ZTI/_ZTS group licensed in
 *     config/tu_manifest.d/ov027/daPgDfdr_c.json.
 *
 *  2. NINE SYMBOLS THE SHARDS SPELLED TWO WAYS NOW HAVE ONE SPELLING. Each of
 *     the eight file-local helpers came from a separate file with its own
 *     stand-in declarations, and merging them makes the disagreements real
 *     compile errors: func_ov027_02111d70 was `(char*,int)` in one file and
 *     `(int*,int)` in another (include/decl_common.h's `(void*,int)` settles
 *     it), _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj was declared with two
 *     different second parameters, and the data symbols were char / void* /
 *     struct-typed by turns. The spellings kept below are the ones whose address
 *     arithmetic is unchanged; every call site was adjusted with a cast rather
 *     than by changing what is computed.
 */

/* common.h MUST COME FIRST. It and math/Matrix.h both define Matrix4x3, both
 * 0x30 bytes, and whichever a TU sees first stands (see the guard note at the
 * top of include/common.h). daPgDfdr_c.h reaches math/Matrix.h through Model.h,
 * whose spelling is `Matrix3x3 r; Vector3 t;`; func_ov027_02111994 copies the
 * whole matrix in one assignment, and against the NESTED spelling mwccarm splits
 * that copy into a 9-word ldm/stm run plus a separate 3-word tail it then CSEs
 * against the three `>> 3` stores just above it -- 0x94 -> 0xac, six
 * instructions long. common.h's flat `s32 m[12]` restores the cartridge's three
 * flat 4-word ldm/stm pairs. This one line is the difference between
 * 14/15 and 15/15. */
#include "common.h"
#include "daPgDfdr_c.h"
#include "decl_Player.h"
#include "decl_common.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

/* Opaque file types, named only so the calls below are readable. */
struct BMD_File;
struct BTP_File;

/* The runtime state record mStateTable (+0x3cc) points into. */
typedef struct Elem {
    int a, b, c, d;
} Elem;

/* Stand-in for the class as the two state-dispatch thunks see it: they reach
 * mStateTable at +0x3cc and call through the pointer-to-member it holds. The
 * callee's real function-pointer type is not proven, so the shape stays a
 * stand-in rather than moving into the header. */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x3cc]; PMF *pp; };

/* Stand-in for ModelAnim's own vtable, whose slot 5 is Render:
 * include/ModelAnim.h does not declare it callable. */
struct Sub {
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4(); virtual void m(int);
};

extern "C" {
void Matrix4x3_FromRotationY(void *, short);
int DecIfAbove0_Byte(void *p);
int _Z14ApproachLinearRsss(short *r, short t, short s);
void func_0201267c(int a, char *b);
void func_020393d4(void *p, int v);
void _ZN13SharedFilePtr7ReleaseEv(void *);
void _ZN5dCc_c5ClearEv(void *a);
void _ZN5dCc_c6UpdateEv(void *a);
void _ZN9Animation7AdvanceEv(void *a);
void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *o, void *f, int a, int b);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned int c);
void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *o, void *f, int i, int fx, unsigned j);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *o, void *kcl, void *m, int fx, short s, void *clps);
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *, void *, short);
void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *o, void *act, int a, int b, unsigned c, unsigned d);
void _ZN8dActor_c9UpdatePosEP5dCc_c(char *c, void *cyl);
void *_ZN8dActor_c13ClosestPlayerEv(void *c);
void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *t, int a, int b);

/* This TU's own file-local helpers, forward declared in ROM address order.
 * func_ov027_02111cfc and func_ov027_02111d70 are NOT declared here: they are
 * already in include/decl_common.h, as `(void*)` and `(void*,int)`, and those
 * two declarations are what fixed the parameter spellings below. */
void func_ov027_02111994(char *c);
int func_ov027_02111a28(char *c);
void func_ov027_02111b2c(char *c);
int func_ov027_02111c48(char *c);
int func_ov027_02111ca8(char *c);
void func_ov027_02111d38(void *cv);

/* SharedFilePtr slots, as bare bytes: InitResources reaches the BMD_File* and
 * BTP_File* at +4 inside two of them, which only a char-typed symbol spells the
 * same way the ROM does. */
extern char data_ov027_02113c6c;
extern char data_ov027_02113c7c;
extern char data_ov027_02113c94;
extern void *data_ov027_02113c84[];
extern int data_ov027_02113c74[];
extern int data_ov027_02113c8c[];

/* One stride-0xc per-step table at 0x02113a1c, reached through four aliasing
 * symbols -- +0, +4, +8 and +10 of the same record. */
extern char data_ov027_02113a1c[];
extern char data_ov027_02113a20[];
extern char data_ov027_02113a24[];
extern char data_ov027_02113a26[];

extern Elem data_ov027_02113ce4[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN10daPgDfdr_c13InitResourcesEv, 0x02111eb4, size 0x1c8  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c13InitResourcesEv
/* daPgDfdr_c::InitResources -- vtable slot 0, and THE KEY FUNCTION of this
 * class: the destructor is inline in the header, so the first declared
 * non-inline virtual is what makes this TU emit the _ZTV/_ZTI/_ZTS group.
 *
 * It loads the model into mModelAnim, its three animations and its BTP texture
 * sequence, hard-codes the object's world position and facing, hangs the KCL on
 * the mesh collider with dBgW::UpdatePosAndAngs as the update hook, disables
 * gravity (mVertAccel and mTerminalVelocity both zero), starts the actor
 * collider, and finally drops the object onto whatever ground a raycast 0x14000
 * above it finds -- falling back to that raised point when nothing is hit.
 *
 * dBgW_KcMbg::SetFile, TextureSequence::SetFile and dCcAc_c::Init all carry
 * Fix12<int> BY VALUE in their mangled names (wall 6az), so all three keep
 * scalar extern "C" declarations rather than becoming callable methods. */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN10daPgDfdr_c8BehaviorEv, 0x02111e34, size 0x80         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c8BehaviorEv
/* daPgDfdr_c::Behavior -- vtable slot 6.
 *
 * The two Animation::Advance calls are the same call on two different
 * sub-objects, which the raw offsets hid: `c + 0x370` is the Animation BASE of
 * mModelAnim (0x320 + 0x50, the multiple-inheritance offset include/ModelAnim.h
 * documents), and `c + 0x384` is mTextureSequence, whose Animation base sits at
 * offset 0. Spelling both as a base-pointer conversion is what makes that
 * readable -- and the compiler computes the same two addresses. */
s32 daPgDfdr_c::Behavior()
{
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    func_ov027_02111cfc(this);
    if (_ZN6Player16IsInsideOfCannonEv(_ZN8dActor_c13ClosestPlayerEv(this))) {
        mFlags &= ~2;
    } else {
        mFlags |= 2;
    }
    _ZN9Animation7AdvanceEv(static_cast<Animation*>(&mModelAnim));
    _ZN9Animation7AdvanceEv(static_cast<Animation*>(&mTextureSequence));
    _ZN5dCc_c5ClearEv(&mdCcAc_c);
    _ZN5dCc_c6UpdateEv(&mdCcAc_c);
    func_ov027_02111994((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN10daPgDfdr_c6RenderEv, 0x02111e00, size 0x34           */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c6RenderEv
/* daPgDfdr_c::Render -- vtable slot 9. The call to the model's own Render still
 * goes through the `Sub` stand-in -- but through &mModelAnim rather than through
 * a 0x320-byte pad. The TextureSequence update takes mModelAnim's
 * ModelComponents, which is what `this + 0x328` was (0x320 + Model's own +0x08). */
s32 daPgDfdr_c::Render()
{
    _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mModelAnim.data);
    Sub *b = (Sub *)&mModelAnim; b->m(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN10daPgDfdr_c16OnPendingDestroyEv, 0x02111dfc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c16OnPendingDestroyEv
/* daPgDfdr_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot, so the base's own handler does not
 * run for this class. */
void daPgDfdr_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN10daPgDfdr_c16CleanupResourcesEv, 0x02111d8c, 0x70     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
/* daPgDfdr_c::CleanupResources -- vtable slot 3. Releases five shared files --
 * one, then the three-entry table, then another -- and takes the inherited mesh
 * collider out of the collision world in between, before the last release. The
 * order is the ROM's. */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov027_02111d70, 0x02111d70, size 0x1c                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111d70
/* Enter a state: point mStateTable at entry `idx` of the runtime table, then run
 * that entry's first callback immediately. */
extern "C" void func_ov027_02111d70(void *self, int idx)
{
    *(Elem **)((char *)self + 0x3cc) = &data_ov027_02113ce4[idx];
    func_ov027_02111d38(self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov027_02111d38, 0x02111d38, size 0x38                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111d38
/* Call mStateTable's entry [0] -- the state's enter hook -- through `this`. */
extern "C" void func_ov027_02111d38(void *cv)
{
    C *c = (C *)cv;
    PMF *p = c->pp;
    (c->**p)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov027_02111cfc, 0x02111cfc, size 0x3c                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111cfc
/* Call mStateTable's entry [1] -- the state's per-frame hook -- through `this`. */
extern "C" void func_ov027_02111cfc(void *cv)
{
    C *c = (C *)cv;
    PMF *p = c->pp + 1;
    (c->**p)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov027_02111ca8, 0x02111ca8, size 0x54                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111ca8
/* Turn-in-place state, enter hook: play the turning animation at full speed and
 * arm mTimer for 20 frames. */
extern "C" int func_ov027_02111ca8(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, data_ov027_02113c84[1], 0, 0x1000, 0);
    *(int *)(c + 0x37c) = 0x1000;
    *(char *)(c + 0x3d8) = 0x14;
    *(int *)(c + 0x3d0) = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov027_02111c48, 0x02111c48, size 0x60                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111c48
/* Turn-in-place state, per-frame hook: once mTimer has run out, ease mAngleY
 * towards this step's target angle and, on arrival, enter state 1. */
extern "C" int func_ov027_02111c48(char *c)
{
    if (DecIfAbove0_Byte(c + 0x3d8) == 0) {
        unsigned char idx = *(unsigned char *)(c + 0x3d9);
        short val = *(short *)(data_ov027_02113a26 + idx * 0xc);
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), val, 0x514) != 0) {
            func_ov027_02111d70(c, 1);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov027_02111b2c, 0x02111b2c, size 0x11c                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111b2c
/* Walk state, enter hook: load this step's distance, speed and heading out of
 * the stride-0xc table, then pick the walk animation -- the turning one, at full
 * speed, when this step's heading differs from the one it eases towards,
 * otherwise the walking one at a speed scaled from the step's own. */
extern "C" void func_ov027_02111b2c(char *c)
{
    *(int *)(c + 0x3d4) = *(int *)(data_ov027_02113a1c + *(unsigned char *)(c + 0x3d9) * 0xc);
    *(int *)(c + 0x98) = *(int *)(data_ov027_02113a20 + *(unsigned char *)(c + 0x3d9) * 0xc);
    *(short *)(c + 0x94) = *(short *)(data_ov027_02113a24 + *(unsigned char *)(c + 0x3d9) * 0xc);
    if (*(short *)(data_ov027_02113a24 + *(unsigned char *)(c + 0x3d9) * 0xc) != *(short *)(data_ov027_02113a26 + *(unsigned char *)(c + 0x3d9) * 0xc)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, (void *)data_ov027_02113c8c[1], 0, 0x1000, 0);
        *(int *)(c + 0x37c) = 0x1000;
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, (void *)data_ov027_02113c74[1], 0, 0x1000, 0);
        *(int *)(c + 0x37c) = (int)(((s64) * (int *)(data_ov027_02113a20 + *(unsigned char *)(c + 0x3d9) * 0xc) * 0x5000 + 0x800) >> 12);
    }
    *(int *)(c + 0x3d0) = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov027_02111a28, 0x02111a28, size 0x104                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111a28
/* Walk state, per-frame hook: spend mDistanceLeft at mHorzSpeed a frame; when it
 * runs out, advance mStepIndex (wrapping at 9) and re-enter state 0. While it
 * lasts, move the actor and fire a footstep sound on the two animation frames
 * that carry one, per which of the two walk animations is playing. */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov027_02111994, 0x02111994, size 0x94                 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111994
/* Rebuild the collision matrix from mAngleY and the actor's position, copy it
 * into mClsnMat, and hand it to the mesh collider. The three `>> 3` stores are
 * the scratch matrix's own translation; the unscaled position goes into the copy.
 * Every access stays at its raw offset: this helper reaches fields through a
 * `char *` and reconciling those to member names moves the field-address CSE
 * (notes/agents/roles/writer.md). */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN10daPgDfdr_cD1Ev 0x021118c8 size 0x5c and        */
/* _ZN10daPgDfdr_cD0Ev 0x02111924 size 0x70 -- are NOT written here.           */
/*                                                                            */
/* The destructor is defined INLINE in include/daPgDfdr_c.h; see change (1) in */
/* this file's header for why. One vtable store and three member teardowns     */
/* come out of that one empty body: its own vptr, then -- in reverse           */
/* declaration order -- dCcAc_c at +0x398, TextureSequence at +0x384 and       */
/* ModelAnim at +0x320, then dBgActor_c's own teardown inlined (its destructor */
/* is defined in its class body too), which is dBgW_KcMbg at +0x124 and Model  */
/* at +0x0d4, then dActor_c. D0 is that plus the inherited inline `operator    */
/* delete` found by ordinary lookup on dActor_c two levels up; slot 17 is the  */
/* deleting variant.                                                          */
/* -------------------------------------------------------------------------- */
