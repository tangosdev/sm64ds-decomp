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
 * the base, and RestoreInitial's own zero pass touches only mSoundHandle (see
 * below) -- read exhaustively, it says nothing about the other four.
 *
 * SaveSnapshot is a three-way state machine gated by two level-indexed
 * byte tables (data_020a0de8/de9) and driven by two more (data_020a0dea/
 * deb, both shifted left 12 to become fixed-point):
 *   - "trigger" (a new flag byte set): sets the base's unk_031, zeroes the
 *     base's mVel0/mVel1, latches unk_03c/mDriveNow from the tables and
 *     mirrors each into unk_034/mDriveBase -- then, only if mCurrent1 is above
 *     the 0xa0000 baseline, pulls mDriveBase down by the excess.
 *   - "active" (unk_031 already 1): recomputes mDriveNow from the table each
 *     call and clamps it to [mDriveBase, mDriveBase + 0x38000], recomputes
 *     mCurrent1 from mDriveNow - mDriveBase + 0xa0000, and plays a sound via
 *     Sound_PlayIfNotActive (stored back into mSoundHandle) whenever that new
 *     mCurrent1 differs from the snapshot mSnapshot1.
 *   - "release": clears the base's unk_031/mVel0/mVel1, OR (if neither
 *     the trigger nor the release condition holds) eases mCurrent1 back
 *     toward the 0xa0000 baseline using the base's mVel1 as velocity.
 *
 * The original spelled three of these updates through a raw pointer cast
 * that reads and writes the SAME address twice -- `int *p = (int *)(c +
 * off); *p = *p - k;` -- solely to keep a compound assignment off the
 * struct member. All three drop to a plain compound assignment on the real
 * member and still reproduce byte-exact.
 *
 * unk_034 and unk_03c are set together, once, in the "trigger" branch
 * (unk_034 = unk_03c) and never read again -- not by this class's own three
 * functions and not by anything else in the tree. They keep unk_ names on
 * purpose: their width and their write are evidenced, their meaning is not.
 *
 * mSoundHandle is the sound handle Sound_PlayIfNotActive returns; RestoreInitial
 * zeroes it and it is the ONLY offset RestoreInitial's own zero pass
 * reaches -- unk_034/mDriveBase/unk_03c/mDriveNow carry whatever SaveSnapshot
 * last left there across a restore.
 *
 * FIELD NAMES and their evidence are tabulated in
 * notes/smartball-provenance.md.
 *
 * CONSTRUCTED BY func_ov006_0211157c, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. Unlike every sibling, its only caller is the unmatched
 * func_ov006_02118b70 -- func_ov006_02115b0c.c never allocates a spring --
 * so this migration has fewer literal call sites to rename than usual. It
 * sets the base's mRadius to 0x20000, the per-child constant every sibling
 * constructor also writes; it does not touch anything from 0x34 to 0x44. */
#ifndef CMGSMARTBALL_SPRING_C_H
#define CMGSMARTBALL_SPRING_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_spring_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034;     /* 0x034 -- copied from unk_03c in the trigger branch and
                          never read again by anything in the tree. No evidence
                          of a use, so no name. */
    s32 mDriveBase;  /* 0x038 -- the drive reading captured at the trigger frame,
                          less however far mCurrent1 already sat above the
                          0xa0000 rest height. Every active frame the spring's
                          height is recomputed as
                          `mCurrent1 = mDriveNow - mDriveBase + 0xa0000`, so this
                          is the zero point that reading is measured from. */
    s32 unk_03c;     /* 0x03c -- data_020a0dea[level*4] << 12, latched once in
                          the trigger branch (and mirrored into unk_034).
                          Nothing in the tree reads either back. No name. */
    s32 mDriveNow;   /* 0x040 -- data_020a0deb[level*4] << 12, the live drive
                          reading. Latched at trigger, then re-read every active
                          frame and clamped to
                          [mDriveBase, mDriveBase + 0x38000] before it sets the
                          height. */
    s32 mSoundHandle;/* 0x044 -- the handle Sound_PlayIfNotActive returns, fed
                          straight back in as its own first argument on the next
                          call so the compression sound is not restarted. The
                          only field this class's RestoreInitial zeroes. */
};

typedef char cMgSmartball_spring_c_size_must_be_0x48[sizeof(cMgSmartball_spring_c) == 0x48 ? 1 : -1];

#endif
