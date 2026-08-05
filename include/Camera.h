/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Camera: 12 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CAMERA_H
#define CAMERA_H
#include "types.h"

/* fwd */
struct j;
struct Camera {
    u8  pad_000[0x80];
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    s32 unk_08c;            /* 0x08c */
    s32 unk_090;            /* 0x090 */
    u8  unk_094;            /* 0x094 */
    u8  pad_095[0x1b];
    s32 unk_0b0;            /* 0x0b0 */
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 */
    s32 unk_0bc;            /* 0x0bc */
    s32 unk_0c0;            /* 0x0c0 */
    u8  unk_0c4;            /* 0x0c4 */
    u8  pad_0c5[0x4b];
    s32 unk_110;            /* 0x110 */
    u8  pad_114[0x24];
    s32 unk_138;            /* 0x138 */
    s32 unk_13c;            /* 0x13c */
    u8  pad_140[0x14];
    u32 unk_154;            /* 0x154 */
    u8  pad_158[0x4e];
    u8  unk_1a6;            /* 0x1a6 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    void GoBehindPlayer(unsigned int j);
    void OnPendingDestroy();
    void SaveCameraStateBeforeTalk();
#endif
};

#endif
