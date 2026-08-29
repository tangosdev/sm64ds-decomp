#ifndef CHILLBULLY_H
#define CHILLBULLY_H

#include "types.h"
#include "daOts_c.h"

/* daIDonketu_c in the ROM's RTTI, and the third of daOts_c's children -- the small
 * one that stands on the ice. ChillBully_Spawn is Bully_Spawn and BigBully_Spawn
 * again, with 0x3fc for the allocation and this class's vtable stored last.
 *
 * SIZE 0x3fc, the literal in ChillBully_Spawn's fBase_c::operator new. The base
 * ends at 0x398, so the two bytes below are all this class adds -- which is why it is
 * four bytes smaller than its siblings rather than larger.
 *
 * Its vtable is at 0x02113930 in ov027; the word before it is 0x021138b0, which is
 * _ZTI12daIDonketu_c. Besides InitResources, Behavior and the destructor pair, it
 * overrides daOts_c's state-1 and state-4 hooks. CleanupResources, Render, the actor
 * hooks, and the four sound hooks remain inherited.
 */
struct ChillBully : daOts_c {
    u8  pad_398[0x62];
    /* Which star this one carries: InitResources takes param1 & 0xf, and both users
       pass it on with bit 6 set. */
    u8  mStarIdx;                   /* 0x3fa */
    /* The slot dActor_c::TrackStar claimed for that star, or -1 -- see the note on
       TrackStar in dActor_c.h. Its ADDRESS is what goes to UntrackAndSpawnStar, which
       is why it is a field and not a local. */
    s8  mStarSlot;                  /* 0x3fb */

    virtual ~ChillBully();

    /* methods */
    int Behavior();
    int InitResources();
    virtual int UpdateRunState();
    virtual void UpdateDeathState();
};

typedef char ChillBully_size_must_be_0x3fc[sizeof(ChillBully) == 0x3fc ? 1 : -1];

#endif /* CHILLBULLY_H */
