/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Bully: 6 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BULLY_H
#define BULLY_H
#include "types.h"
#include "ModelAnim.h"
#include "WithMeshClsn.h"
#include "MovingCylinderClsn.h"

struct Bully {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x2c];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x36];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x43];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x110 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x174 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5BullyD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x174 */
    s32 mFileTable;            /* 0x330 */
    u8  pad_334[0x8];
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x33c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN5BullyD1Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x33c */
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x8b];
    /* An actor unique ID, not a count: Behavior passes it to
       Actor::FindWithID and increments the byte at +0x3fe of whatever comes
       back; InitResources zeroes it. Left unnamed because that is as far as
       the bytes go -- BigBully's u8 at the same offset is a different field
       with a different use, so the offset is no guide. */
    s32 unk_3fc;            /* 0x3fc */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
    void InitResources();
#endif
};

#endif
