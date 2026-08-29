#ifndef BIGBULLY_H
#define BIGBULLY_H

#include "types.h"
#include "daOts_c.h"

/* daBDonketu_c in the ROM's RTTI, and Bully's sibling rather than its subclass: both
 * derive from daOts_c, which owns all five of the members this header used to
 * restate. BigBully_Spawn is byte-for-byte Bully_Spawn apart from the vtable it
 * stores last.
 *
 * SIZE 0x400, the literal in BigBully_Spawn's fBase_c::operator new. The base ends
 * at 0x398.
 */
struct BigBully : daOts_c {
    u8  pad_398[0x62];
    u16 mSecretSoundCounter;        /* 0x3fa */
    u8  mStarID;                    /* 0x3fc */
    u8  unk_3fd;                    /* 0x3fd */
    u8  mNumBulliesKilled;                    /* 0x3fe */
    u8  pad_3ff[0x1];

    virtual ~BigBully();

    /* methods */
    int Behavior();
    int InitResources();
    int Render();
    virtual int UpdateRunState();
    virtual void UpdateDeathState();
};

typedef char BigBully_size_must_be_0x400[sizeof(BigBully) == 0x400 ? 1 : -1];

#endif /* BIGBULLY_H */
