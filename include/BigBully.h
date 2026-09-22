#ifndef BIGBULLY_H
#define BIGBULLY_H

#include "types.h"
#include "daOts_c.h"

/* daBDonketu_c in the ROM's RTTI, and Bully's sibling rather than its subclass: both
 * derive from daOts_c, which owns all five of the members this header used to
 * restate. daBDonketu_c_classInit is byte-for-byte daDonketu_c_classInit apart from the vtable it
 * stores last.
 *
 * SIZE 0x400, the literal in daBDonketu_c_classInit's fBase_c::operator new. The base ends
 * at 0x398.
 *
 * SM64DS RTTI names the implementation daBDonketu_c. The reconstructed
 * factory daBDonketu_c_classInit (historical alias
 * BigBully_Spawn) constructs it for the BOSS_DONKETU
 * registry profile.
 */
struct BigBully : daOts_c {
    u8  pad_398[0x62];
    u16 mSecretSoundCounter;        /* 0x3fa */
    u8  mStarID;                    /* 0x3fc */
    u8  unk_3fd;                    /* 0x3fd */
    u8  mNumBulliesKilled;                    /* 0x3fe */
    u8  pad_3ff[0x1];

    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~BigBully();   /* no slot */
#else
    virtual ~BigBully();   /* D1 and D0 */
#endif

    /* methods */
    int Behavior();
    int InitResources();
    int Render();
    virtual int UpdateRunState();
    virtual void UpdateDeathState();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char BigBully_size_must_be_0x400[sizeof(BigBully) == 0x400 ? 1 : -1];
#endif

#endif /* BIGBULLY_H */
