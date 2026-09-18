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
 * TEXT ONLY. The vtable, the RTTI records, the four SharedFilePtr pairs, the
 * CLPS blocks and the .bss dispatch table are declared here and defined
 * nowhere -- the overlay blob owns every .data/.bss word, and defining any of
 * them would make mwcc emit sections this entry is not allowed to own.
 *
 * SPLIT, not whole: the ROM interleaves this class's state bodies
 * (0x0211c684..0x0211cdec) between the destructors and CleanupResources, and
 * the collision-callback pair (0x0211d270..0x0211d2a0) between InitResources
 * and the factories. One delinks entry licenses one contiguous range, so the
 * class ships as three files -- the destructor pair beside this one, the four
 * factories in d_a_trs_trap_classinits.cpp -- with a manifest entry each. The
 * state bodies belong to a later TU of their own; they are called, never
 * defined, here.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm emits
 * one .text section per function in the REVERSE of source order, so
 * InitResources (highest address) is written FIRST. Do not reorder.
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
 *   and relocation records for all 8 functions.
 * - Behavior names mStateTimer directly at both sites (TRAP-2709
 *   experiment: unified spelling matches).
 * - +0x418 on the spawned BOOK_SWITCH is daBookGen_c's, which has no header;
 *   +0x154 here has no recovered reader. S14: no g_profile_* rows live here.
 */

#include "daTrsTrap_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* The per-trap resource rows, indexed by mIndex: the model handle, the
   collision-mesh handle, the CLPS block, and the bookshelf's six book-switch
   spawn offsets (x/y pairs). Symbols keep their data_ov063_* names; only the
   entry types are recovered. */
extern SharedFilePtr *data_ov063_0211e27c[];
extern SharedFilePtr *data_ov063_0211e28c[];
extern CLPS_Block *data_ov063_0211e9e8[];
extern s32 data_ov063_0211e9f8[];

/* The four dispatch records, bound to this trap's own class (see above). */
typedef void (daTrsTrap_c::*PMF)();
extern PMF data_ov063_0211ef38[];

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region. Everything here is reached from a
 * C++-named member, which cannot declare it in its own body.
 * ------------------------------------------------------------------------ */
extern "C" {
/* This TU's matrix/collision refresh, and the mesh callback it installs. */
void func_ov063_0211c684(char *c);
void func_ov063_0211c6f8(char *c);
void func_ov063_0211d28c(void *a, void *b, void *c);
/* dBgW callback-word stores (see above). */
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
/* dBgW_KcMbg::SetFile, mangled (wall 6az, see above). Spelled exactly as the
   free definition declares it; only the TU-local linkage differs. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *f, const Matrix4x3 *m, int fx, short s, void *b);
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

// @symbol _ZN11daTrsTrap_c16OnPendingDestroyEv
void daTrsTrap_c::OnPendingDestroy()
{
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
