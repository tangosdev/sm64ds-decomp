/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CapEnemy: 11 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CAPENEMY_H
#define CAPENEMY_H
#include "types.h"

/* fwd */
struct Vector3;
struct v;
struct v_;
struct CapEnemy {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x2f];
    u8  unk_08c;            /* 0x08c */
    u8  pad_08d[0x23];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  pad_0d4[0x3f];
    u8  unk_113;            /* 0x113 */
    u8  mModel;            /* 0x114 */
    u8  pad_115[0x4f];
    u8  unk_164;            /* 0x164 */
#ifdef __cplusplus
    /* methods */
    int DestroyIfCapNotNeeded();
    int GetCapEatenOffIt(const Vector3 & v_);
    struct Actor * ReleaseCap(const Vector3 & v_);
    struct Actor * RespawnIfHasCap();
    void RenderCapModel(const Vector3 * v);
    void Unk_02005d94();
#endif
};

#endif
