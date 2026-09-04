//cpp
/* Genuine production translation unit for ov027/daIDonketu_c (7 function(s)),
 * enrolled as one `complete` delinks span.
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
 * Assembled from these legacy one-function sources, all now deleted (ROM address
 * order):
 *   [0] 0x021115c4  src/_ZN12daIDonketu_cD1Ev.cpp
 *   [1] 0x02111618  src/_ZN12daIDonketu_cD0Ev.cpp
 *   [2] 0x02111680  src/_ZN12daIDonketu_c14UpdateRunStateEv.cpp
 *   [3] 0x021116f0  src/_ZN12daIDonketu_c16UpdateDeathStateEv.cpp
 *   [4] 0x02111770  src/_ZN12daIDonketu_c8BehaviorEv.cpp
 *   [5] 0x0211181c  src/_ZN12daIDonketu_c13InitResourcesEv.cpp
 *   [6] 0x0211186c  src/daIDonketu_c_Spawn.c
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `ChillBully`, a coined name; the measurement that settles it is at the top of
 * include/daIDonketu_c.h. That rename is what makes this promotion possible at
 * all: a coined class reaches no symbol home, so its _ZTI/_ZTS/_ZTV records can
 * never be word-compared against the cartridge and its key-function TU cannot
 * declare a verifiable compiler_only_output (tools/tubuild.py
 * apply_compiler_only_policy).
 *
 * The bodies below are the recovered C++ ones from the deleted per-function
 * sources. Two things changed with the promotion and neither is cosmetic: the
 * destructor moved inline into the header (see the note where ordinals 0 and 1
 * would be), and the factory's second vtable store now names the table this TU
 * DEFINES rather than the anonymous `data_ov027_02113930` alias.
 */

#include "daIDonketu_c.h"
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"

extern "C" {
extern int _ZTV12daIDonketu_c[];
extern int _ZTV7daOts_c[];
int func_ov064_02116110(char *self, s16 step);
int func_ov064_0211616c(void *self);
int func_ov064_02116d1c(char *c);
int func_ov064_02116ec0(void *);
int _ZN8dActor_c9TrackStarEjj(void *, unsigned int, unsigned int);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daIDonketu_c_Spawn, 0x0211186c, size 0x5c                  */
/* -------------------------------------------------------------------------- */
// @symbol daIDonketu_c_Spawn
/* resolved: VT0 = _ZTV7daOts_c, VT1 = _ZTV12daIDonketu_c.
 *
 * The natural C++ spelling `return new daIDonketu_c` reproduces the text, but
 * relocates the allocation call to the unavailable global `_Znwm` rather than the
 * cartridge's fBase_c::operator new, so the measured C allocator veneer is kept.
 * 1020 = 0x3fc is the size the class header asserts.
 *
 * TWO vptr stores, base then leaf, because the base sub-object is constructed
 * before the complete object -- not one store repeated. The four members
 * constructed between them sit at the offsets daOts_c declares them at.
 *
 * What DID change with the promotion is the second store. This TU now DEFINES the
 * leaf vtable, and mwccarm's `_ZTV12daIDonketu_c` addresses the object's start --
 * the two-word [offset-to-top, typeinfo] preamble at 0x02113928 -- while
 * symbols.txt's symbol at 0x02113930 is already the slot array. Hence `&[2]`.
 * _ZTV7daOts_c is NOT defined here (daOts_c's key function is CleanupResources, in
 * src/_ZN7daOts_c16CleanupResourcesEv.cpp), so that one resolves to symbols.txt's
 * ov064 address and takes no bias. */
extern "C" int *daIDonketu_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1020);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV7daOts_c;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x174);
        _ZN7dCcAc_cC1Ev((char *)p + 0x33c);
        _ZN11ShadowModelC1Ev((char *)p + 0x370);
        p[0] = (int)&_ZTV12daIDonketu_c[2]; /* +8: this TU defines it */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12daIDonketu_c13InitResourcesEv, 0x0211181c, size 0x50  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c13InitResourcesEv
/* daIDonketu_c::InitResources -- vtable slot 0, ov027 0x0211181c.
 *
 * daOts_c leaves slot 0 pure virtual (the word in its vtable is a literal zero), so
 * every one of its three children has to supply this. This one points the shared
 * file table at its own list and then claims a star slot.
 */
int daIDonketu_c::InitResources()
{
    mFileTable = (int)data_ov027_021138f4;
    int r = func_ov064_02116ec0((char *)this);
    mStarIdx = param1 & 0xf;
    mStarSlot = _ZN8dActor_c9TrackStarEjj((char *)this, mStarIdx, 2);
    return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12daIDonketu_c8BehaviorEv, 0x02111770, size 0xac        */
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
 * why mStarSlot is signed. See the long note on that symbol in dActor_c.h.
 */
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
    return func_ov064_02116d1c((char *)this);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12daIDonketu_c16UpdateDeathStateEv, 0x021116f0,         */
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
/* ROM ordinal 2 -- _ZN12daIDonketu_c14UpdateRunStateEv, 0x02111680, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daIDonketu_c14UpdateRunStateEv
/* daIDonketu_c::UpdateRunState -- vtable slot 31, daOts_c's state-1 hook. The
 * charge: hold still while the wind-up plays out over the first ten frames, then
 * run at 0x14000. The pointer returned from the first arm is the cartridge's own
 * shape, not a meaningful value -- r0 is simply left holding the timer address. */
int daIDonketu_c::UpdateRunState()
{
    if (*(u16 *)&mStateTimer < 0xa) {
        mHorzSpeed = 0;
        int result = func_ov064_02116110((char *)this, 0x700);
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
/* ROM ordinals 0 and 1 -- _ZN12daIDonketu_cD1Ev 0x021115c4 size 0x54 and      */
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
