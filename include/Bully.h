#ifndef BULLY_H
#define BULLY_H

#include "types.h"
#include "daOts_c.h"

/* daDonketu_c in the ROM's RTTI. Derives from daOts_c, which owns every member this
 * header used to restate -- the ModelAnim, the WithMeshClsn, the file table, the
 * MovingCylinderClsn and the ShadowModel are all the base's, and Bully_Spawn proves
 * it by constructing them between the two vtable stores.
 *
 * SIZE 0x400, which is the literal in Bully_Spawn's ActorBase::operator new. The base
 * ends at 0x398, so everything below is Bully's own.
 */
struct Bully : daOts_c {
    u8  pad_398[0x64];
    /* An actor unique ID, not a count: Behavior passes it to Actor::FindWithID and
       increments the byte at +0x3fe of whatever comes back; InitResources zeroes it.
       Left unnamed because that is as far as the bytes go -- BigBully's u8 at the
       same offset is a different field with a different use, so the offset is no
       guide. */
    s32 unk_3fc;                    /* 0x3fc */

    virtual ~Bully();

    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
    int InitResources();
};

typedef char Bully_size_must_be_0x400[sizeof(Bully) == 0x400 ? 1 : -1];

#endif /* BULLY_H */
