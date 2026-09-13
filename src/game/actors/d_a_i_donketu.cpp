//cpp
/* Genuine production translation unit for ov027/daIDonketu_c (7 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * Chill Bully (CHILL_BULLY 217) -- ov027/daIDonketu_c. ov027 is mixed
 * (SLIDING_ICE_SPAWNER / SLIDING_ICE / CHILL_BULLY / BUBBA / SNOWMAN_BREATH).
 * RTTI names this class daIDonketu_c; the debug table names ICE_DONKETU;
 * overlay_actors.md's project name is CHILL_BULLY. This is the chill bully,
 * not Bubba / sliding ice / penguin.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daIDonketu_c -> d_a_i_donketu.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder; see
 * notes/tu-reconstruction-pilot-report.md sec 3 for the one documented exception
 * (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * order):
 *   [0] 0x021115c4  src/_ZN12daIDonketu_cD1Ev.cpp
 *   [1] 0x02111618  src/_ZN12daIDonketu_cD0Ev.cpp
 *   [2] 0x02111680  src/_ZN12daIDonketu_c14UpdateRunStateEv.cpp
 *   [3] 0x021116f0  src/_ZN12daIDonketu_c16UpdateDeathStateEv.cpp
 *   [4] 0x02111770  src/_ZN12daIDonketu_c8BehaviorEv.cpp
 *   [5] 0x0211181c  src/_ZN12daIDonketu_c13InitResourcesEv.cpp
 *   [6] 0x0211186c  src/daIDonketu_c_classInit.c
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `ChillBully`, a coined name; the measurement that settles it is at the top of
 * include/daIDonketu_c.h.
 *
 * deslop leftovers:
 * - func_ov064_02116ec0 (InitResources), func_ov064_02116d1c (Behavior),
 *   func_ov064_0211616c (UpdateDeathState), func_ov064_02116110 (UpdateRunState):
 *   unmatched ov064 daOts_c helpers; name_evidence none.
 * - data_ov027_021138f4 file-table handle: this TU claims .text only.
 * - S14: g_profile_ICE_DONKETU stays outside the licensed .text.
 * - UpdateRunState keeps `*(u16 *)&mStateTimer`: named s16 vs u16* is a
 *   signedness/addressing wall (blo vs blt).
 * - no Player.h
 */

#include "daIDonketu_c.h"

extern "C" {
int func_ov064_02116110(void *self, s16 step);
int func_ov064_0211616c(void *self);
int func_ov064_02116d1c(void *self);
int func_ov064_02116ec0(void *self);
extern int data_ov027_021138f4[];
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daIDonketu_c_classInit
/* Reconstructed source-style name: SM64DS proves daIDonketu_c through RTTI,
 * allocation size, vtable identity, and the ICE_DONKETU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daIDonketu_c_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111870 loads 1020 = 0x3fc -- the class's own size -- into the header's
 * leaf operator new; 0x02111880 calls dEnemyBase_c's C2; the store at
 * 0x0211188c lays down daOts_c's vptr; the four member C1s at 0x110 / 0x174 /
 * 0x33c / 0x370 run between the two vptr stores; the store at 0x021118bc lays
 * down this class's vptr at &[2] because this TU defines the table. The null
 * check is the one `new` itself emits. The implicit constructor keeps the
 * intervening daOts_c construction inlined; declaring a constructor of our
 * own would emit a `bl` the factory does not have. */
extern "C" daIDonketu_c *daIDonketu_c_classInit(void)
{
    return new daIDonketu_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c13InitResourcesEv
/* daIDonketu_c::InitResources -- vtable slot 0, ov027 0x0211181c.
 *
 * daOts_c leaves slot 0 pure virtual (the word in its vtable is a literal zero), so
 * every one of its three children has to supply this. This one points the shared
 * file table at its own list and then claims a star slot. */
int daIDonketu_c::InitResources()
{
    mFileTable = (int)data_ov027_021138f4;
    int r = func_ov064_02116ec0(this);
    mStarIdx = param1 & 0xf;
    mStarSlot = TrackStar(mStarIdx, 2);
    return r;
}

/* -------------------------------------------------------------------------- */
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
int daIDonketu_c::Behavior()
{
    int r = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 0);
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
    return func_ov064_02116d1c(this);
}

/* -------------------------------------------------------------------------- */
/* size 0x80                                                                   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c16UpdateDeathStateEv
/* daIDonketu_c::UpdateDeathState -- vtable slot 32, daOts_c's state-4 hook. The
 * same star drop as Behavior's kill path without the invincible-character test: by
 * the time state 4 runs the death is already decided. */
void daIDonketu_c::UpdateDeathState()
{
    int result = func_ov064_0211616c(this);
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c14UpdateRunStateEv
/* daIDonketu_c::UpdateRunState -- vtable slot 31, daOts_c's state-1 hook. The
 * charge: hold still while the wind-up plays out over the first ten frames, then
 * run at 0x14000. The pointer returned from the first arm is the cartridge's own
 * shape, not a meaningful value -- r0 is simply left holding the timer address.
 *
 * MATCH wall: mStateTimer is s16 in dEnemyBase_c.h. A named halfword compare
 * is signed (blt); the ROM's first arm is unsigned (blo). Keep the u16*
 * addressing. */
int daIDonketu_c::UpdateRunState()
{
    if (*(u16 *)&mStateTimer < 0xa) {
        mHorzSpeed = 0;
        int result = func_ov064_02116110(this, 0x700);
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

/* -------------------------------------------------------------------------- */
/* _ZN12daIDonketu_cD0Ev 0x02111618 size 0x68 -- are NOT written here.         */
/*                                                                            */
/* The destructor is defined INLINE in include/daIDonketu_c.h. Written         */
/* out-of-line here the real destructor makes mwccarm emit D0 BEFORE D1, the   */
/* reverse of the cartridge's order, which objisolate refuses for the whole    */
/* translation unit, and it emits a third D2 body with no ROM home. This       */
/* entry's own pre-promotion verification block recorded both, as `ordinal     */
/* pair(s) not in ROM order: [(0, 1)]` and an unhandled _ZN12daIDonketu_cD2Ev  */
/* in compilerOnlyOutput. The inline definition gives the retail D1/D0 pair in */
/* ROM order and no D2, while Behavior -- declared out-of-line and first in    */
/* the class body -- keeps this TU as the class's key-function TU, so it still */
/* owns the complete _ZTV/_ZTI/_ZTS group declared in this entry's             */
/* compiler_only_output.                                                      */
/*                                                                            */
/* TWO vptr stores and four member destructor calls come out of that one empty */
/* body: its own vptr, then daOts_c's -- inlined, because that destructor is   */
/* defined in its class body -- then ShadowModel 0x370, dCcAc_c 0x33c,         */
/* dBgCh_Actr 0x174 and ModelAnim 0x110 in reverse declaration order, then     */
/* dEnemyBase_c. daIDonketu_c adds no member with a destructor of its own,     */
/* only two bytes. D0 is that plus the inherited inline `operator delete`;     */
/* slot 17 is the deleting variant. Byte-for-byte the same shape as Bully's    */
/* and BigBully's, which is what three siblings sharing a base look like.      */
/* -------------------------------------------------------------------------- */
