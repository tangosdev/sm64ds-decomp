#ifndef DMG3DESPMODEL_C_H
#define DMG3DESPMODEL_C_H

#include "ModelAnim.h"
#include "TextureSequence.h"
#include "dMg3DEspAnimSet_c.h"

/* The source spelling and ordinary method names are inferred from its sole
 * owner, dScMg3DEsp_c, and each method's role. The 0x21c-byte extent and
 * member lifetime are fixed by its exact constructor/destructor pair. */
struct dMg3DEspModel_c {
    typedef void (dMg3DEspModel_c::*State)();

    dMg3DEspModel_c();
    ~dMg3DEspModel_c();

    void ResetTransform();
    void Render();
    void Behavior();
    void Reset();
    int InitResources();

    s32 unk_000;                      /* 0x000 */
    s32 unk_004;                      /* 0x004 */
    s32 unk_008;                      /* 0x008 */
    ModelAnim mModelAnim;             /* 0x00c */
    TextureSequence mTextureSequence; /* 0x070 */
    dMg3DEspAnimSet_c mAnimSet;       /* 0x084 */
    u32 unk_204;                      /* 0x204 */
    s32 unk_208;                      /* 0x208 */
    s32 unk_20c;                      /* 0x20c */
    State mState;                     /* 0x210 */
    s16 mTextureFrame;                /* 0x218 */
    u8 mPolygonID;                    /* 0x21a */
    u8 pad_21b;                       /* 0x21b */
};

typedef char dMg3DEspModel_c_size_must_be_0x21c[
    sizeof(dMg3DEspModel_c) == 0x21c ? 1 : -1];

#endif
