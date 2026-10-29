//cpp
/* daIDonketu_c -- Chill Bully (ICE_DONKETU, ov027), child of the daOts_c
 * Bully base in ov064. Own resources, own run/death states; otherwise the
 * base's common workers. _ZTS/_ZTV prove the class name; the ICE_DONKETU
 * profile proves the registry slot. The tree used to call it `ChillBully`,
 * which is coined.
 *
 * Two load-bearing spellings, both measured:
 *
 *   Function order is the REVERSE of the ROM's -- mwccarm emits one .text
 *   section per function in reverse source order. Do not reorder (the
 *   destructor D0/D1/D2 group is the only exception).
 *
 *   UpdateRunState reads its timer as `*(u16 *)&mStateTimer`: the member
 *   is s16 and the ROM compares unsigned (blo, not blt).
 *
 * Notes:
 * - The TU is one `complete` delinks span claiming .text only, so the
 *   file-table handle data_ov027_021138f4 and g_profile_ICE_DONKETU stay
 *   outside it.
 * - func_ov064_02116110 (run) and func_ov064_0211616c (death) are daOts_c
 *   workers in ov064; they keep their labels with that class.
 * - D1/D0 are not written here: the inline destructor in the header
 *   emits them in cartridge order with no homeless D2. Behavior, the
 *   first declared virtual, keeps this TU the key-function TU, so it
 *   still owns the _ZTV/_ZTI/_ZTS group.
 * - The SharedFilePtr subclasses stay declared-only wrappers: they let
 *   the sinit name the ROM's SharedFilePtr veneer entry points without
 *   inventing a layout.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_i_donketu_AssetId {
    ASSET_ICE_DONKETU_BMD = 0x02e4,
    ASSET_ICE_DONKETU_DEAD_BCA = 0x02e5,
    ASSET_ICE_DONKETU_HIT_BCA = 0x02e6,
    ASSET_ICE_DONKETU_TURN_BCA = 0x02e7,
    ASSET_ICE_DONKETU_WALK_BCA = 0x02e8
};

#include "daIDonketu_c.h"
#include "SharedFilePtr.h"

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the four
 * animation handles), spelled through declared-only subclasses so the
 * static initializer names the real entry points. The code reaches them
 * through the data_ov027_021138f4 pointer table. */
struct IDonketuModelFilePtr : SharedFilePtr {
    IDonketuModelFilePtr(u32 fileID);
    ~IDonketuModelFilePtr();
};
struct IDonketuAnimationFileHandle : SharedFilePtr {
    IDonketuAnimationFileHandle(u32 fileID);
    ~IDonketuAnimationFileHandle();
};

extern int data_ov027_021138f4[];

/* -------------------------------------------------------------------------- */
// @symbol daIDonketu_c_classInit
/* Factory: `return new` emits the size (0x3fc), the base C2, the member
 * C1s and both vptr stores. Reconstructed source-style name: SM64DS proves
 * daIDonketu_c through RTTI, allocation size, vtable identity, and the
 * ICE_DONKETU registry profile; later EAD lineage supplies classInit.
 * Exact original spelling is not preserved. Historical alias:
 * daIDonketu_c_Spawn. */
extern "C" daIDonketu_c *daIDonketu_c_classInit(void) {
    return new daIDonketu_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c13InitResourcesEv
/* daIDonketu_c::InitResources -- vtable slot 0, ov027 0x0211181c.
 *
 * daOts_c leaves slot 0 pure virtual (the word in its vtable is a literal zero), so
 * every one of its three children has to supply this. This one points the shared
 * file table at its own list and then claims a star slot. */
int daIDonketu_c::InitResources() {
    mFileTable = (int)data_ov027_021138f4;
    int r = InitResourcesCommon();
    mStarIdx = param1 & 0xf;
    mStarSlot = TrackStar(mStarIdx, 2);
    return r;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c8BehaviorEv
/* daIDonketu_c::Behavior -- vtable slot 6, ov027 0x02111770, and THE KEY FUNCTION
 * of this class: the destructor is inline in the header, so the first declared
 * non-inline virtual is what makes this TU emit the _ZTV/_ZTI/_ZTS group.
 *
 * daOts_c leaves slot 6 pure virtual, so each of its three children supplies its
 * own. This one is the star-carrying one: if an invincible character killed it, it
 * poofs, drops its star a little above itself, and marks itself for destruction;
 * otherwise it falls through to the shared behaviour in ov064.
 *
 * Every call below is a real method, so the compiler mangles the names and the
 * signatures become falsifiable -- including UntrackAndSpawnStar's `s8 &`, which is
 * why mStarSlot is signed. See the long note on that symbol in dActor_c.h. */
int daIDonketu_c::Behavior() {
    int r = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 0);
    if (r != 0) {
        if (r == 2) {
            Vector3 v;
            TriplePoofDust();
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + 0x64000;
            UntrackAndSpawnStar(mStarSlot, (mStarIdx | 0x40) & 0xff, v, 4);
            MarkForDestruction();
        }
        return 1;
    }
    return BehaviorCommon();
}

// @symbol _ZN12daIDonketu_c16UpdateDeathStateEv
/* daIDonketu_c::UpdateDeathState -- vtable slot 32, daOts_c's state-4 hook. The
 * same star drop as Behavior's kill path without the invincible-character test: by
 * the time state 4 runs the death is already decided. */
void daIDonketu_c::UpdateDeathState() {
    int result = func_ov064_0211616c();
    if (result == 0)
        return;
    TriplePoofDust();
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x64000;
    UntrackAndSpawnStar(mStarSlot, (mStarIdx | 0x40) & 0xff, pos, 4);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c14UpdateRunStateEv
/* daIDonketu_c::UpdateRunState -- vtable slot 31, daOts_c's state-1 hook. The
 * charge: hold still while the wind-up plays out over the first ten frames, then
 * run at 0x14000. The pointer returned from the first arm is the cartridge's own
 * shape, not a meaningful value -- r0 is simply left holding the timer address.
 * mStateTimer is s16 in dEnemyBase_c.h: a named halfword compare is signed
 * (blt) where the ROM's first arm is unsigned (blo), so the u16* addressing
 * stays. */
int daIDonketu_c::UpdateRunState() {
    if (*(u16 *)&mStateTimer < 0xa) {
        mHorzSpeed = 0;
        int result = func_ov064_02116110(0x700);
        if (result != 0)
            return result;
        u16 *timer = (u16 *)&mStateTimer;
        if (*timer == 9)
            *timer = 0;
        return (int)timer;
    }
    mHorzSpeed = 0x14000;
    u16 *timer = (u16 *)&mStateTimer;
    u32 value = *timer;
    if (value > 0x23) {
        value = 0;
        *timer = 0;
    }
    return value;
}
// @symbol _ZN12daIDonketu_cD1Ev
// @symbol _ZN12daIDonketu_cD0Ev
/* D1/D0 emit from the header's inline destructor: written out-of-line here
 * it would produce D0 before D1 and a homeless D2. The empty body still
 * chains two vptr stores and the member destructor calls
 * (dExtShadowModel_c, dCcAc_c, dBgCh_Actr, dExtAnmModel_c, then dEnemyBase_c);
 * D0 adds the inherited inline operator delete. */

/* The Chill Bully's model and animation handles, in the sinit call order the
 * data_ov027_021138f4 file table repeats. */
IDonketuModelFilePtr gIceDonketuModelFile(ASSET_ICE_DONKETU_BMD);
IDonketuAnimationFileHandle gIceDonketuDeadAnimationFile(ASSET_ICE_DONKETU_DEAD_BCA);
IDonketuAnimationFileHandle gIceDonketuHitAnimationFile(ASSET_ICE_DONKETU_HIT_BCA);
IDonketuAnimationFileHandle gIceDonketuTurnAnimationFile(ASSET_ICE_DONKETU_TURN_BCA);
IDonketuAnimationFileHandle gIceDonketuWalkAnimationFile(ASSET_ICE_DONKETU_WALK_BCA);
