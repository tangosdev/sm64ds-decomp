//cpp
/* Big Boo's Haunt animated-furniture traps -- ov063/daTrsTrap_c.
 *
 * One class, four registry profiles: the rising staircase (KAIDAN 0x41), the
 * tilting trapdoor (TERESAPIT 0xa0), the sliding bookshelf (BOOKSHELF 0x9e)
 * and the spinning merry-go-round (MERRYGOROUND 0x9f). InitResources reads the
 * profile's actor id back as mIndex, which selects the model/collision/CLPS
 * row and the per-frame state body. The staircase master spawns its two
 * fellow steps, the bookshelf its three book-switches (BOOK_SWITCH 0xd5); the
 * trapdoor and the merry-go-round react to the player standing on the mesh.
 *
 * This file is the whole linker unit 0x0211c600..0x0211d270, 16 functions:
 *   0x0211c600  D1, D0 (the out-of-line destructor, the key function)
 *   0x0211c684  func_ov063_0211c684  model matrix from position and angles
 *   0x0211c6f8  func_ov063_0211c6f8  collision matrix, and the mesh Transform
 *   0x0211c770  func_ov063_0211c770  staircase settle step (per-frame table)
 *   0x0211c7b0  func_ov063_0211c7b0  merry-go-round state  (mIndex 3)
 *   0x0211c82c  func_ov063_0211c82c  merry-go-round occupancy
 *   0x0211c89c  func_ov063_0211c89c  bookshelf state       (mIndex 2)
 *   0x0211cae8  func_ov063_0211cae8  book-switch callback (ORs mBookFlags)
 *   0x0211cb54  func_ov063_0211cb54  trapdoor state       (mIndex 1)
 *   0x0211cc18  func_ov063_0211cc18  staircase rise state (mIndex 0)
 *   0x0211cdec  CleanupResources, OnPendingDestroy, Render, Behavior,
 *               InitResources
 * Below it, daTrs_c's TU ends at 0x0211c600. Above it, the collision-callback
 * pair at 0x0211d270 opens the factory unit (d_a_trs_trap_classinits.cpp).
 * The four state bodies are daTrsTrap_c members; the .bss dispatch table
 * data_ov063_0211ef38 at the bottom of this file binds them.
 *
 * The statics live here too: the eight file handles (four model, four
 * collision) and the dispatch table are defined at the bottom of this file,
 * so mwcc emits __sinit_d_a_trs_trap.cpp (the ROM's 0x0211e3cc initializer)
 * and its .ctor entry. Defining the destructor out of line makes this TU
 * emit the vtable and the RTTI chain; the manifest licenses them as
 * deadstrip-data at their ROM homes.
 *
 * `#pragma defer_codegen off` makes source order the emission order, so the
 * file runs in ROM order, destructor first.
 *
 * deslop leftovers:
 * - dBgW_KcMbg::SetFile keeps its mangled TU-local spelling: its signature
 *   carries Fix12<int> by value (wall 6az), so the method form size-DIFFs.
 * - func_020393d4/func_020393c4 install the BeforeClsn/unk_1c callback words;
 *   dBgW carries no setter, and dBgW.h itself blesses this spelling.
 * - SharedFilePtr+4 is the loaded KCL file: the handle's layout is still
 *   unrecovered (SharedFilePtr.h), and the ROM re-reads the word rather than
 *   keeping dBgW_Kc::LoadFile's return.
 * - The dispatch table is a pointer-to-member of daTrsTrap_c itself. An
 *   earlier draft bound it to an opaque forward-declared class on the theory
 *   that a polymorphic receiver changes the {ptr, adj} record shape; that is
 *   false here -- under 2004/b56 the real class gives byte-identical bodies
 *   and relocation records.
 * - The four state bodies are members under their func_ov063_* names (the
 *   dispatch table's PMF records need member functions); the matrix refreshes,
 *   the settle step, the merry-go-round occupancy test and the book-switch
 *   callback keep C linkage and the byte-matching char-offset bodies they
 *   matched with as separate files.
 * - Behavior names mStateTimer directly at both sites (TRAP-2709
 *   experiment: unified spelling matches).
 * - +0x418 on the spawned BOOK_SWITCH is daBookGen_c's, which has no header;
 *   +0x154 here has no recovered reader. S14: no g_profile_* rows live here.
 */

/* common.h first, so this TU reads the flat Matrix4x3 words: the two matrix
   refreshes copy data_020a0e68 into the object with one ldm/stm block move,
   which the structured Matrix3x3 + Vector3 spelling scalarizes (0x90 and
   0x8c bytes against the ROM's 0x74 and 0x78). */
#include "common.h"
#include "daTrsTrap_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"

#pragma defer_codegen off

/* The per-trap resource rows, indexed by mIndex: the model handle, the
   collision-mesh handle, the CLPS block, and the bookshelf's six book-switch
   spawn offsets (x/y pairs). Symbols keep their data_ov063_* names; only the
   entry types are recovered. */
extern SharedFilePtr *data_ov063_0211e27c[];
extern SharedFilePtr *data_ov063_0211e28c[];
extern CLPS_Block *data_ov063_0211e9e8[];
extern s32 data_ov063_0211e9f8[];

/* The four dispatch records, bound to this trap's own class (see above).
   Defined at the bottom of this file. */
typedef void (daTrsTrap_c::*PMF)();
extern PMF data_ov063_0211ef38[];

/* 8-byte file handles. The models use func_02017acc / func_02017ab4 and the
 * collision files func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct TrsTrapModelFilePtr : SharedFilePtr {
    u32 words[2];

    TrsTrapModelFilePtr(u32 fileID);
    ~TrsTrapModelFilePtr();
};

struct TrsTrapCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    TrsTrapCollisionFilePtr(u32 fileID);
    ~TrsTrapCollisionFilePtr();
};

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region.
 * ------------------------------------------------------------------------ */
extern "C" {
/* This TU's matrix/collision refresh and the occupancy test, called before
   their definitions; the mesh callback lives in the factory unit. */
void func_ov063_0211c684(char *c);
void func_ov063_0211c6f8(char *c);
void func_ov063_0211c82c(char *c);
void func_ov063_0211d28c(dBgW *clsn, daTrsTrap_c *self, dActor_c *actor);
/* dBgW callback-word stores (see above). */
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
/* dBgW_KcMbg::SetFile, mangled (wall 6az, see above). Spelled exactly as the
   free definition declares it; only the TU-local linkage differs. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *f, const Matrix4x3 *m, int fx, short s, void *b);
/* dBgW_KcMbg::Transform: dBgW_KcMbg.h notes it keeps a free definition. */
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *self, const Matrix4x3 &mtx, short s);

extern Matrix4x3 data_020a0e68;             /* the shared scratch matrix */
/* Staircase settle offsets, one signed byte per frame 0..3. */
extern signed char data_ov063_0211e26c[];
/* Staircase rise limits, indexed by mStepIndex. */
extern int data_ov063_0211e270[];

void Vec3_Asr(void *dst, void *src, int n);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
u8 IsAreaShowing(int idx);
unsigned int func_02012790(unsigned int);
}

/* Out of line, and FIRST in this ROM-ascending file: that puts D1 and D0 at
 * 0x0211c600 / 0x0211c638, the two lowest addresses in the unit, and makes
 * this TU the one that emits the vtable. mwcc also emits D2, which the ROM
 * does not have; the manifest licenses it as deadstrip. */
// @symbol _ZN11daTrsTrap_cD1Ev
// @symbol _ZN11daTrsTrap_cD0Ev
daTrsTrap_c::~daTrsTrap_c()
{
}

/* The model matrix: position (>> 3) and the three angles, into
   mModel.mat4x3 at 0xf0. Called from Behavior. */
// @symbol func_ov063_0211c684
extern "C" void func_ov063_0211c684(char *c)
{
    int v[3];
    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short *)(c + 0x8c), *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(Matrix4x3 *)(c + 0xf0) = data_020a0e68;
}

/* The collision matrix mClsnMat from position and angles, then the moving
   mesh follows it. Called from InitResources and Behavior. */
// @symbol func_ov063_0211c6f8
extern "C" void func_ov063_0211c6f8(char *c)
{
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short *)(c + 0x8c), *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(Matrix4x3 *)(c + 0x324) = data_020a0e68;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(c + 0x15c, *(Matrix4x3 *)(c + 0x324), *(short *)(c + 0x8e));
}

/* Staircase settle: frames 0..3 nudge mPosY by the signed table entry
   (<< 12); any later frame reports done. */
// @symbol func_ov063_0211c770
extern "C" int func_ov063_0211c770(unsigned char *c, int idx)
{
    if ((unsigned int)idx > 3 || (short)idx < 0)
        return 1;
    (*(int *)(((int)c + 0x60))) += data_ov063_0211e26c[idx] << 12;
    return 0;
}

/* Merry-go-round state: spins by mAngVelY while mState is 0, keeps the
   occupancy flag and the looping sound going. */
// @symbol _ZN11daTrsTrap_c19func_ov063_0211c7b0Ev
void daTrsTrap_c::func_ov063_0211c7b0()
{
    short *t;
    if (mState != 0) {
        mAngVelY = 0;
        return;
    }
    mAngVelY = 0x80;
    t = &mAngleY;
    *t = (short)(*t + mAngVelY);
    func_ov063_0211c82c((char *)this);
    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8f, *(Vector3 *)&mCamSpacePosX, 0);
}

/* Merry-go-round occupancy. The first call only latches mInitLatch; after
   that mOccupiedFlag is set while a rider stands on the mesh or the area is
   showing. */
// @symbol func_ov063_0211c82c
extern "C" void func_ov063_0211c82c(char *c)
{
    if (*(unsigned char *)(c + 0x152) == 0) {
        *(unsigned char *)(((int)c + 0x152)) += 1;
        return;
    }
    if (*(int *)(c + 0x124) != 0 || IsAreaShowing(*(signed char *)(c + 0xcc))) {
        *(unsigned char *)(c + 0x153) = 1;
    } else {
        *(unsigned char *)(c + 0x153) = 0;
    }
}

/* Bookshelf state. Hidden (mVisible clear) once the player is past the
   wall; state 0 waits for all three book-switches, 1 plays the secret chime,
   2 slides the shelf, 3 finishes the chime and removes the shelf. */
// @symbol _ZN11daTrsTrap_c19func_ov063_0211c89cEv
void daTrsTrap_c::func_ov063_0211c89c()
{
    Vector3 pp;
    char *p;

    mAreaId = ~0;
    if (IsAreaShowing(mSavedAreaId) == 0) {
        mBookFlags = 0;
        mAreaId = mSavedAreaId;
        return;
    }
    p = (char *)ClosestPlayer();
    {
        int *q = (int *)(p + 0x5c);
        pp.x = q[0];
        pp.y = q[1];
        pp.z = q[2];
    }
    if (pp.x >= (int)0xffa24000)
        mVisible = 0;
    else
        mVisible = 1;

    switch (mState) {
    case 0:
        if ((mBookFlags & 8) == 0 &&
            Vec3_Dist((Vector3 *)&mPosX, &pp) < 0x320000) {
            short a = Vec3_HorzAngle((Vector3 *)&mPosX, &pp);
            if (a <= -0x7000 || a >= 0x7000)
                mBookFlags |= 8;
        }
        {
            u8 m = mBookFlags & 7;
            if (m == 7) {
                mState = 1;
                return;
            }
            if (m == 1) return;
            if (m == 3) return;
            mBookFlags &= 8;
        }
        return;
    case 1:
        {
            u16 t = mStateTimer;
            if (t > 0x64) {
                mState = 2;
                return;
            }
            if (t < 0x1e) return;
            Sound::PlaySecretSound(this, &mSoundTimer);
        }
        return;
    case 2:
        mPosX += 0x5000;
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x8d, *(Vector3 *)&mCamSpacePosX, 0);
        Sound::PlaySecretSound(this, &mSoundTimer);
        if (mStateTimer > 0x65)
            mState = 3;
        return;
    case 3:
        if (Sound::PlaySecretSound(this, &mSoundTimer) == 0)
            return;
        MarkForDestruction();
        return;
    }
}

/* Book-switch callback: ORs switch bit m into mBookFlags, then the right or
   wrong chime -- the order is 1, then 1|2, then all three. */
// @symbol func_ov063_0211cae8
extern "C" void func_ov063_0211cae8(unsigned char *c, int m)
{
    unsigned char *p = (unsigned char *)(c + 0x157);
    *p |= m;
    unsigned char v = c[0x157] & 7;
    if (v != 1 && v != 3 && v != 7) {
        func_02012790(0xe);
        return;
    }
    func_02012790(0x26);
}

/* Trapdoor state. While an actor stands on it, mAngVelX tilts toward the
   rider's z offset; with nobody on it the door swings back and stops at
   level. The tilt is applied twice in the standing case, as in the ROM. */
// @symbol _ZN11daTrsTrap_c19func_ov063_0211cb54Ev
void daTrsTrap_c::func_ov063_0211cb54()
{
    if (mStandingActor != 0) {
        mAngVelX = (s16)((mStandingActor->mPosZ - mPosZ) >> 0xc);
        *(s16 *)((int)this + 0x8c) = *(s16 *)((int)this + 0x8c) + mAngVelX;
    } else {
        int a = mAngleX;
        if (a < 0) a = -a;
        if (a < 0xbb8 || mStateTimer > 0xf) {
            int v;
            mAngVelX = 0;
            v = mAngleX;
            if (v > 0) {
                if (v < 0xc8) mAngleX = 0;
                else mAngVelX = -0xc8;
            } else {
                if (v > -0xc8) mAngleX = 0;
                else mAngVelX = 0xc8;
            }
        }
    }
    *(s16 *)((int)this + 0x8c) = *(s16 *)((int)this + 0x8c) + mAngVelX;
}

/* Staircase rise state. Waits until the master step (or this one, for the
   master) is released; then rises 8 per frame to this step's limit, settles
   through func_ov063_0211c770, and the last step plays the secret chime. */
// @symbol _ZN11daTrsTrap_c19func_ov063_0211cc18Ev
void daTrsTrap_c::func_ov063_0211cc18()
{
    int limit;
    daTrsTrap_c *other;
    unsigned char *pst;
    int *p60;
    int *p144;

    limit = data_ov063_0211e270[mStepIndex];
    other = (daTrsTrap_c *)dActor_c::FindWithID(mParentUniqueID);
    if (other != 0) {
        if (other->mTriggered == 0)
            return;
        mVisible = 1;
    } else {
        if (mTriggered == 0)
            return;
        mVisible = 1;
    }

    switch (mState) {
    case 0:
        mPosY = mHomePosY;
        mRiseProgress = 0;
        pst = &mState;
        *pst = (unsigned char)(*pst + 1);
        /* fallthrough */
    case 1:
        p60 = &mPosY;
        p144 = &mRiseProgress;
        *p60 = *p60 + 0x8000;
        *p144 = *p144 + 8;
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x82, *(Vector3 *)&mCamSpacePosX, 0);
        if (mRiseProgress <= limit)
            return;
        mRiseProgress = limit;
        mPosY = mHomePosY + (limit << 12);
        pst = &mState;
        *pst = (unsigned char)(*pst + 1);
        return;
    case 2:
        if (mStateTimer == 0)
            Sound::PlayBank3(0x3d, *(Vector3 *)&mCamSpacePosX);
        if (func_ov063_0211c770((unsigned char *)this, mStateTimer) != 0) {
            pst = &mState;
            *pst = (unsigned char)(*pst + 1);
        }
        return;
    case 3:
        if (mStepIndex == 2) {
            if (Sound::PlaySecretSound(this, &mSoundTimer) != 0) {
                pst = &mState;
                *pst = (unsigned char)(*pst + 1);
            }
        } else {
            pst = &mState;
            *pst = (unsigned char)(*pst + 1);
        }
        return;
    case 4:
    default:
        return;
    }
}

// @symbol _ZN11daTrsTrap_c16CleanupResourcesEv
/* Vtable slot 3. Unhooks the moving mesh and releases this trap's model and
 * collision rows. */
int daTrsTrap_c::CleanupResources()
{
    mMovingMeshCollider.Disable();
    data_ov063_0211e27c[mIndex]->Release();
    data_ov063_0211e28c[mIndex]->Release();
    return 1;
}

// @symbol _ZN11daTrsTrap_c16OnPendingDestroyEv
void daTrsTrap_c::OnPendingDestroy()
{
}

// @symbol _ZN11daTrsTrap_c6RenderEv
/* Vtable slot 9. Draws the model unless this trap is parked invisible (an
 * untriggered step, or the bookshelf while the player is past the wall). */
int daTrsTrap_c::Render()
{
    if (mVisible == 0)
        return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN11daTrsTrap_c8BehaviorEv
/* Vtable slot 6. One call through this trap's state body every frame, then
 * the model/collision refresh and the standing-actor clear. mStateTimer
 * counts frames in the state and restarts whenever the body moves on. */
int daTrsTrap_c::Behavior()
{
    unsigned char before = mState;
    int idx = mIndex;
    (this->*data_ov063_0211ef38[idx])();
    /* Count frames in state; restart on transition. */
    u16 *ctr = &mStateTimer;
    *ctr = *ctr + 1;
    if (before != mState) {
        mStateTimer = 0;
    }
    func_ov063_0211c684((char *)this);
    func_ov063_0211c6f8((char *)this);
    mStandingActor = 0;
    return 1;
}

// @symbol _ZN11daTrsTrap_c13InitResourcesEv
/* Vtable slot 0. KAIDAN's master step (step index 0) drops into place and
 * spawns fellow steps 1 and 2; every trap then snapshots its home position,
 * loads its model/collision row, seats the moving-mesh collider with the
 * UpdatePosAndAngs callback, and -- for the bookshelf -- spawns its three
 * book-switches along the offset table.
 *
 * Two shapes in the spawn loop's preheader are load-bearing for the byte
 * match, not style (notes/mwccarm-codegen.md 6v):
 *   - the offset table is referenced by SYMBOL at the use site, never through
 *     a named local. Naming it fixes its colour but makes LICM hoist it to the
 *     front of the preheader; unnamed, LICM hoists it in first-use order and
 *     lands it where the ROM has it.
 *   - v16's three fields are written x, y, z in that order; any other order
 *     swaps the two strh at +0x2a0/+0x2a4. */
int daTrsTrap_c::InitResources()
{
    char *c = (char *)this;
    int is41;
    int t;
    u16 actorType;
    s32 param;
    s32 dx;
    int i;
    int bit;
    Vector3 pos;
    void *sp;

    param = param1;
    mStepIndex = ((u32)param >> 8) & 3;
    mParentUniqueID = 0;
    is41 = 1;
    mVisible = is41;
    actorType = actorID;
    if (actorType != 0x41)
        is41 = 0;
    if (is41 != false) {
        mIndex = 0;
        if (mStepIndex == 0) {
            bit = param1 & 1;
            dx = (bit ^ 1) * 0xc8000;
            if (bit != false)
                mVisible = 0;
            mPosY = mPosY - 0x258000 + dx;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            for (i = 1; i <= 2; i++) {
                pos.z -= 0xc8000;
                pos.y += dx;
                sp = dActor_c::Spawn(0x41, 0x100 * i, pos, 0, mAreaId, -1);
                ((daTrsTrap_c *)sp)->mParentUniqueID = uniqueID;
                ((daTrsTrap_c *)sp)->mVisible = mVisible;
            }
        }
    } else {
        t = (int)(actorType == 0xa0);
        if (t != false) {
            mIndex = 1;
        } else {
            t = (int)(actorType == 0x9e);
            if (t != false) {
                mIndex = 2;
            } else {
                mIndex = 3;
            }
        }
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mState = 0;
    mStandingActor = 0;
    mInitLatch = 0;
    mOccupiedFlag = 0;
    *(u8 *)(c + 0x154) = 0;
    mTriggered = 0;
    mSoundHandle = 0;
    mSoundTimer = 0;

    dBgW_Kc::LoadFile(*data_ov063_0211e28c[mIndex]);
    if (mModel.SetFile((BMD_File *)Model::LoadFile(
            *data_ov063_0211e27c[mIndex]), 1, -1) == 0)
        return 0;

    func_ov063_0211c6f8(c);

    {
        int m = mIndex;
        if (m == 3) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMovingMeshCollider, *(void **)((char *)data_ov063_0211e28c[m] + 4), &mClsnMat,
                0x1000, mAngleY, data_ov063_0211e9e8[m]);
        } else {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMovingMeshCollider, *(void **)((char *)data_ov063_0211e28c[m] + 4), &mClsnMat,
                0x199, mAngleY, data_ov063_0211e9e8[m]);
        }
    }

    func_020393d4((int *)&mMovingMeshCollider, (int)&dBgW::UpdatePosAndAngs);
    func_020393c4((int *)&mMovingMeshCollider, (int)&func_ov063_0211d28c);
    mMovingMeshCollider.Enable(this);

    if (mIndex == 2) {
        Vector3 base; Vector3_16 v16;
        int j, even, odd, actorId, neg1;
        base.x = mHomePosX;
        base.y = mHomePosY;
        j = 0;
        base.z = mHomePosZ;
        v16.x = 0;
        even = j;
        v16.y = -0x8000;
        v16.z = 0;
        odd = 1;
        actorId = 0xd5;
        neg1 = -1;
        for (; j < 3; j++) {
            base.x = mHomePosX - data_ov063_0211e9f8[even];
            base.y = mHomePosY + data_ov063_0211e9f8[odd];
            base.z = mHomePosZ - 0xb4000;
            sp = dActor_c::Spawn(actorId, j, base, &v16, mAreaId, neg1);
            *(s32 *)((char *)sp + 0x418) = uniqueID;
            even += 2; odd += 2;
        }
        mSavedAreaId = mAreaId;
    }
    mBookFlags = 0;
    mAngVelX = 0;
    mAngVelY = 0;
    mAngVelZ = 0;
    return 1;
}

/* Source order is construction order: the eight file handles first (the two
 * resource tables data_ov063_0211e27c/e28c index them per variant), then the
 * dispatch records. __sinit_d_a_trs_trap.cpp emits the constructions, the
 * destructor registrations and the record copies; the registration nodes are
 * compiler temporaries. */
TrsTrapModelFilePtr data_ov063_0211eeb8(0x6bb);
TrsTrapModelFilePtr data_ov063_0211eeb0(0x6bd);
TrsTrapModelFilePtr data_ov063_0211ee98(0x6c1);
TrsTrapModelFilePtr data_ov063_0211eea8(0x6c3);
TrsTrapCollisionFilePtr data_ov063_0211eec0(0x6bc);
TrsTrapCollisionFilePtr data_ov063_0211eea0(0x6be);
TrsTrapCollisionFilePtr data_ov063_0211eec8(0x6c2);
TrsTrapCollisionFilePtr data_ov063_0211eed0(0x6c4);

PMF data_ov063_0211ef38[] = {
    &daTrsTrap_c::func_ov063_0211cc18,
    &daTrsTrap_c::func_ov063_0211cb54,
    &daTrsTrap_c::func_ov063_0211c89c,
    &daTrsTrap_c::func_ov063_0211c7b0,
};
