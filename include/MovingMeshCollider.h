/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MovingMeshCollider: 14 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MOVINGMESHCOLLIDER_H
#define MOVINGMESHCOLLIDER_H
#include "types.h"

/* fwd */
struct Matrix4x3;
struct RaycastGround;
struct RaycastLine;
struct SphereClsn;
struct Vector3;
struct a_;
struct ground_;
struct idx_;
struct mtx_;
struct ray_;
struct res_;
struct sphere_;
struct MovingMeshCollider {
    u8  pad_000[0x20];
    u8  unk_020;            /* 0x020 */
    u8  pad_021[0x2f];
    s32 mScale;            /* 0x050 */
    u8  unk_054;            /* 0x054 */
    u8  pad_055[0x2f];
    u8  unk_084;            /* 0x084 */
    u8  pad_085[0x2f];
    u8  unk_0b4;            /* 0x0b4 */
    u8  pad_0b5[0x2f];
    u8  unk_0e4;            /* 0x0e4 */
    u8  pad_0e5[0x2f];
    s16 unk_114;            /* 0x114 */
    s16 unk_116;            /* 0x116 */
    s32 unk_118;            /* 0x118 */
    s32 unk_11c;            /* 0x11c */
    s32 unk_120;            /* 0x120 */
    s32 unk_124;            /* 0x124 */
    s32 unk_128;            /* 0x128 */
    s32 unk_12c;            /* 0x12c */
    u8  unk_130;            /* 0x130 */
    u8  pad_131[0x3];
    u8  unk_134;            /* 0x134 */
    u8  pad_135[0x2f];
    s32 unk_164;            /* 0x164 */
    u8  unk_168;            /* 0x168 */
    u8  pad_169[0x2f];
    u8  unk_198;            /* 0x198 */
#ifdef __cplusplus
    /* methods */
    int DetectClsn(RaycastGround & ground_);
    int DetectClsn(RaycastLine & ray_);
    int DetectClsn(SphereClsn & sphere_);
    short GetAngularVelY();
    void GetNormal(short idx_, Vector3 & res_);
    void GetTriangleOrigin(short idx_, Vector3 & res_);
    void Transform(const Matrix4x3 & mtx_, short a_);
    void Virtual08();
#endif
};

#endif
