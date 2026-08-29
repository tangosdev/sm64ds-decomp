#ifndef DAOBJGURAGURA_C_H
#define DAOBJGURAGURA_C_H

#include "types.h"

/* The abstract base of the tilting platforms: the slabs that wobble under your
 * weight and settle back when you step off. `guragura` is the wobble itself.
 *
 * A LAYER THE TREE DID NOT HAVE. This header used to be a flat struct under
 * `u8 pad_000[0x330]`, emitted by `tools/rtti_vtables.py --emit-headers` because
 * that pass knew offsets and not sizeof(base). include/dBgActor_c.h has since
 * settled sizeof(dBgActor_c) = 0x320, so the class can be spelled as what it is. The
 * regenerate line is gone with the generated body: that tool deletes only files
 * that still carry it, and this one is hand-written now.
 *
 *   _ZTI15daObjGuragura_c  ov002 0x0210905c
 *   _ZTS15daObjGuragura_c  ov002 0x02109068   "15daObjGuragura_c"
 *   vtable                 ov002 0x02109084, 32 slots, same count as the base
 *   kind                   __si_class_type_info, ONE base, subobject offset 0
 *   base                   dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null. Its
 * own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0).
 *
 * TWO DESCENDANTS: daObjKm2_Gura_c (TiltingPlatformBfs) and daObjFl_Gura_c
 * (TiltingPlatformLll). Each one's destructor stores this class's vtable between
 * its own and _ZTV10dBgActor_c.
 *
 * SIZE 0x350, the literal both factories pass to fBase_c::operator new.
 *
 * TWO QUATERNIONS, AND THEY CLOSE ON EACH OTHER. This class's own Behavior,
 * `daObjGuragura_c::Behavior` (0x020b616c), ends with
 *
 *     Quaternion_SLerp(this+0x320, this+0x330, 0x199, this+0x320)
 *
 * so 0x320 is the current orientation and 0x330 the target, both 4 x s32 = 0x10.
 * 0x320 + 0x10 = 0x330 and 0x330 + 0x10 = 0x340, and the same function writes the
 * target as exactly four words from data_02092768[0..3]. The generated body had
 * those four words as four separate s32 at 0x330/0x334/0x338/0x33c and no 0x320 at
 * all; they are one quaternion.
 *
 *   0x34c  cleared at the end of every Behavior. Something else sets it; nothing
 *          this class's own methods do.
 *   0x34d  the settle delay. While DecIfAbove0_Byte is still counting it down the
 *          target quaternion is left alone; when it hits zero the target is
 *          reloaded from the table.
 *
 * 0x34d + 1 = 0x34e, and the quaternions force 4-byte alignment, so sizeof rounds
 * to 0x350 -- the factories' literal. 0x340..0x34c is UNOBSERVED, not inherited:
 * nothing this class's own methods touch lands there and neither leaf declares
 * anything there. Field NAMES are coined from what the code does; nothing in the
 * ROM names them.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjGuragura_c : dBgActor_c {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    s32 mTilt[4];           /* 0x320 - quaternion, the live orientation */
    s32 mTiltTarget[4];     /* 0x330 - quaternion, what it lerps toward */
    u8  pad_340[0xc];
    u8  mBumped;            /* 0x34c - cleared every frame by Behavior */
    u8  mSettleDelay;       /* 0x34d - frames before the target is reloaded */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN15daObjGuragura_cD1Ev (which does exist out of line, at ov002
       0x020b6030, still under its func_ov002_ name). An out-of-line declaration
       here would make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjGuragura_c() {}

    /* Two of the four own overrides the banner above already lists, spelled
       WITHOUT the `virtual` keyword -- the same way include/daObjMarioCap_c.h and
       include/daObjRc_Dorifu_c.h spell theirs. An override of a virtual the base
       already declares is implicitly virtual either way, so both reuse a slot the
       32-slot table above already has and neither adds a field: the 0x350 size
       assert below still holds. Writing `virtual` would read as a NEW slot, which
       is exactly what this class must not gain.

       RENDER IS DECLARED FIRST ON PURPOSE, AND THE ORDER IS WHAT MATTERS, not the
       slot number. The destructor above is inline, so the KEY FUNCTION -- the first
       non-inline virtual declared in the class -- is whichever of these comes
       first, and the key function's translation unit is the one that emits
       _ZTV15daObjGuragura_c. That symbol is already delinked data (ov002
       0x02109084, config/arm9/overlays/ov002/symbols.txt), and tools/eligible.py
       drops any file whose object carries a section other than .text -- so a TU
       that emitted it would be dropped from the build entirely and the function it
       defines would stop being compiled, with every gate still reporting green
       (notes: "unbuildable files are invisible"). Render's definition
       (func_ov002_020b6144) is not migrated, so naming it here parks the key
       function on a translation unit that does not exist and no file emits the
       vtable -- the tree's state today, and the same mechanism
       include/dBgActor_c.h's own destructor comment relies on. Measured on the
       sibling daObjKuruma_c: with Behavior declared first the object came out with
       eleven .data sections; with Render first, one .text. */
    s32 Render();                      /* slot  9 -- see above; not yet migrated */
    s32 Behavior();                    /* slot  6 */
    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dBgActor_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual s32 InitResources() = 0;        /* slot  0 */
    virtual s32 CleanupResources() = 0;     /* slot  3 */
};

typedef char daObjGuragura_c_size_must_be_0x350[sizeof(daObjGuragura_c) == 0x350 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit dBgActor_c's fields from and so spells the layout flat. Same arrangement
   as include/dBgActor_c.h. */
struct daObjGuragura_c {
    u8  pad_000[0x320];
    s32 mTilt[4];           /* 0x320 */
    s32 mTiltTarget[4];     /* 0x330 */
    u8  pad_340[0xc];
    u8  mBumped;            /* 0x34c */
    u8  mSettleDelay;       /* 0x34d */
};

#endif /* __cplusplus */

#endif /* DAOBJGURAGURA_C_H */
