#ifndef DAOBJWC_OBJ05_C_H
#define DAOBJWC_OBJ05_C_H

#include "types.h"
#include "dBgActor_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daObjWc_Obj05_c_classInit (historical alias CageLift_Spawn) allocates
 *                 816 = 0x330, calls dBgActor_c::dBgActor_c(), stores _ZTV15daObjWc_Obj05_c,
 *                 then the members below in this order.
 *   ~daObjWc_Obj05_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x330 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjWc_Obj05_c : dBgActor_c {
    /* 0x320..0x32c, folded in from a SECOND header that used to declare this
     * same class over again, un-inherited, and be cast to from `this` inside
     * InitResources and Behavior. (That header held the cartridge name while
     * the real declaration below was still called CageLift; folding it in is
     * what freed the name for this file.) Both spellings were
     * compiled into the ROM at once. The names are that header's own; they are
     * placeholders and renaming them cannot change codegen. Its `mPosY` (0x060)
     * and `mAngleY` (0x08e) are NOT repeated here -- they are dActor_c's own
     * fields at those offsets, reached by inheritance. */
    s32 unk_320;                    /* 0x320 */
    s32 unk_324;                    /* 0x324 -- Sound::PlayLong handle */
    s16 unk_328;                    /* 0x328 */
    u8  unk_32a;                    /* 0x32a */
    u8  unk_32b;                    /* 0x32b */
    u8  unk_32c;                    /* 0x32c -- state selector */
    u8  pad_32d[0x3];

    /* Defined INLINE on purpose. Out of line, mwccarm emits D2, D0, D1; the
     * ROM has D1 at 0x02111ac4 then D0 at 0x02111b08 and no D2 anywhere in
     * ov029. Moving the (empty) body here flips emission to D1, D0 and drops
     * the D2 entirely. The key function is then the first DECLARED non-inline
     * virtual, InitResources, which lives in this TU -- so this TU also owns
     * the class's vague-linkage _ZTV/_ZTI/_ZTS, licensed in the manifest. */
    virtual ~daObjWc_Obj05_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char daObjWc_Obj05_c_size_must_be_0x330[sizeof(daObjWc_Obj05_c) == 0x330 ? 1 : -1];

#endif /* DAOBJWC_OBJ05_C_H */
