//cpp
/* daDsn_c -- DOSUN, the Thwomp (ov091, 0x02132938..0x02132cb8).
 *
 * The dropping leaf of daDsnBase_c; its sibling is daDkk_c, the sliding
 * Grindel in ov025. The base owns the shared 0..4 cycle -- rise, hover, slam,
 * rest, recover -- as seven C-linkage helpers in src/actors/daDsnBase_c.cpp;
 * this class owns only the trigger logic that decides when to drop, the file
 * table it hands to daDsnBase_c::Init, and what happens when a mega character
 * flattens it.
 *
 * mwccarm emits ordinary functions in reverse source order, so the definitions
 * below run from the highest retail address back down. The destructor pair is
 * written by nobody: include/daDsn_c.h defines ~daDsn_c() in the class body,
 * and that alone makes mwccarm emit D1 (0x02132938) then D0 (0x02132998) at the
 * bottom of the section list -- the cartridge's own order. See the header for
 * why the in-class form is load-bearing.
 *
 * The factory, daDsn_c_classInit at 0x02132cb8, directly follows this range
 * and still lives in src/d_a_dsn.c.
 *
 * Folded from six one-function sources, one per member (ROM address order):
 *   [0] 0x02132938  D1                  (now the header's inline body)
 *   [1] 0x02132998  D0                  (now the header's inline body)
 *   [2] 0x02132a0c  OnAimedAtWithEgg
 *   [3] 0x02132a14  OnHitByMegaChar
 *   [4] 0x02132ab0  Behavior
 *   [5] 0x02132c84  InitResources
 */

#include "daDsn_c.h"
#include "decl_common.h"
#include "Player.h"

/* ABI seam: everything here still answers to its linker name, because no
   header declares it in a form this TU can call. Kept above the first
   @symbol marker so no member is charged with the spellings.

   func_ov091_* are daDsnBase_c's shared state steps. They keep ROM labels
   and C linkage because daDkk_c::Behavior (ov025) calls the same seven by
   name across the module boundary. */
extern "C" {
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_02012694(unsigned int id, const Vector3 *pos);
void func_ov091_02133020(char *c);
void func_ov091_02132ff4(char *c);
void func_ov091_02132f04(char *c);
void func_ov091_02132e98(char *c);
void func_ov091_02132e64(char *c);
void func_ov091_02133098(char *c);
int func_ov091_02132dc0(char *c);
void _ZN9Animation7AdvanceEv(void *self);
int _ZN9Animation8FinishedEv(void *self);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
}

/* -------------------------------------------------------------------------- */

// @symbol _ZN7daDsn_c13InitResourcesEv
/* Vtable slot 0, which daDsnBase_c leaves pure. Hands the base this leaf's
 * file table, runs the shared setup, and starts the cycle at rest. */
int daDsn_c::InitResources()
{
    mFileTable = (int)data_ov091_02135138;
    int result = Init();
    mState = 0;
    mHoldTimer = 0;
    mTriggered = 0;
    return result;
}

// @symbol _ZN7daDsn_c8BehaviorEv
/* Vtable slot 6, which daDsnBase_c leaves pure, and this class's ABI key
 * function. A trigger while resting or rising sends the Thwomp straight to the
 * slam; otherwise each state runs its base-class step.
 *
 * Animation::currFrame is reached as a raw 20.12 word. The ROM shifts it right
 * by 12 to read and left by 12 to write, in the same function that calls
 * Animation::Advance on the same sub-object, so it is the member and not four
 * bytes of daDsn_c -- see include/daDsn_c.h. It is spelled through a cast
 * rather than `mTextureSequence.currFrame.val` because mwccarm 2004/b56
 * rejects the member chain through the Fix12<int> template with an expression
 * syntax error; the bare member and the cast both compile, and only the cast
 * takes the shifts. */
int daDsn_c::Behavior()
{
    if (mState < 2) {
        if (mTriggered != 0)
            mState = 2;
    }
    switch (mState) {
    case 0: {
        unsigned short frame = (unsigned short)((*(s32 *)&mTextureSequence.currFrame) >> 12);
        if (frame != 0) {
            (*(s32 *)&mTextureSequence.currFrame) = (int)((((unsigned)frame - 1) << 16) >> 4);
            frame = (unsigned short)((*(s32 *)&mTextureSequence.currFrame) >> 12);
            if (frame == 0) {
                mHoldTimer = 0xa;
            }
        } else {
            if (mHoldTimer != 0) {
                mHoldTimer--;
            } else {
                func_ov091_02133020((char *)this);
            }
        }
        break;
    }
    case 1:
        func_ov091_02132ff4((char *)this);
        break;
    case 2:
        if (mTriggered != 0) {
            (*(s32 *)&mTextureSequence.currFrame) = 0;
            func_ov091_02132f04((char *)this);
        } else {
            _ZN9Animation7AdvanceEv((char *)&mTextureSequence);
            if (_ZN9Animation8FinishedEv((char *)&mTextureSequence) != 0) {
                if (mHoldTimer != 0) {
                    mHoldTimer--;
                } else {
                    func_ov091_02132f04((char *)this);
                }
            } else {
                mHoldTimer = 5;
            }
        }
        break;
    case 3:
        func_ov091_02132e98((char *)this);
        if (mState == 4) {
            if (mTriggered != 0) {
                mRetrigger = 0x5a;
                mTriggered = 0;
            }
        }
        break;
    case 4:
        if (mTriggered != 0) {
            mTriggered = 0;
            mRetrigger = 0x5a;
        }
        func_ov091_02132e64((char *)this);
        break;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv((char *)this);
    func_ov091_02133098((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_((char *)this, 0, 0) == 0) {
        if (func_ov091_02132dc0((char *)this) == 0)
            goto done;
    }
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv((char *)this);
done:
    return 1;
}

// @symbol _ZN7daDsn_c15OnHitByMegaCharER6Player
/* Vtable slot 27. A mega character walks through the Thwomp: credit the kill,
 * burst particle 0x48 and a dust poof at the Thwomp's position lifted by
 * OnAimedAtWithEgg()'s height -- called through the real virtual dispatch --
 * and remove it. mPosX/mPosY/mPosZ are dActor_c's, inherited. */
void daDsn_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Vector3 poofPos;
    poofPos.x = mPosX;
    poofPos.y = mPosY;
    poofPos.z = mPosZ;
    int height = OnAimedAtWithEgg();
    poofPos.y += height;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, poofPos.x, poofPos.y, poofPos.z);
    /* Copied word by word on purpose: `Vector3 poofPosCopy = poofPos;`
       changes the size of this function (measured; the bytes differ). */
    Vector3 poofPosCopy;
    ((int *)&poofPosCopy)[0] = ((int *)&poofPos)[0];
    ((int *)&poofPosCopy)[1] = ((int *)&poofPos)[1];
    ((int *)&poofPosCopy)[2] = ((int *)&poofPos)[2];
    PoofDustAt(poofPosCopy);
    MarkForDestruction();
    func_02012694(0x1e, (const Vector3 *)((char *)this + 0x74));
}

// @symbol _ZN7daDsn_c16OnAimedAtWithEggEv
/* Vtable slot 29. A constant: the height (20.12 fixed point, 206.0)
 * OnHitByMegaChar adds to the dust-poof position. */
int daDsn_c::OnAimedAtWithEgg()
{
    return 843776;
}
