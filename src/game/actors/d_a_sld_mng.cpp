//cpp
/* Genuine production translation unit for ov019/daSldMng_c (5 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daSldMng_c -> d_a_sld_mng.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0] 0x0211261c  src/_ZN10daSldMng_cD1Ev.cpp
 *   [1] 0x02112640  src/_ZN10daSldMng_cD0Ev.cpp
 *   [2] 0x02112678  src/_ZN10daSldMng_c8BehaviorEv.cpp
 *   [3] 0x0211271c  src/_ZN10daSldMng_c13InitResourcesEv.cpp
 *   [4] 0x0211274c  src/daSldMng_c_Spawn.c
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `IceSlideManager`, a coined name. MEASURED in
 * extracted/overlays/overlay_0019.bin (ov019 base 0x021111a0, the `.text
 * start:` on line 1 of config/arm9/overlays/ov019/delinks.txt): the vtable
 * object's preamble at 0x021133c4 is [offset-to-top 0, 0x0211338c], and
 * _ZTI10daSldMng_c at 0x0211338c reads [0x0209a764, 0x02113398, 0x0208e390]
 * -- _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), a typeinfo
 * name at 0x02113398 whose bytes are the string "10daSldMng_c", and
 * _ZTI8dActor_c, so the cartridge states the direct base too.
 * config/arm9/overlays/ov019/symbols.txt already carried _ZTI10daSldMng_c and
 * _ZTS10daSldMng_c at those two addresses while every function symbol and the
 * vtable were still spelled IceSlideManager; tools/class_rename.py closed that
 * gap, and it is what makes this promotion possible at all: a coined class
 * reaches no symbol home, so its _ZTI/_ZTS/_ZTV records can never be
 * word-compared against the cartridge and its key-function TU cannot declare a
 * verifiable compiler_only_output (tools/tubuild.py
 * apply_compiler_only_policy).
 *
 * The shadow TU this replaces suppressed the flat header with
 * `#define DASLDMNG_C_H` and carried its own TU-local dActor_c. Both are gone:
 * include/daSldMng_c.h is the real class, deriving from the real
 * include/dActor_c.h, and the size assert there pins it at 0xd8 -- the literal
 * the factory below passes to fBase_c::operator new.
 */

#include "daSldMng_c.h"
#include "decl_Actor.h"
#include "decl_common.h"
#include "decl_ActorBase.h"

extern "C" {
extern int _ZTV10daSldMng_c[];
extern u16 DecIfAbove0_Short(u16 *timer);
/* ABI wall, carried verbatim from the legacy Behavior source: spelling this as
 * Sound::PlaySub(..., Fix12<int>, bool) makes mwccarm home the by-value Fix12
 * in an 8-byte stack slot, growing that body from 0xa4 to 0xb8. The cartridge
 * passes the same raw fixed-point word in r3, so the measured scalar call view
 * is retained while the actor's own methods are spelled normally. */
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    u32 soundID, u32 volume, u32 pan, Fix12i distance, bool loop);
}

/* The spawn position, ov019 0x021135d8. Nothing in the ROM names it. */
extern Vector3 data_ov019_021135d8;

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daSldMng_c_Spawn, 0x0211274c, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daSldMng_c_Spawn
/* resolved: VT = _ZTV10daSldMng_c.
 *
 * The natural C++ spelling `return new daSldMng_c` reproduces the text, but
 * relocates the allocation call to the unavailable global `_Znwm` rather than
 * the cartridge's fBase_c::operator new, so the measured C allocator veneer is
 * kept. What DID change with the promotion is the vptr store: this TU now
 * defines the vtable, and mwccarm's `_ZTV<C>` addresses the object's start --
 * the two-word [offset-to-top, typeinfo] preamble -- while symbols.txt's
 * `_ZTV10daSldMng_c` at 0x021133cc is already the slot array. Hence `&[2]`. */
extern "C" int *daSldMng_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV10daSldMng_c[2]; /* +8: this TU defines the vtable */
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN10daSldMng_c13InitResourcesEv, 0x0211271c, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daSldMng_c13InitResourcesEv
/* daSldMng_c::InitResources -- vtable slot 0, ov019 0x0211271c.
 *
 * Real member function, and THE KEY FUNCTION of this class: the destructor is
 * inline in the header, so the first declared non-inline virtual is what makes
 * this TU emit the _ZTV/_ZTI/_ZTS group. mPosX/mPosY/mPosZ are dActor_c's own
 * named fields; mKillTimer is this class's, seeded with 120 frames. */
int daSldMng_c::InitResources()
{
    mPosX = data_ov019_021135d8.x;
    mPosY = data_ov019_021135d8.y;
    mPosZ = data_ov019_021135d8.z;
    mKillTimer = 0x78;
    return 1;
}


/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN10daSldMng_c8BehaviorEv, 0x02112678, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daSldMng_c8BehaviorEv
/* daSldMng_c::Behavior -- vtable slot 6, ov019 0x02112678.
 *
 * Real member function. State 0 waits for the player to come within 0x180000
 * (24.0 in Fix12), starts the loop sound and steps mState; state 1 counts
 * mKillTimer down and, at zero, stops the sound and kills the actor. Both
 * dActor_c calls are the real inherited methods; only Sound::PlaySub keeps its
 * mangled spelling, for the ABI reason stated above. */
int daSldMng_c::Behavior()
{
    switch (mState) {
    case 0:
        if (DistToCPlayer() < 0x180000) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x14, 0x7f, 0x15666, false);
            ++mState;
        }
        break;
    case 1:
        if (DecIfAbove0_Short(&mKillTimer) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x7f, 0, 0x15666, false);
            KillAndTrackInDeathTable();
        }
        break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN10daSldMng_cD1Ev 0x0211261c size 0x24 and       */
/* _ZN10daSldMng_cD0Ev 0x02112640 size 0x38 -- are NOT written here.         */
/*                                                                            */
/* The destructor is defined INLINE in include/daSldMng_c.h. Written           */
/* out-of-line here the real destructor makes mwccarm emit D0 BEFORE D1, the   */
/* reverse of the cartridge's order, which objisolate refuses for the whole    */
/* translation unit, and it emits a third D2 body with no ROM home. The inline */
/* definition gives the retail D1/D0 pair in ROM order and no D2, while        */
/* InitResources -- declared out-of-line above and first in the class body --  */
/* keeps this TU as the class's key-function TU, so it still owns the complete */
/* _ZTV/_ZTI/_ZTS group declared in this entry's compiler_only_output. D0 is   */
/* that destructor plus dActor_c's inherited inline `operator delete`; slot 17 */
/* is the deleting variant.                                                    */
/*                                                                            */
/* The body is genuinely empty: the class adds no member with a destructor to  */
/* dActor_c, so the ROM's D1 is a vptr store and the base chain, nothing more. */
/* -------------------------------------------------------------------------- */
