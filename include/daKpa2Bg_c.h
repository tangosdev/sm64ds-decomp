/* MEASURED, from the cartridge -- this class's name is the ROM's own, not a coined one.
 *
 *   _ZTS  ov060 0x0211a87c  "10daKpa2Bg_c"
 *   _ZTI  ov060 0x0211a870  __si_class_type_info; +8 -> _ZTI10dBgActor_c (ov002
 *                           0x021089ec), so the DIRECT base is dBgActor_c and
 *                           nothing else.
 *   _ZTV  ov060 0x0211a8b0  the address point; 0x0211a8a8 holds the Itanium
 *                           preamble (0 offset-to-top, then &_ZTI).
 *   size  0x570             daKpa2Bg_c_classInit's own literal (1392).
 *
 * Overridden slots, by diffing the 32-word table against dBgActor_c's: 0
 * (InitResources, 0x02117c30), 3 (CleanupResources, 0x02117b58), 6 (Behavior,
 * 0x02117bc8), 9 (Render, 0x02117b9c), 16/17 (D1 0x02117980, D0 0x021179d4).
 * Slot 31 still holds dBgActor_c's arm9 word (Kill, 0x020ee55c), so nothing else
 * is overridden. The coined `BowserFireSeaArena` alias that used to sit on this
 * vtable is gone, and so is the coined `_ZTV18BowserFireSeaArena` line that named
 * 0x0211a8b0 in ov060's symbols.txt.
 *
 * This class is a PARTIAL promotion: src/actors/daKpa2Bg_c.cpp is the merged
 * translation unit and licenses the contiguous run 0x02117980..0x02117c30,
 * eight of its ten functions. InitResources (0x02117c30) and
 * daKpa2Bg_c_classInit (0x02117cdc) keep their own enrolled shards and are not
 * written in the merged source, though both were shown to reproduce their ROM
 * bytes from it. The obstacle is one relocation, not a byte: the CLPS block
 * InitResources passes to dBgW_KcMbg::SetFile is loaded from 0x021115bc, which
 * ov060/relocs.txt records as an ambiguous cross-overlay load with twenty
 * candidate modules, and ov060 makes no unambiguous call to any of them, so the
 * reference stays the phantom `func_021115bc` and no symbols.txt defines it.
 * The factory sits ABOVE that hole, so no contiguous complete span reaches it
 * either. See config/tu_manifest.d/ov060/daKpa2Bg_c.json for the measurements.
 *
 * SM64DS RTTI names the implementation daKpa2Bg_c. The reconstructed factory
 * daKpa2Bg_c_classInit (historical alias daKpa2Bg_c_classInit)
 * installs this class's cartridge vtable; the reconstructed profile
 * global g_profile_KOOPA2BG (historical alias daKpa2Bg_c_SpawnInfo)
 * is its registry descriptor.
 */
/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class daKpa2Bg_c: 6 matched functions.
 *
 * A dBgActor_c subclass -- the only one in ov060 -- which is what the destructor
 * shows: it tears down its own two members, then REWRITES THE VPTR to _ZTV10dBgActor_c
 * and tears down two more at dBgActor_c's offsets, then chains to dActor_c. That middle
 * vptr store is a base-class destructor running inline, and it is the evidence for
 * the base.
 *
 * Two sub-objects of its own, on top of dBgActor_c's two:
 *
 *     dBgActor_c            0x000 + 0x324 = 0x324   -> mModel2
 *     Model               0x324 + 0x050 = 0x374   -> mMovingMeshCollider2
 *     dBgW_KcMbg  0x374 + 0x1c8 = 0x53c   -> padding, then unk_56c
 *
 * mMovingMeshCollider2 was a `u8` marker with 0x1f7 bytes of pad behind it; the pad
 * was the object.
 *
 * sizeof is 0x570, which is not inferred from the fields: daKpa2Bg_c_classInit
 * asks fBase_c::operator new for 1392 bytes.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef DAKPA2BG_C_H
#define DAKPA2BG_C_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daKpa2Bg_c : dBgActor_c {
    /* THIS CLASS'S OWN, not dBgActor_c's -- dBgActor_c ends at 0x31e. They are what
       push mModel2 from 0x320 to 0x324, and the first two sit in the base's
       tail padding. */
    s16 mAngleXSpeed;                           /* 0x31e -- Behavior adds it to mAngleX every frame */
    s16 mAngleYSpeed;                           /* 0x320 -- ... to mAngleY */
    s16 mAngleZSpeed;                           /* 0x322 -- ... to mAngleZ */
    Model mModel2;                              /* 0x324 */
    dBgW_KcMbg mMovingMeshCollider2;    /* 0x374 */
    u8  pad_53c[0x30];
    s32 unk_56c;            /* 0x56c */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* MEASURED -- INLINE ON PURPOSE. The cartridge orders the pair D1
       0x02117980 then D0 0x021179d4, and an in-class body is the form that
       emits them in that order and emits no D2. An out-of-line definition
       emits D0 ahead of the written D1 plus a homeless D2, and the licensed
       run cannot carry either. Keep the opening brace on the signature line:
       tools/check_header_offsets.py only arms its body skip when it is there. */
    virtual ~daKpa2Bg_c() {}    /* slots 16 (D1), 17 (D0) */

    /* --- overrides of inherited fBase_c slots; each takes its base's index,
           so the order here adds nothing to the table. It decides only which
           member is this class's key function, and with the destructor inline
           that is Behavior -- the TU that defines it emits _ZTV10daKpa2Bg_c
           and both destructor variants. --- */
    virtual int Behavior();             /* slot  6 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int InitResources();        /* slot  0 */
    virtual int Render();               /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daKpa2Bg_c_size_must_be_0x570[
    sizeof(daKpa2Bg_c) == 0x570 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, which has no base class to inherit
   dBgActor_c's and dActor_c's fields from and so spells the whole layout flat. */
struct daKpa2Bg_c {
    u8  pad_000[0x8e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [src/actors/daKpa2Bg_c.cpp] */
    Model mModel1;            /* 0x0d4 */
    dBgW_KcMbg mMovingMeshCollider1;    /* 0x124 */
    u8  pad_2ec[0x32];
    s16 mAngleXSpeed;       /* 0x31e */
    s16 mAngleYSpeed;       /* 0x320 */
    s16 mAngleZSpeed;       /* 0x322 */
    /* Model member, named by _ZN5ModelD1Ev at +0x324 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel2;            /* 0x324 */
    dBgW_KcMbg mMovingMeshCollider2;    /* 0x374 */
    u8  pad_53c[0x30];
    s32 unk_56c;            /* 0x56c */
};

#endif /* __cplusplus */

#endif
