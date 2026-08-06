/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Stage: 15 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef STAGE_H
#define STAGE_H
#include "types.h"

struct Stage {
    u8  pad_000[0x4];
    /* 0x004..0x050 is ActorBase's, and ActorBase.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 unk_004;                 /* 0x004 */
    u32 param1;                  /* 0x008 */
    u16 actorID;                 /* 0x00c */
    u8  aliveState;              /* 0x00e */
    u8  shouldBeKilled;          /* 0x00f */
    u8  unk_010;                 /* 0x010 */
    u8  unk_011;                 /* 0x011 */
    u8  unk_012;                 /* 0x012 */
    u8  unk_013;                 /* 0x013 */
    u8  sceneNode[0x14];               /* 0x014 */
    u8  behavNode[0x10];               /* 0x028 */
    u8  renderNode[0x10];              /* 0x038 */
    u8  pad_048[0x8];
    u8  unk_050;            /* 0x050 */
    u8  pad_051[0x81b];
    u8  unk_86c;            /* 0x86c */
    u8  pad_86d[0x7];
    u8  unk_874;            /* 0x874 */
    u8  pad_875[0x47];
    u8  unk_8bc;            /* 0x8bc */
    u8  pad_8bd[0x5f];
    u8  unk_91c;            /* 0x91c */
    u8  pad_91d[0x4f];
    u8  unk_96c;            /* 0x96c */
    u8  pad_96d[0x1f];
    u8  unk_98c;            /* 0x98c */
    u8  pad_98d[0x7];
    u8  unk_994;            /* 0x994 */
    u8  pad_995[0x1f];
    u8  unk_9b4;            /* 0x9b4 */
    u8  unk_9b5;            /* 0x9b5 */
    u8  unk_9b6;            /* 0x9b6 */
    u8  pad_9b7[0x1];
    u8  unk_9b8;            /* 0x9b8 */
    u8  pad_9b9[0x3];
    u8  unk_9bc;            /* 0x9bc */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    void LoadFog();
    void LoadSkybox();
    void LoadTextureTransformers();
    void OnPendingDestroy();
    void RenderFog();
    void RenderModel();
    void RenderModelTransparent();
#endif
};

#endif
