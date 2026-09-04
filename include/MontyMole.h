#ifndef MONTYMOLE_H
#define MONTYMOLE_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

/* TWO WITNESSES:
 *
 *   daChoropu_c_classInit  fBase_c::operator new(396 = 0x18c),
 *       dActor_c::dActor_c(), stores _ZTV9MontyMole, then the two members
 *       below in this order.
 *   _ZN9MontyMoleD0Ev  the same two members destroyed in reverse, then
 *       ~dActor_c.
 *
 * SIZE 0x18c is the factory's own literal; unk_188 (4 bytes, 0x188) closes
 * exactly on it.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0, so unk_008 (dActor_c/fBase_c's
 * own param1 at 0x008) is the only pre-0xd0 field this class's consumers
 * touched; it was repointed to the inherited fBase_c name.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. MontyMole
 * overrides slot 0 (InitResources), slot 3 (CleanupResources), slot 6
 * (Behavior) and slot 9 (Render) -- all still fBase_c's own slots in
 * dActor_c -- plus slot 29 (OnAimedAtWithEgg). Every other slot holds the
 * base's own word and is inherited, so it is deliberately not redeclared
 * here.
 *
 * SM64DS RTTI names the implementation daChoropu_c. The reconstructed
 * factory daChoropu_c_classInit (historical alias
 * MontyMole_Spawn) constructs it for the CHOROPU
 * registry profile.
 */
struct MontyMole : dActor_c {
    u8  pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build checks. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x138. [_ZN9MontyMoleD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x138 */
    u8  unk_16c;            /* 0x16c */
    u8  pad_16d[0xf];
    s32 unk_17c;            /* 0x17c */
    u8  unk_180;            /* 0x180 */
    u8  unk_181;            /* 0x181 */
    u8  unk_182;            /* 0x182 */
    u8  unk_183;            /* 0x183 */
    u8  unk_184;            /* 0x184 */
    u8  pad_185[0x3];
    s32 unk_188;            /* 0x188 */

    virtual ~MontyMole();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual s32  OnAimedAtWithEgg();      /* slot 29 */
};

typedef char MontyMole_size_must_be_0x18c[sizeof(MontyMole) == 0x18c ? 1 : -1];

#endif
