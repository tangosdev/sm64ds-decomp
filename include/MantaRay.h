#ifndef MANTARAY_H
#define MANTARAY_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN8MantaRayD1Ev` destroys each member, and
 * `MantaRay_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV8MantaRay`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAcPos_c  0x40    -> 0x150
 *     0x150 dBgCh_Actr               0x1bc   -> 0x30c
 *     0x30c ModelAnim                  0x64    -> 0x370
 *
 * SIZE IS THE FACTORY'S LITERAL, NOT THE FIELD SPAN. `MantaRay_Spawn` calls
 * `fBase_c::operator new(1028)` -- 0x404 -- and stores this class's vtable,
 * so that literal IS this class's sizeof. The evidenced fields reach only
 * 0x38c; the 0x94 between is trailing space no source reads, and a rounded-up
 * field span would have made this assert wrong by 120 bytes.
 *
 * SM64DS proves this class as daManta_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daManta_c_classInit at 0x02132fe8 (historical alias MantaRay_Spawn)
 * allocates 0x404 and installs this class's cartridge vtable. It backs the
 * MANTA registry profile, whose descriptor at 0x02134218 is reconstructed as
 * g_profile_MANTA.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct MantaRay : dEnemyBase_c {
    dCcAcPos_c    mdCcAcPos_c; /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x150 */
    ModelAnim                    mModelAnim;            /* 0x30c */
    u8                           unk_370;               /* 0x370 */
    u8  pad_371[0xb];
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          mPathNode;             /* 0x384 */
    s32                          unk_388;               /* 0x388 */
    u8  pad_38c[0x78];

    /* --- vtable --- */
    virtual ~MantaRay();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char MantaRay_size_must_be_0x404[sizeof(MantaRay) == 0x404 ? 1 : -1];

#endif /* MANTARAY_H */
