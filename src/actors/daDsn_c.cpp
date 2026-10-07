//cpp
/* daDsn_c -- DOSUN, the Thwomp (actor 161).
 *
 * The dropping leaf of daDsnBase_c. Grindel (daDkk_c, ov025) is the sibling
 * that slides. The base owns the shared cycle -- rise, hover, slam, rest,
 * recover -- plus the drop-shadow update and the Yoshi-egg wake, as seven
 * helpers in src/actors/daDsnBase_c.cpp. This leaf owns the trigger that
 * cuts that cycle short, the texture-frame countdown in front of the rise
 * and the slam, and the mega-character kill.
 *
 * Ordinary functions are written highest ROM address first. mwccarm emits
 * one .text section per function in reverse source order. ~daDsn_c() is
 * inline in include/daDsn_c.h, which is what places D1 then D0 at the
 * bottom of the section list. The registry factory daDsn_c_classInit is
 * the highest address, so it is written first. g_profile_DOSUN stays
 * overlay data.
 *
 * deslop leftovers:
 * - Behavior: `(frame - 1) << 12` is one lsl #12 and the function shrinks
 *   0x1d4 to 0x1d0. The ROM is lsl #16; lsr #4, which is
 *   `(((unsigned)frame - 1) << 16) >> 4`.
 * - Behavior: dBgActor_c::IsClsnInRange as Fix12<int> by value grows it
 *   0x1d4 to 0x1f0 (stack temps). `Fix12<int>{0}` does not compile here
 *   ("( expected"). The scalar (void *, int, int) call with (0, 0) is the
 *   ROM's mov r1, #0; mov r2, r1. dBgActor_c.h does not declare it.
 * - OnHitByMegaChar: `Vector3 poofPosCopy = poofPos` is ldm/stm and shrinks
 *   the function 0x9c to 0x94. The three field stores are the ROM copies.
 * - OnHitByMegaChar: Particle::System::NewSimple as Fix12<int> by value
 *   grows it 0x9c to 0xc0. include/Particle__System.h has no NewSimple.
 *   Scalar ints match r0 = id, r1/r2/r3 = x/y/z.
 * - OnHitByMegaChar: Sound::Play(3, 0x1e, &mCamSpacePosX) grows it 0x9c to
 *   0xa0 and the reloc is _ZN5Sound4PlayEjjRK7Vector3, not func_02012694.
 *   func_02012694 is the bank-3 veneer the ROM calls.
 * - The seven func_ov091_* names stay. They are defined in
 *   src/actors/daDsnBase_c.cpp and daDkk_c calls the same symbols. Four
 *   are daDsnBase_c methods now. Three stay C-linkage.
 */

#include "daDsn_c.h"
#include "Player.h"

/* data_ov091_02135138 is the six-word table daDsnBase_c::Init reads out of
   the pointer this leaf stores:
     +0x00 SharedFilePtr * BMD      +0x0c SharedFilePtr * BTP
     +0x04 SharedFilePtr * KCL      +0x10 shadow extent X
     +0x08 CLPS block (not a file)  +0x14 shadow extent Z
   It is declared int [] as include/decl_common.h has it. The symbol stays
   the overlay's; this TU does not own the bytes.

   func_ov091_02132ff4, func_ov091_02132e98 and func_ov091_02132e64 stay
   C-linkage with a char * parameter. func_ov091_02133098,
   func_ov091_02133020, func_ov091_02132f04 and func_ov091_02132dc0 are
   daDsnBase_c methods. */
extern "C" {
extern int data_ov091_02135138[];
void func_ov091_02132ff4(char *self); /* hover */
void func_ov091_02132e98(char *self); /* rest */
void func_ov091_02132e64(char *self); /* recover */
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_02012694(unsigned int id, const Vector3 *pos);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int radius, int yOffset);
}

/* -------------------------------------------------------------------------- */

// @symbol daDsn_c_classInit
/* Reconstructed source-style name: SM64DS proves daDsn_c through RTTI,
 * allocation size, vtable identity, and the DOSUN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Thwomp_Spawn.
 *
 * The whole body falls out of the one `new`: operator new(932 == 0x3a4),
 * dBgActor_c::C2, the mid-construction daDsnBase_c vptr,
 * TextureSequence@0x324, dExtShadowModel_c@0x338 and this class's vptr are the
 * implicit constructor, inlined. */
extern "C" daDsn_c *daDsn_c_classInit()
{
    return new daDsn_c();
}

// @symbol _ZN7daDsn_c13InitResourcesEv
/* Vtable slot 0. Stores this leaf's file table and starts the cycle at rest.
   data_ov091_02135138 is the table daDsnBase_c::Init reads: model, KCL,
   CLPS, BTP, then the two shadow extents. Overlay .data owns the bytes. */
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
/* Vtable slot 6, and this class's key function.
 *
 * A trigger while the countdown (0) or the hover (1) is running skips
 * straight to the wind-up (2). Otherwise:
 *   0  count the texture frame down, hold, then rise
 *   1  hover
 *   2  play the wind-up; a fresh trigger slams immediately
 *   3  rest, and if it ends while still triggered, stretch the timer
 *   4  recover, same stretch
 * The seven calls are daDsnBase_c's helpers, in that order of the cycle
 * plus the shadow update and the egg-proximity wake. */
int daDsn_c::Behavior()
{
    if (mState < 2) {
        if (mTriggered != 0)
            mState = 2;
    }
    switch (mState) {
    case 0: {
        unsigned short frame = (unsigned short)(mTextureSequence.currFrame >> 12);
        if (frame != 0) {
            /* (frame - 1) << 12, written as << 16 >> 4. One lsl #12 shrinks
               this function (see the file comment). */
            mTextureSequence.currFrame = (int)((((unsigned)frame - 1) << 16) >> 4);
            frame = (unsigned short)(mTextureSequence.currFrame >> 12);
            if (frame == 0)
                mHoldTimer = 0xa;
        } else {
            if (mHoldTimer != 0)
                mHoldTimer--;
            else
                func_ov091_02133020(); /* rise */
        }
        break;
    }
    case 1:
        func_ov091_02132ff4((char *)this); /* hover */
        break;
    case 2:
        if (mTriggered != 0) {
            mTextureSequence.currFrame = 0;
            func_ov091_02132f04(); /* slam */
        } else {
            mTextureSequence.Advance();
            if (mTextureSequence.Finished() != 0) {
                if (mHoldTimer != 0)
                    mHoldTimer--;
                else
                    func_ov091_02132f04(); /* slam */
            } else {
                mHoldTimer = 5;
            }
        }
        break;
    case 3:
        func_ov091_02132e98((char *)this); /* rest */
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
        func_ov091_02132e64((char *)this); /* recover */
        break;
    }
    UpdateModelPosAndRotY();
    func_ov091_02133098(); /* drop shadow */
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0 ||
        func_ov091_02132dc0() != 0) /* Yoshi egg within mClipRadius */
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN7daDsn_c15OnHitByMegaCharER6Player
/* Vtable slot 27. Credit the mega kill, burst particle 0x48 at the
   actor position plus OnAimedAtWithEgg's height (real virtual call),
   poof, remove, and play bank-3 sound 0x1e at the camera-space position.
   func_02012694 is that veneer (src/engine/sound/Sound.cpp -> Sound::Play). */
void daDsn_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Vector3 poofPos;
    poofPos.x = mPosX;
    poofPos.y = mPosY;
    poofPos.z = mPosZ;
    poofPos.y += OnAimedAtWithEgg();
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, poofPos.x, poofPos.y, poofPos.z);
    Vector3 poofPosCopy;
    poofPosCopy.x = poofPos.x;
    poofPosCopy.y = poofPos.y;
    poofPosCopy.z = poofPos.z;
    PoofDustAt(poofPosCopy);
    MarkForDestruction();
    func_02012694(0x1e, (const Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN7daDsn_c16OnAimedAtWithEggEv
/* Vtable slot 29. Constant aim height, 206.0 in 20.12. */
int daDsn_c::OnAimedAtWithEgg()
{
    return 0xce000;
}
