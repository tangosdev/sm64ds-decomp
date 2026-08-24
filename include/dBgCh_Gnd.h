/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dBgCh_Gnd: 3 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * HAND-EXTENDED for dBgW_Kc::DetectClsn(dBgCh_Gnd&) at 0x01ffd3f8:
 *   0x010  the dBgPi the hit is written into (func_02037fd4(ray+0x10, ...))
 *   0x038  the probe position, a Fix12i Vector3, read as .x, .z, .y
 *   0x044  the collision height in Fix12i: the search seed on entry, the hit on exit
 *   0x048  the has-collision byte
 * Consistent with dBgW_KcMbg::DetectClsn(dBgCh_Gnd&), which writes
 * 0x44/0x48 and reads 0x44/0x48/0x4c.
 *
 * PROMOTED 2026-08-24 to a real MI declaration (notes/ctor-migration.md item
 * 9). The ROM's own RTTI record (_ZTS9dBgCh_Gnd at 0x02099224) states the
 * base list outright:
 *
 *     dBgCh    @ 0x00   polymorphic   (vptr store at +0)
 *     dBgPi    @ 0x10   polymorphic   (secondary block VTable_dBgPi_dBgCh_GndThunk;
 *                                     this IS the hit record at 0x10 below)
 *
 * The C++ branch declares exactly that; the C branch stays field-for-field
 * IDENTICAL to what this header carried before promotion, because .c
 * translation units reach into the 0x10 interior by the old member spelling.
 */
#ifndef DBGCH_GND_H
#define DBGCH_GND_H
#include "types.h"

#ifdef __cplusplus

#include "dBgCh.h"
#include "dBgPi.h"

/* fwd */
struct dActor_c;

struct dBgCh_Gnd : dBgCh, dBgPi {
    /* 0x10..0x37 is the dBgPi base sub-object itself -- THE HIT RECORD,
       written by dBgW_Kc::DetectClsn through func_02037fd4(&ray->base, ...).

       THE PROBE STATE, supplied by the caller of DetectClsn and overwritten
       with the answer: pos is the search position (read .z, .x, .y by the
       collider walk); clsnY is the floor height seeded on entry and the hit
       on exit; hasClsn says whether the walk found anything at all. unk_04c
       the constructor seeds 0x1f4000 -- a Fix12i of 496.0, the default probe
       height above pos. */
    Vector3 pos;            /* 0x038 */
    Fix12i clsnY;           /* 0x044 */
    u8  hasClsn;            /* 0x048 */
    u8  pad_049[0x3];
    s32 unk_04c;            /* 0x04c */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the key-function
     * arrangement from include/dBgCh.h / include/dBgPi.h. No TU defines it
     * out of line, so no TU emits the vtable blocks either; the ROM supplies
     * _ZTV9dBgCh_Gnd and VTable_dBgPi_dBgCh_GndThunk as data.
     */
    virtual ~dBgCh_Gnd();

    /* DECLARED, defined out of line in src/_ZN9dBgCh_GndC1Ev.cpp as real
     * C++ -- complete-object context for every ROM caller, hence C1.
     * The ROM kept no C2 sibling: nothing derives from dBgCh_Gnd.
     */
    dBgCh_Gnd();

    /* methods */
    void SetObjAndPos(const Vector3 &pos, dActor_c *actor);
    int DetectClsn();
};

/* Size is the flat span the .c branch spells below AND the stride the ~50
   stack stand-ins in src/ reserve. */
typedef char dBgCh_Gnd_size_must_be_0x50[sizeof(dBgCh_Gnd) == 0x50 ? 1 : -1];

#else

struct dBgCh_Gnd {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 - first byte of the 0x28-byte dBgPi
                                       the hit is written into; kept as a byte
                                       because RaycastGroundC1/D1 spell it */
    u8  pad_011[0x27];
    Vector3 pos;            /* 0x038 */
    Fix12i clsnY;           /* 0x044 */
    u8  hasClsn;            /* 0x048 */
    u8  pad_049[0x3];
    s32 unk_04c;            /* 0x04c */

#ifdef __cplusplus
    /* methods */
    void SetObjAndPos(const Vector3 & vec_, dActor_c * actor_);
#endif
};

typedef struct dBgCh_Gnd dBgCh_Gnd;

#endif /* __cplusplus */

#endif
