/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class VolcanoFire: 5 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef VOLCANOFIRE_H
#define VOLCANOFIRE_H
#include "types.h"
#include "dCcAc_c.h"

struct VolcanoFire {
    u8  pad_000[0xd4];
    dCcAc_c mdCcAc_c;         /* 0x0d4 */
    /* The four fields below are read by Behavior and CleanupResources and were
       simply off the end of the generated header. VolcanoFire_Spawn allocates
       0x11c, and 0x118 is the only word neither function touches. */
    /* State record. Behavior loads it, reads a pointer-to-member at +0x8 and
       calls it on `this` -- so it points at a state table entry, not a scalar. */
    void *mState;             /* 0x108 */
    /* The VolcanoRing that spawned this fire: CleanupResources decrements the
       u16 at +0x324 of whatever this points at. */
    void *mSpawner;           /* 0x10c */
    /* Counted down every frame by DecIfAbove0_Short. */
    u16 mKillTimer;           /* 0x110 */
    u8  pad_112[0x2];
    /* Particle handle, passed back into Particle::System::NewUnkCallback818. */
    u32 mParticleID;          /* 0x114 */
    /* Unread by the five recovered functions; the reference calls it killPosY. */
    s32 unk_118;              /* 0x118 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

typedef char VolcanoFire_size_must_be_0x11c[sizeof(struct VolcanoFire) == 0x11c ? 1 : -1];

#endif
