/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FlameChomp: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLAMECHOMP_H
#define FLAMECHOMP_H
#include "types.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct FlameChomp {
    u8  pad_000[0x5c];
    /* dActor_c::mPosX -- dActor_c.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_05c;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x1c];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10FlameChompD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x138 */
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x160 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10FlameChompD0Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x160 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x1a0 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10FlameChompD0Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x1a0 */
    u8  unk_35c;            /* 0x35c */
    u8  pad_35d[0x4b];
    s32 unk_3a8;            /* 0x3a8 */
#ifdef __cplusplus
    /* --- vtable, own override --- */
    int OnYoshiTryEat();              /* slot 18, include/dActor_c.h --
                                          src/_ZN10FlameChomp13OnYoshiTryEatEv.c */

    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
