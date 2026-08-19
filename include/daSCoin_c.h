#ifndef DASCOIN_C_H
#define DASCOIN_C_H
#include "types.h"
#include "dActor_c.h"
#include "MovingCylinderClsn.h"

/* TWO WITNESSES:
 *
 *   daSCoin_c_Spawn  fBase_c::operator new(276 = 0x114),
 *       dActor_c::dActor_c(), stores the class vtable, then the
 *       MovingCylinderClsn member below.
 *   _ZN9daSCoin_cD0Ev  the same member destroyed, then ~dActor_c.
 *
 * SIZE 0x114 is the factory's own literal; unk_113 (1 byte, 0x113) closes
 * exactly on it.
 *
 * RENAMED FROM "InvisibleSecret" -- the same defect and the same fix as
 * daObjAbuku_c/"Bubble" (see that header): this class's own vtable, found
 * via its typeinfo relocation (_ZTI9daSCoin_c <- vtable+4), fills its D1/D0
 * slots (16/17) with the addresses the tree had previously named
 * _ZN15InvisibleSecretD1Ev/_ZN15InvisibleSecretD0Ev, a guess made before the
 * RTTI evidence (_ZTS9daSCoin_c, the ROM's own class-name string) existed.
 *
 * Everything below 0x0d0 is this class's own -- dActor_c ends at exactly
 * 0x0d0, and pad_0d0 (unevidenced, 4 bytes) is the gap before the
 * MovingCylinderClsn member at 0x0d4. The old generated header placed a
 * field at 0x0f8, inside MovingCylinderClsn's own 0x0d4..0x108 span;
 * daSCoin_c_Behavior reads that word directly out of the collision
 * sub-object's own bytes (via a raw offset cast in the consumer, not a
 * field of this class), the same shape as daObjAbuku_c's 0x0f8.
 *
 * unk_112 (1 byte, 0x112) was padding in the generated header, but the
 * class's own (currently unenrolled) func_ov002_020f051c.c helper reads and
 * writes it as a real "already paired" flag, so it is named here for
 * accuracy even though nothing enrolled in this pass touches it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c: only slot 0
 * (InitResources), slot 3 (CleanupResources) and slot 6 (Behavior) differ,
 * all still fBase_c's own slots in dActor_c -- Render, OnPendingDestroy,
 * OnYoshiTryEat and OnTurnIntoEgg are all still the base's own words. All
 * three are extern "C" free functions under their mangled names (the same
 * idiom fBase_c.h itself uses for its own slot 0), and the destructor is
 * declared but never defined out of line -- like dActor_c's own
 * _ZTV8dActor_c, this class's vtable stays ROM-supplied data
 * (kind:data(any) in symbols.txt), not compiler-emitted; D1/D0 stay the
 * pre-existing extern "C" free functions, just renamed.
 */
struct daSCoin_c : dActor_c {
    u8  pad_0d0[0x4];
    /* MovingCylinderClsn member, named by daSCoin_c_Spawn's own C1 call and
       the class's own destructors' D1 call at +0xd4.
       [daSCoin_c_Spawn.c, _ZN9daSCoin_cD1Ev.c, _ZN9daSCoin_cD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x0d4 */
    s32 unk_108;            /* 0x108 */
    u8  pad_10c[0x1];
    u8  unk_10d;            /* 0x10d */
    u8  unk_10e;            /* 0x10e */
    u8  unk_10f;            /* 0x10f */
    u8  unk_110;            /* 0x110 */
    u8  unk_111;            /* 0x111 */
    u8  unk_112;            /* 0x112 -- read/written by the class's own (unenrolled) func_ov002_020f051c.c */
    u8  unk_113;            /* 0x113 */

    virtual ~daSCoin_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
};

typedef char daSCoin_c_size_must_be_0x114[sizeof(daSCoin_c) == 0x114 ? 1 : -1];

#endif
