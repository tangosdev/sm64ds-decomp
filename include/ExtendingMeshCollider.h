/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ExtendingMeshCollider: 10 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef EXTENDINGMESHCOLLIDER_H
#define EXTENDINGMESHCOLLIDER_H
#include "types.h"

/* fwd */
struct RaycastGround;
struct RaycastLine;
struct SphereClsn;
struct Vector3;
struct ground_;
struct idx_;
struct line_;
struct res_;
struct sphere_;
struct ExtendingMeshCollider {
    u8  pad_000[0x20];
    u8  unk_020;            /* 0x020 */
    u8  pad_021[0x2f];
    s32 mScale;            /* 0x050 */
    u8  pad_054[0x110];
    s32 unk_164;            /* 0x164 */
    u8  pad_168[0x60];
    u8  unk_1c8;            /* 0x1c8 */
    u8  pad_1c9[0x3];
    u8  unk_1cc;            /* 0x1cc */
#ifdef __cplusplus
    /* methods */
    int DetectClsn(RaycastGround & ground_);
    int DetectClsn(RaycastLine & line_);
    int DetectClsn(SphereClsn & sphere_);
    void GetNormal(short idx_, Vector3 & res_);
    void Virtual08();
#endif
};

#endif
