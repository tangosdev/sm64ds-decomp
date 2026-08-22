/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SnowmanHead: 5 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SNOWMANHEAD_H
#define SNOWMANHEAD_H
#include "types.h"
#include "Model.h"
#include "TextureSequence.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct SnowmanHead {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x48];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11SnowmanHeadD0Ev.c] */
    TextureSequence mTextureSequence;            /* 0x124 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11SnowmanHeadD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x138 */
    /* dBgCh_Actr member, named by the class's own destructor calling
       dBgCh_Actr's D1 at +0x16c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11SnowmanHeadD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x16c */
    /* Trailing remainder, 0x10 bytes. All three markers are typed and the
       last one ends at 0x328; SnowmanHead_Spawn allocates 0x338. Nothing in
       the five recovered functions reads this range -- the state machine that
       does lives in func_ov072_02120560 / func_ov072_021205d4, which are not
       decompiled -- so it stays explicit padding rather than invented fields. */
    u8  pad_328[0x10];
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
#endif
};

typedef char SnowmanHead_size_must_be_0x338[sizeof(struct SnowmanHead) == 0x338 ? 1 : -1];

#endif
