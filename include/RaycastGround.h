/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RaycastGround: 3 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * HAND-EXTENDED for MeshCollider::DetectClsn(RaycastGround&) at 0x01ffd3f8:
 *   0x010  the ClsnResult the hit is written into (func_02037fd4(ray+0x10, ...))
 *   0x038  the probe position, a Fix12i Vector3, read as .x, .z, .y
 *   0x044  the collision height in Fix12i: the search seed on entry, the hit on exit
 *   0x048  the has-collision byte
 * Consistent with MovingMeshCollider::DetectClsn(RaycastGround&), which writes
 * 0x44/0x48 and reads 0x44/0x48/0x4c.
 */
#ifndef RAYCASTGROUND_H
#define RAYCASTGROUND_H
#include "types.h"

/* fwd */
struct Actor;
struct actor_;
struct vec_;
struct RaycastGround {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 - first byte of the 0x28-byte ClsnResult
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
    void SetObjAndPos(const Vector3 & vec_, Actor * actor_);
#endif
};

#endif
