//cpp
/* ov013/daObjClock_c -- reconstructed translation unit (9 functions).
 *
 * ROM run 0x021113bc..0x021116ac, plus the class's .data run at
 * 0x021121a4..0x0211227c (_ZTI, _ZTS, the g_profile_CLOCK_LONG and
 * g_profile_CLOCK_SHORT descriptors, and the _ZTV whose address point is
 * 0x02112200). ov013 is the clock painting overlay (not TTC / ov065):
 * CLOCK_PAINTING_HAND_SHORT (292) and CLOCK_PAINTING_HAND_LONG (293) share
 * this class; CLOCK_PAINTING_PENDULUM (294) is the sibling daObjClockHuriko_c.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder; the
 * destructor pair at the end is the one documented exception, because the
 * compiler picks the order inside a D0/D1 group itself.
 *
 * Members, in ROM address order (each was a one-function shard before this
 * TU took over the run; the shards are gone):
 *   [0] 0x021113bc  daObjClock_c::~daObjClock_c   (D1)
 *   [1] 0x021113ec  daObjClock_c::~daObjClock_c   (D0)
 *   [2] 0x02111430  func_ov013_02111430           (state helper)
 *   [3] 0x02111478  daObjClock_c::CleanupResources
 *   [4] 0x021114a4  daObjClock_c::Render
 *   [5] 0x021114cc  daObjClock_c::Behavior
 *   [6] 0x021115cc  daObjClock_c::InitResources
 *   [7] 0x0211163c  daObjClock_c_classInit_CLOCK_SHORT   (factory, actor 292)
 *   [8] 0x02111674  daObjClock_c_classInit_CLOCK_LONG    (factory, actor 293)
 *
 * Leftover: measured under mwccarm 2004/b56, and left in the form that matches.
 * Folding actorID == 0x125 into the if: InitResources 0x70 -> 0x64.
 * A ternary, or mHandIndex = actorID != 0x125: InitResources 0x70 -> 0x60.
 * The widened int isLongHand stays.
 * Lower-bound-first quadrant compares: Behavior stays 0x100 and 21 words differ.
 * Dropping the repeated lower bounds: Behavior 0x100 -> 0xe0.
 * Folding the two stopped quadrants into one test: Behavior 0x100 -> 0xf8.
 * mAngleZ = mAngleZ + step: Behavior 0x100 -> 0xfc.
 * Testing mHandIndex before IsAreaShowing: Behavior stays 0x100 and 5 words differ.
 * Assigning a Vector3 to mModel.mat4x3.t: func_ov013_02111430 0x48 -> 0x50.
 * The three translation stores stay.
 * data_ov013_021116ac and data_ov013_021116b0 keep linker names. This TU
 * reads them; it does not own that .data. The two factories stay
 * CLOCK_SHORT / CLOCK_LONG, because one C name cannot cover both.
 * Matrix4x3_FromRotationZXYExt stays a call. No free function defined in
 * this TU takes the clock; func_ov013_02111430 is already the method.
 */

#include "daObjClock_c.h"
#include "SharedFilePtr.h"

/* Model handles construct through func_02017acc and destroy through
 * func_02017ab4. The manifest aliases those undefined members onto the ROM
 * symbols. */
struct ClockModelFilePtr : SharedFilePtr {
    u32 words[2];

    ClockModelFilePtr(u32 fileID);
    ~ClockModelFilePtr();
};

extern "C" {
extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
int IsAreaShowing(int areaId);
extern s8 data_02092110;             /* current level id; hands advance while it is not positive */
extern u8 data_0209f2c0;              /* clock setting: 0 slow, 1 fast, 2 random, 3 stopped */
extern s16 data_ov013_021116ac[];
extern SharedFilePtr *data_ov013_021116b0[];
extern ClockModelFilePtr data_ov013_02112294;
extern ClockModelFilePtr data_ov013_0211229c;
}

enum {
    ACTOR_CLOCK_SHORT = 0x124,
    ACTOR_CLOCK_LONG = 0x125
};

enum {
    CLOCK_SETTING_SLOW = 0,
    CLOCK_SETTING_FAST = 1,
    CLOCK_SETTING_RANDOM = 2,
    CLOCK_SETTING_STOPPED = 3
};

/* ONE CLASS, TWO PROFILES. Both factories install the same vtable and the
 * same 0x128 allocation. They cannot share one C name: the EAD spelling
 * would be daObjClock_c_classInit for both. CLOCK_SHORT at 0x02090230 and
 * CLOCK_LONG at 0x0208ffa8 are the ROM debug strings. */
// @symbol daObjClock_c_classInit_CLOCK_LONG
extern "C" daObjClock_c *daObjClock_c_classInit_CLOCK_LONG()
{
    return new daObjClock_c();
}

// @symbol daObjClock_c_classInit_CLOCK_SHORT
extern "C" daObjClock_c *daObjClock_c_classInit_CLOCK_SHORT()
{
    return new daObjClock_c();
}

/* 0x1c actor descriptor. Source order is CLOCK_LONG (0x021121c0) then
 * CLOCK_SHORT (0x021121dc). +4/+6 are the halfwords fBase_c passes to the
 * behavior and render priority setters; the s16 type is the storage, not
 * a recovered signedness. */
struct ClockSpawnInfo {
    daObjClock_c *(*classInit)();
    s16 profileID;
    s16 drawOrder;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;                 /* 0x1000 == 1.0 */
    s32 clipDistance;
    s32 farDistance;
};
typedef char ClockSpawnInfo_size_must_be_0x1c[sizeof(ClockSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_CLOCK_LONG
extern "C" ClockSpawnInfo g_profile_CLOCK_LONG = {
    daObjClock_c_classInit_CLOCK_LONG, ACTOR_CLOCK_LONG, 0x00a3, 0x00000006,
    0x00064000, 0x000fa000, 0x00c80000, 0x00640000
};

// @symbol g_profile_CLOCK_SHORT
extern "C" ClockSpawnInfo g_profile_CLOCK_SHORT = {
    daObjClock_c_classInit_CLOCK_SHORT, ACTOR_CLOCK_SHORT, 0x00a2, 0x00000006,
    0x00064000, 0x000fa000, 0x00c80000, 0x00640000
};

// @symbol _ZN12daObjClock_c13InitResourcesEv
/* Hand 0 is the long hand, hand 1 the short. The actorID test stays in an
 * int: folding it into the if sizes this 0x64 against 0x70, and a ternary
 * or mHandIndex = actorID != 0x125 sizes it 0x60. */
int daObjClock_c::InitResources()
{
    int isLongHand = actorID == ACTOR_CLOCK_LONG;
    if (isLongHand)
        mHandIndex = 0;
    else
        mHandIndex = 1;
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov013_021116b0[mHandIndex]), 1, -1);
    func_ov013_02111430();
    return 1;
}

// @symbol _ZN12daObjClock_c8BehaviorEv
/* While the level id is not positive the hand advances by its per-hand
 * speed. Otherwise the long hand, when its area is showing, writes the
 * clock setting from the quadrant of -mAngleZ. Upper bound first is the
 * match: lower-bound-first compares are 21 words off at the same 0x100.
 * Dropping the repeated lower bounds sizes this 0xe0. Folding the two
 * stopped quadrants sizes it 0xf8. mAngleZ = mAngleZ + step sizes it 0xfc.
 * Testing mHandIndex before IsAreaShowing is 5 words off. */
int daObjClock_c::Behavior()
{
    if (data_02092110 <= 0) {
        mAngleZ += data_ov013_021116ac[mHandIndex];
    } else if (IsAreaShowing(mAreaId) && mHandIndex == 0) {
        u16 angle = -mAngleZ;
        if (angle < 0x2000)
            data_0209f2c0 = CLOCK_SETTING_STOPPED;
        else if (angle < 0x6000 && angle >= 0x2000)
            data_0209f2c0 = CLOCK_SETTING_SLOW;
        else if (angle < 0xa000 && angle >= 0x6000)
            data_0209f2c0 = CLOCK_SETTING_RANDOM;
        else if (angle < 0xe000 && angle >= 0xa000)
            data_0209f2c0 = CLOCK_SETTING_FAST;
        else if (angle >= 0xe000)
            data_0209f2c0 = CLOCK_SETTING_STOPPED;
    }
    func_ov013_02111430();
    return 1;
}

// @symbol _ZN12daObjClock_c6RenderEv
int daObjClock_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN12daObjClock_c16CleanupResourcesEv
int daObjClock_c::CleanupResources()
{
    data_ov013_021116b0[mHandIndex]->Release();
    return 1;
}

// @symbol _ZN12daObjClock_c19func_ov013_02111430Ev
/* Rebuild mModel.mat4x3 from the actor angles and from position >> 3.
 * Assigning a Vector3 to mat4x3.t sizes this 0x50 against 0x48.
 * The marker above is not decoration -- without it tools/tiers.py folds this
 * body into the preceding member's fragment. */
void daObjClock_c::func_ov013_02111430()
{
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

// @symbol _ZN12daObjClock_cD1Ev
// @symbol _ZN12daObjClock_cD0Ev
/* Both destructors come from the inline `~daObjClock_c() {}` in the header.
 * Out of line, mwccarm emits D0 before D1 (objisolate refuses the TU) and a
 * D2 the cartridge has no home for.
 */

/* File-scope objects: the short hand's model file (0x5b2) and the long
 * hand's model file (0x5b1); the two constructions
 * __sinit_daObjClock_c.cpp performs, in retail order. */
ClockModelFilePtr data_ov013_0211229c(0x5b1);
ClockModelFilePtr data_ov013_02112294(0x5b2);
