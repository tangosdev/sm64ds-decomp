/* class cMgSmartball_spring_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ecfc,
 * RTTI ov006:0x0213ec64 (_ZTI21cMgSmartball_spring_c),
 * _ZTS21cMgSmartball_spring_c ov006:0x0213ee50.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x48, NOT FROM func_ov006_02115b0c.c LIKE EVERY OTHER SIBLING.
 * spring's allocation site is inside func_ov006_02118b70, which has no
 * matched source and is unmatched (out of scope for this migration). The
 * literal was read straight off the ROM: `MOV r0, #0x48` immediately
 * before `BL _Znwj` at 0x02119098, with the constructor called at
 * 0x021190c0. Base ends at 0x34, so this class adds 0x14 bytes -- five
 * s32 fields.
 *
 * FIELD EVIDENCE is SaveSnapshot alone; Update never touches offsets past
 * the base, and RestoreInitial's own zero pass touches only unk_044 (see
 * below) -- read exhaustively, it says nothing about the other four.
 *
 * SaveSnapshot is a three-way state machine gated by two level-indexed
 * byte tables (data_020a0de8/de9) and driven by two more (data_020a0dea/
 * deb, both shifted left 12 to become fixed-point):
 *   - "trigger" (a new flag byte set): sets the base's unk_031, zeroes the
 *     base's unk_020/unk_024, latches unk_03c/unk_040 from the tables and
 *     mirrors each into unk_034/unk_038 -- then, only if mCurrent1 is above
 *     the 0xa0000 baseline, pulls unk_038 down by the excess.
 *   - "active" (unk_031 already 1): recomputes unk_040 from the table each
 *     call and clamps it to [unk_038, unk_038 + 0x38000], recomputes
 *     mCurrent1 from unk_040 - unk_038 + 0xa0000, and plays a sound via
 *     Sound_PlayIfNotActive (stored back into unk_044) whenever that new
 *     mCurrent1 differs from the snapshot mSnapshot1.
 *   - "release": clears the base's unk_031/unk_020/unk_024, OR (if neither
 *     the trigger nor the release condition holds) eases mCurrent1 back
 *     toward the 0xa0000 baseline using the base's unk_024 as velocity.
 *
 * The original spelled three of these updates through a raw pointer cast
 * that reads and writes the SAME address twice -- `int *p = (int *)(c +
 * off); *p = *p - k;` -- solely to keep a compound assignment off the
 * struct member. All three drop to a plain compound assignment on the real
 * member and still reproduce byte-exact.
 *
 * unk_034 and unk_03c are set together, once, in the "trigger" branch
 * (unk_034 = unk_03c) and never read again by this class's own three
 * functions; nothing here says what reads them afterward.
 *
 * unk_044 is the sound handle Sound_PlayIfNotActive returns; RestoreInitial
 * zeroes it and it is the ONLY offset RestoreInitial's own zero pass
 * reaches -- unk_034/038/03c/040 carry whatever SaveSnapshot last left
 * there across a restore.
 *
 * CONSTRUCTED BY func_ov006_0211157c, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. Unlike every sibling, its only caller is the unmatched
 * func_ov006_02118b70 -- func_ov006_02115b0c.c never allocates a spring --
 * so this migration has fewer literal call sites to rename than usual. It
 * sets the base's unk_028 to 0x20000, the per-child constant every sibling
 * constructor also writes; it does not touch unk_034..unk_044. */
#ifndef CMGSMARTBALL_SPRING_C_H
#define CMGSMARTBALL_SPRING_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_spring_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034; /* 0x034 -- set once from unk_03c on trigger; see header comment */
    s32 unk_038; /* 0x038 -- rest-length baseline; see header comment */
    s32 unk_03c; /* 0x03c -- table-driven value mirrored into unk_034 on trigger */
    s32 unk_040; /* 0x040 -- table-driven compressed height, clamped each active frame */
    s32 unk_044; /* 0x044 -- Sound_PlayIfNotActive's handle; the only field RestoreInitial zeroes */
};

typedef char cMgSmartball_spring_c_size_must_be_0x48[sizeof(cMgSmartball_spring_c) == 0x48 ? 1 : -1];

#endif
