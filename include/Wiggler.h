#ifndef WIGGLER_H
#define WIGGLER_H

#include "types.h"

/* The Wiggler. Five body segments, so five of everything -- and EIGHT arrays,
 * which close on eight consecutive boundaries:
 *
 *     Enemy                        ends 0x110
 *     ModelAnim[5]                 0x110 + 5*0x64 = 0x304  -> MaterialChanger
 *     MaterialChanger[5]           0x304 + 5*0x14 = 0x368  -> TextureSequence
 *     TextureSequence[5]           0x368 + 5*0x14 = 0x3cc  -> the first triple
 *     Vector3[5]                   0x3cc + 5*0x0c = 0x408  -> the second
 *     Vector3[5]                   0x408 + 5*0x0c = 0x444  -> the shorts
 *     Vector3s[5]                  0x444 + 5*0x06 = 0x462
 *     MovingCylinderClsnWithPos[5] 0x478 + 5*0x40 = 0x5b8  -> the second set
 *     MovingCylinderClsnWithPos[5] 0x5b8 + 5*0x40 = 0x6f8
 *
 * The Vector3s array is what named func_02011508 -- see the note on Vector3s in
 * include/types.h and src/_ZN8Vector3sD1Ev.cpp.
 */

#ifdef __cplusplus

#include "Enemy.h"
#include "ModelAnim.h"
#include "MaterialChanger.h"
#include "TextureSequence.h"
#include "WithMeshClsn.h"
#include "MovingCylinderClsnWithPos.h"

struct Wiggler : Enemy {
    ModelAnim mModelAnims[5];                        /* 0x110 */
    MaterialChanger mMaterialChangers[5];            /* 0x304 */
    TextureSequence mTextureSequences[5];            /* 0x368 */
    Vector3 unk_3cc[5];                              /* 0x3cc */
    Vector3 unk_408[5];                              /* 0x408 */
    Vector3s unk_444[5];                             /* 0x444 */
    u8  pad_462[0x16];
    MovingCylinderClsnWithPos mCylinderClsns1[5];    /* 0x478 */
    MovingCylinderClsnWithPos mCylinderClsns2[5];    /* 0x5b8 */
    u8  pad_6f8[0x10];
    WithMeshClsn mWithMeshClsn;                      /* 0x708 */

    virtual ~Wiggler();

    int InitResources();
    int Render();
};

#else

/* The same object for a C translation unit, flat. */
struct Wiggler {
    u8  pad_000[0x708];
    u8  mWithMeshClsn[0x1bc];      /* 0x708 */
};

#endif /* __cplusplus */

#endif /* WIGGLER_H */
