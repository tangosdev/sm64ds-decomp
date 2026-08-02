#ifndef TEXTURETRANSFORMER_H
#define TEXTURETRANSFORMER_H

#include "types.h"
#include "Animation.h"
#include "math/Fix12.h"

/* Animation child that drives BTA-file playback, vtable _ZTV18TextureTransformer at 0x0208e7c4:
 * two slots, the destructor pair, nothing else. Update and Prepare are
 * plain methods.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- see
 * include/ModelBase.h. The structors stay self-contained C files.
 *
 * Prepare's ROM body is a 0xc tail-call veneer into func_02046b64, which is the
 * real (still unnamed) implementation taking (this, &model, &file).
 * SetFile's definition stays a mangled free function (wall 6az,
 * Fix12<int> in the signature); the declaration below is the real one.
 */

#ifdef __cplusplus

struct ModelComponents;
struct BMD_File;
struct BTA_File { u16 numFrames; };
struct TextureTransformer : Animation {
    BTA_File *file;           /* 0x10 */

    /* --- vtable: the destructor pair only. --- */
    virtual ~TextureTransformer();                       /* slots 0 (D1), 1 (D0) */

    /* --- non-virtual --- */
    void Prepare(BMD_File &model, BTA_File &animFile);
    void Update(ModelComponents &model);
    void SetFile(BTA_File &animFile, int flags, Fix12<int> speed,
                 u32 startFrame);        /* free function, wall 6az */

};

typedef char TextureTransformer_size_must_be_0x14[sizeof(TextureTransformer) == 0x14 ? 1 : -1];

#endif /* __cplusplus */

#endif
