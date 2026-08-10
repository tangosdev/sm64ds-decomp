/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserSkyPlatform: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERSKYPLATFORM_H
#define BOWSERSKYPLATFORM_H
#include "types.h"
#include "Model.h"

struct BowserSkyPlatform {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x48];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN17BowserSkyPlatformD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    u8  mMovingCylinderClsnWithPos;            /* 0x124 */
    u8  pad_125[0x4b];
    s32 unk_170;            /* 0x170 */
    s32 unk_174;            /* 0x174 */
    s32 unk_178;            /* 0x178 */
    s32 unk_17c;            /* 0x17c */
    s32 unk_180;            /* 0x180 */
    s32 unk_184;            /* 0x184 */
    u8  pad_188[0x20];
    s32 unk_1a8;            /* 0x1a8 */
    u8  pad_1ac[0x2];
    u8  unk_1ae;            /* 0x1ae */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
