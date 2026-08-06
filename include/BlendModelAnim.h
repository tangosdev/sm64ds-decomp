#ifndef BLENDMODELANIM_H
#define BLENDMODELANIM_H

#include "types.h"
#include "ModelAnim.h"

/* ModelAnim child that cross-fades between the previous pose and the new
 * animation, vtable _ZTV14BlendModelAnim at 0x0208e94c (same seven-slot
 * shape as ModelAnim, every slot overridden) plus its own
 * VTable_Animation_BlendModelAnimThunk secondary.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelAnim.h; the D1 additionally frees unk_6c before chaining
 * to ModelAnim::D2, which is why it stays a C file with real logic.
 *
 * LAYOUT: ModelAnim 0x00..0x64, then the blend state. blendWeight ramps
 * 0 -> 0x1000 (1.0) by blendStep per Advance; below 1.0 UpdateVerts and
 * Virtual10 route through the blending func_0204531c instead of
 * UpdateVertsUsingBones. unk_6c is a buffer the destructor frees and
 * func_020165c4 (DoSetFile's tail) sets up.
 *
 * SetAnim's definition stays a mangled free function (wall 6az).
 */

#ifdef __cplusplus

struct BlendModelAnim : ModelAnim {
    Fix12i blendWeight;        /* 0x64 - 0x1000 is 1.0 */
    Fix12i blendStep;          /* 0x68 - per-frame increment */
    void *unk_6c;              /* 0x6c - owned; D1 deletes it */

    /* --- vtable order. Do not reorder. --- */
    virtual ~BlendModelAnim();                            /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b);      /* slot 2 */
    virtual void UpdateVerts();                           /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);               /* slot 4 */
    virtual void Render(const Vector3 *scale);            /* slot 5 */
    virtual void Virtual18(u32 mat, const Vector3 *scale);/* slot 6 */

    /* --- non-virtual --- */
    void Advance();
    void SetAnim(BCA_File &animFile, int numBlendFrames, int flags,
                 Fix12<int> speed, u16 startFrame);  /* free function, wall 6az */
};

typedef char BlendModelAnim_size_must_be_0x70[sizeof(BlendModelAnim) == 0x70 ? 1 : -1];

#else

/* The same object for C translation units, both vptrs written out -- the same shape
 * ModelAnim.h gives its own C fallback, with the blend state appended. */
struct BlendModelAnim {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents data;       /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x1c */
    void *transformsBuf;               /* 0x4c */
    void **animVtable;                 /* 0x50 */
    u32 numFramesAndFlags;             /* 0x54 */
    s32 currFrame;                     /* 0x58 */
    s32 speed;                         /* 0x5c */
    struct BCA_File *file;             /* 0x60 */
    s32 blendWeight;                   /* 0x64 */
    s32 blendStep;                     /* 0x68 */
    void *unk_6c;                      /* 0x6c */
};

/* So an object header declaring a BlendModelAnim member reads the same in both modes:
 * C++ gets the class, C gets the flat stand-in above, and neither needs `struct`. */
typedef struct BlendModelAnim BlendModelAnim;

#endif /* __cplusplus */

#endif /* BLENDMODELANIM_H */
