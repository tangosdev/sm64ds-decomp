/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RaycastGround: 3 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RAYCASTGROUND_H
#define RAYCASTGROUND_H
#include "types.h"

/* fwd */
struct Actor;
struct Vector3;
struct actor_;
struct vec_;
struct RaycastGround {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x3b];
    s32 unk_04c;            /* 0x04c */
#ifdef __cplusplus
    /* methods */
    void SetObjAndPos(const Vector3 & vec_, Actor * actor_);
#endif
};

#endif
