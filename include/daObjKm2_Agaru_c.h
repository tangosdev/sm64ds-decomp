#ifndef DAOBJKM2_AGARU_C_H
#define DAOBJKM2_AGARU_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjKm2_Agaru_c_classInit (historical alias daObjKm2_Agaru_c_Spawn)
 *                 fBase_c::operator new(808 = 0x328), dBgActor_c::dBgActor_c(),
 *                 stores this class's vtable.
 *   ~daObjKm2_Agaru_c   chains straight to ~dBgActor_c: the four members below
 *                 are scalars, so the destructor names none of them.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly
 * on it. dBgActor_c ends at 0x320, so the four fields below are this class's.
 *
 * THE FIELDS come from Behavior and InitResources, which is a rising platform:
 * InitResources latches the spawn height into 0x320; Behavior counts 0x324 up
 * to 0x14 between moves, steps mPosY by 0xa000 per frame between that spawn
 * height and +0x5dc000 above it, and drives the whole thing off the state byte
 * at 0x327. func_ov045_021114a8 -- the dBgW callback InitResources installs --
 * sets 0x326 when something of collision type 0xbf is standing on it, and
 * Behavior clears it every frame.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots
 * declared below differ; every other slot holds the base's own word and is
 * inherited, so it is deliberately not redeclared here.
 */
struct daObjKm2_Agaru_c : dBgActor_c {
    s32 mRestY;                         /* 0x320 -- spawn height, the low stop */
    u16 mDelayTimer;                    /* 0x324 */
    u8  mRidden;                        /* 0x326 */
    u8  mState;                         /* 0x327 */

    virtual ~daObjKm2_Agaru_c();        /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();      /* slot  0 */
    virtual s32   CleanupResources();   /* slot  3 */
    virtual s32   Behavior();           /* slot  6 */
    virtual s32   Render();             /* slot  9 */
};

typedef char daObjKm2_Agaru_c_size_must_be_0x328[sizeof(daObjKm2_Agaru_c) == 0x328 ? 1 : -1];

#endif /* DAOBJKM2_AGARU_C_H */
