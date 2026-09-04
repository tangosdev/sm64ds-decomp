#ifndef DAIDONKETU_C_H
#define DAIDONKETU_C_H

#include "types.h"
#include "daOts_c.h"

/* The third of daOts_c's children -- the small Bully that stands on the ice.
 * daIDonketu_c_classInit is daDonketu_c_classInit and daBDonketu_c_classInit again, with 0x3fc for the
 * allocation and this class's vtable stored last.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `ChillBully`, a coined name. MEASURED in extracted/overlays/overlay_0027.bin
 * (ov027 base 0x021111a0, the `.text start:` on line 1 of
 * config/arm9/overlays/ov027/delinks.txt): the vtable object's preamble at
 * 0x02113928 is [offset-to-top 0, 0x021138b0], and the typeinfo record at
 * 0x021138b0 reads [0x0209a764, 0x021138c8, 0x021138bc] --
 * _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), a typeinfo name at
 * 0x021138c8 whose bytes are the string "12daIDonketu_c", and the base's record.
 * Following that base pointer gives "7daOts_c" at 0x021138a4, which is the name the
 * tree already uses for the parent.
 *
 * That rename is what makes the promotion to one translation unit possible at all:
 * a coined class reaches no symbol home, so its _ZTI/_ZTS/_ZTV records can never be
 * word-compared against the cartridge and its key-function TU cannot declare a
 * verifiable compiler_only_output (tools/tubuild.py apply_compiler_only_policy).
 *
 *   _ZTS7daOts_c        ov027 0x021138a4   "7daOts_c"
 *   _ZTI12daIDonketu_c  ov027 0x021138b0
 *   _ZTI7daOts_c        ov027 0x021138bc
 *   _ZTS12daIDonketu_c  ov027 0x021138c8   "12daIDonketu_c"
 *   _ZTV12daIDonketu_c  ov027 0x02113930   (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *
 * BOTH TYPEINFO RECORDS LIVE IN ov027, the leaf's and the base's, even though
 * daOts_c's code and vtable are in ov064. That is the vague-linkage copy the leaf's
 * own typeinfo needs to point at, emitted into whichever module first needed it.
 *
 * SIZE 0x3fc, the literal in daIDonketu_c_classInit's fBase_c::operator new. The base
 * ends at 0x398, so the two bytes below are all this class adds -- which is why it is
 * four bytes smaller than its siblings rather than larger.
 *
 * Besides InitResources, Behavior and the destructor pair, it overrides daOts_c's
 * state-1 and state-4 hooks. CleanupResources, Render, the actor hooks, and the four
 * sound hooks remain inherited.
 *
 * SM64DS RTTI names the implementation daIDonketu_c. The reconstructed factory
 * daIDonketu_c_classInit (historical alias daIDonketu_c_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_ICE_DONKETU
 * (historical alias daIDonketu_c_SpawnInfo) is its registry descriptor.
 */
struct daIDonketu_c : daOts_c {
    u8  pad_398[0x62];
    /* Which star this one carries: InitResources takes param1 & 0xf, and both users
       pass it on with bit 6 set. */
    u8  mStarIdx;                   /* 0x3fa */
    /* The slot dActor_c::TrackStar claimed for that star, or -1 -- see the note on
       TrackStar in dActor_c.h. Its ADDRESS is what goes to UntrackAndSpawnStar, which
       is why it is a field and not a local. */
    s8  mStarSlot;                  /* 0x3fb */

    /* --- vtable --- */
    /* INLINE IS LOAD-BEARING. Written out-of-line in the translation unit, mwccarm
       emits D0 BEFORE D1 -- the reverse of the cartridge's order, which
       objisolate.py refuses for the whole TU (this entry's own pre-promotion
       verification block recorded it as `ordinal pair(s) not in ROM order: [(0,1)]`)
       -- plus a third D2 with no ROM home. Defined here it emits the retail D1/D0
       pair in ROM order and no D2. Unlike daOts_c's and dEnemyBase_c's, this body is
       inlined by nobody: the class has no descendants. */
    virtual ~daIDonketu_c() {}          /* slots 16 (D1), 17 (D0) */

    /* THE KEY FUNCTION IS Behavior -- the first DECLARED non-inline virtual, not
       the first slot. It and InitResources override daOts_c's pure
       `virtual int Behavior() = 0` / `virtual int InitResources() = 0` pair, so they
       are already virtual and add no slot; the spellings match those declarations
       exactly. Whichever TU defines the key function emits this class's
       _ZTV/_ZTI/_ZTS group -- src/actors/d_a_i_donketu.cpp, which licenses them as
       deadstrip-data so romdata_check word-compares them against the cartridge. */
    int Behavior();                     /* slot  6 */
    int InitResources();                /* slot  0 */
    virtual int UpdateRunState();       /* slot 31 */
    virtual void UpdateDeathState();    /* slot 32 */
};

typedef char daIDonketu_c_size_must_be_0x3fc[sizeof(daIDonketu_c) == 0x3fc ? 1 : -1];

#endif /* DAIDONKETU_C_H */
