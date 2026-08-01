/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Model: 21 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MODEL_H
#define MODEL_H
#include "types.h"

/* fwd */
struct a;
struct arg1_;
struct arg2_;
struct b;
struct file_;
struct id;
struct mode;
struct ov0ID_;
struct r1;
struct r2;
struct Model {
    u8  pad_000[0x4];
    u32 unk_004;            /* 0x004 */
    s32 unk_008;            /* 0x008 */
    u32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    u32 unk_014;            /* 0x014 */
    s32 unk_018;            /* 0x018 */
    u32 unk_01c;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    u32 unk_024;            /* 0x024 */
    s32 unk_028;            /* 0x028 */
    s32 unk_02c;            /* 0x02c */
    s32 unk_030;            /* 0x030 */
    s32 unk_034;            /* 0x034 */
    u8  pad_038[0x14];
    u8  unk_04c;            /* 0x04c */
#ifdef __cplusplus
    /* methods */
    int DoSetFile(char * file_, int a, int b);
    void HideMaterial(int r1, int r2);
    void LoadAndSetFile(unsigned short ov0ID_, int arg1_, int arg2_);
    void SetPolygonID(int id);
    void SetPolygonMode(int mode);
    void ShowMaterial(int r1, int r2);
    void UpdateVerts();
#endif
};

#endif
