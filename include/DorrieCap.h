/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class DorrieCap: 5 matched functions, 13 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DORRIECAP_H
#define DORRIECAP_H
#include "types.h"
#include "Model.h"
#include "MovingCylinderClsn.h"

struct DorrieCap {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x20];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 mEatingPlayer;            /* 0x0d0 */
    u8  unk_0d4;            /* 0x0d4 */
    u8  pad_0d5[0x1a];
    u8  unk_0ef;            /* 0x0ef */
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0f0 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9DorrieCapD1Ev.c] */
    Model mModel;            /* 0x0f0 */
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x140 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9DorrieCapD1Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x140 */
    s32 unk_174;            /* 0x174 */
#ifdef __cplusplus
    /* --- vtable, own override --- */
    int OnYoshiTryEat();              /* slot 18, include/dActor_c.h --
                                          src/_ZN9DorrieCap13OnYoshiTryEatEv.c */

    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
