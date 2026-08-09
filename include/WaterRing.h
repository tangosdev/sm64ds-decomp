/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WaterRing: 5 matched functions, 19 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WATERRING_H
#define WATERRING_H
#include "types.h"
#include "Model.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct WaterRing {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x74];
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    s16 unk_096;            /* 0x096 */
    u8  pad_098[0x68];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9WaterRingD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9WaterRingD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x150 */
    /* Model member, named by _ZN5ModelD1Ev at +0x30c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x30c */
    u8  mTextureTransformer;            /* 0x35c */
    u8  pad_35d[0xb];
    s32 unk_368;            /* 0x368 */
    u8  pad_36c[0x4];
    u8  unk_370;            /* 0x370 */
    u8  pad_371[0xb];
    s32 unk_37c;            /* 0x37c */
    s8  unk_380;            /* 0x380 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
