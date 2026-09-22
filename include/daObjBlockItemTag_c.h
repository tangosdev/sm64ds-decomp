#ifndef DAOBJBLOCKITEMTAG_C_H
#define DAOBJBLOCKITEMTAG_C_H

#include "dActor_c.h"

/* The cartridge RTTI names this class daObjBlockItemTag_c. daObjBlockItemTag_c is the
 * readable compatibility spelling already carried by all configured virtuals.
 * Its __si_class_type_info record names dActor_c as the sole direct base at
 * offset zero, and the 31-slot table at 0x02108c18 has exactly the same extent
 * as dActor_c. Only slots 0, 3, 6, 16 and 17 are overridden.
 *
 * Four adjacent factories independently allocate 0xdc bytes, call dActor_c's
 * constructor and install this same table. They are the item-tag variants for
 * One-Up Mushroom, Mega Mushroom, Koopa Shell and Silver Star blocks. The PMF
 * table selects the corresponding action below after the physical block links
 * itself to this tag. */
struct daObjBlockItemTag_c : dActor_c {
    u8 pad_0d0[0x4];
    s8 mStarID;                    /* 0x0d4 -- Silver Star number, 0x7f -> 0 */
    s8 mTrackStarID;               /* 0x0d5 -- dActor_c::TrackStar result */
    u8 mActionPending;             /* 0x0d6 */
    u8 mActionIndex;               /* 0x0d7 -- index into the four-entry PMF table */
    u8 mIsAttached;                /* 0x0d8 -- linked to a physical block */
    u8 pad_0d9[0x3];

    /* Declared first so the single out-of-line definition in
     * src/actors/daObjBlockItemTag_c.cpp stays the key function and the TU
     * emits the same verified data passengers. */
    virtual ~daObjBlockItemTag_c();                         /* slots 16, 17 */

    virtual int InitResources();                   /* slot  0 */
    virtual int CleanupResources();                /* slot  3 */
    virtual int Behavior();                        /* slot  6 */

    /* The ROM helper takes (&destination, &tag), not a member-function ABI
     * (this, &destination). A static class helper preserves that observed
     * argument order while keeping the routine owned by daObjBlockItemTag_c. */
    static void GetSpawnPos(Vector3 &destination, daObjBlockItemTag_c &tag);

    void SpawnKoopaShell();
    void SpawnMegaMushroom();
    void SpawnOneUpMushroom();
    void SpawnSilverStar();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBlockItemTag_c_size_must_be_0xdc[sizeof(daObjBlockItemTag_c) == 0xdc ? 1 : -1];
#endif

#endif /* DAOBJBLOCKITEMTAG_C_H */
