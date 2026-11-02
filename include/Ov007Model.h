#ifndef OV007_MODEL_H
#define OV007_MODEL_H

#include "types.h"
#include "math/Matrix.h"

/* Reconstructed layouts and names, not recovered class identity: the ov007
 * menu's animated model record and the animation player each of its
 * animation slots holds. */

struct ModelComponents;
struct BCA_File;
struct BTA_File;
struct BTP_File;

/* The 0x1c-byte animation player func_ov007_020c92ac allocates,
 * func_ov007_020c9268 resets, func_ov007_020c9214 starts and
 * func_ov007_020c9110 ticks. */
typedef struct Ov007AnimPlayer {
    /* 0x00: 0 playing; 1 held (func_ov007_020c9110 leaves the frame alone);
     * func_ov007_020c9110 sets 2 when a one-shot ends */
    s32 state;
    /* 0x04: what func_ov007_020c9110 does at either end: 0 stops (state 2),
     * 1 wraps to the other end, 2 negates speed and plays back */
    s32 loopMode;
    /* 0x08: bit 0, set by func_ov007_020c9214: func_ov007_020c9110 skips
     * one advance and clears it, so the start frame is shown once */
    s32 flags;
    /* 0x0c: index into the owner's animation table; -1 none. volatile:
     * func_ov007_020b0a20 re-reads it for each of its three tests, and
     * without it mwccarm caches the load and the function is 8 bytes short. */
    volatile s16 anim;
    /* 0x0e: func_ov007_020c9214 moves the old anim here before it installs
     * the new one; no reader */
    s16 prevAnim;
    u16 frame;      /* 0x10: the whole frame func_ov007_020c9110 last reached */
    u16 frameCount; /* 0x12 */
    /* 0x14: Q12 frame position; func_ov007_020c9110 adds speed to it and
     * clamps it to 0 .. (frameCount - 1) << 12 */
    s32 framePos;
    s32 speed; /* 0x18: Q12 frames per tick; func_ov007_020c9214's callers pass 0x1000 */
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates and
 * func_ov007_020bc47c sets up. The three animation slots are (count, table,
 * player) triples that func_ov007_020bc6b4, func_ov007_020bc658 and
 * func_ov007_020bc5fc bind; func_ov007_020bc894 steps them. */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00: func_ov007_020bc6d4 builds it from the BMD file */
    /* 0x04: the pose model->transforms points at (one matrix per bone);
     * func_02046564-sized, as dExtSimpleModel_c's transformsBuf */
    struct Matrix4x3 *transforms;
    /* 0x08: flags & 2 only: the next frame's pose; on in-between frames
     * func_ov007_020bc02c averages it into transforms */
    struct Matrix4x3 *nextTransforms;
    /* 0x0c: flags & 4 only; func_020462bc-sized, as dExtBlendAnmModel_c's */
    void *blendBuffer;
    s32 numBoneAnims;              /* 0x10 */
    struct BCA_File **boneAnims;   /* 0x14: boneAnim->anim indexes it (UpdateBones) */
    Ov007AnimPlayer *boneAnim;     /* 0x18 */
    s32 numTexSrtAnims;            /* 0x1c */
    struct BTA_File **texSrtAnims; /* 0x20: func_020469e8, dExtAnmTexSRT_c */
    Ov007AnimPlayer *texSrtAnim;   /* 0x24 */
    s32 numTexSeqAnims;            /* 0x28 */
    struct BTP_File **texSeqAnims; /* 0x2c: func_02046bbc, dExtAnmTexPat_c */
    Ov007AnimPlayer *texSeqAnim;   /* 0x30 */
    Vector3 pos;   /* 0x34: func_ov007_020bc968 copies it into the matrix translation */
    Vector3 scale; /* 0x40: ModelComponents::Render's scale (func_ov007_020bbff0) */
    u16 rotX;      /* 0x4c: func_ov007_020c39f8's angles (func_ov007_020bc968) */
    u16 rotY;      /* 0x4e */
    u16 rotZ;      /* 0x50 */
    char pad_52[2];
    /* 0x54: what func_ov007_020bbff0 renders the model with;
     * func_ov007_020bc968 builds it */
    struct Matrix4x3 matrix;
    /* 0x84, medium confidence: one word per BMD material, allocated by
     * func_ov007_020bc6d4; func_ov007_020bc47c copies each material's +0x1c
     * into it, the word func_02044b30 writes to REG_TEXIMAGE_PARAM
     * (0x040004a8). Nothing in ov007 reads it back; func_ov007_020bc53c
     * frees it. */
    s32 *materialTexParams;
    /* 0x88: flags & 4 only; func_ov007_020bc7dc's cross-fade source pose */
    Ov007AnimPlayer *blendAnim;
    /* 0x8c: frames left of the cross-fade; func_ov007_020bc374 seeds it and
     * blendFrameCount with the same length */
    s16 blendFrames;
    s16 blendFrameCount; /* 0x8e: the cross-fade's length; blendFrames / it is the weight */
    char pad_90[4];
    /* 0x94: func_ov007_020bbff0 renders the model only while it is nonzero.
     * Set by func_ov007_020bc47c (when it binds the model's file),
     * func_ov007_020be980 (with the animation), func_ov007_020b2614,
     * func_ov007_020b0da0 (when states[1]'s stateTicks is 1) and
     * func_ov007_020bf790 (show). Cleared by func_ov007_020b066c,
     * func_ov007_020be9ac, the fade's constructor func_ov007_020bfcec,
     * func_ov007_020bfbdc, func_ov007_020bf790 (hide) and
     * func_ov007_020bf690 (at the end of the transition) */
    s32 visible;
    /* 0x98: 1 on the frames that step the animations; with flags & 3 it
     * alternates, so they run at half rate (func_ov007_020bc47c seeds 1) */
    s32 animTick;
    /* 0x9c: func_ov007_020bc6d4's second argument. 1/2 half-rate animation
     * (2 also fills the in-between frames), 4 cross-fade buffers, 8 skips
     * dExtSimpleModel_c::LoadTexAndPal (func_ov007_020bc47c) */
    s32 flags;
} Ov007Model;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007AnimPlayer_size_must_be_0x1c[sizeof(Ov007AnimPlayer) == 0x1c ? 1 : -1];
typedef char Ov007Model_size_must_be_0xa0[sizeof(Ov007Model) == 0xa0 ? 1 : -1];
#endif

#endif
