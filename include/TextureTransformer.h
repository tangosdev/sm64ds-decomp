#ifndef TEXTURETRANSFORMER_H
#define TEXTURETRANSFORMER_H

#include "types.h"
#include "Animation.h"
#include "math/Fix12.h"

/* Animation child that drives BTA-file playback, vtable _ZTV18TextureTransformer at 0x0208e7c4:
 * two slots, the destructor pair, nothing else. Update and Prepare are
 * plain methods.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 stays a C file.
 *
 * Prepare's ROM body is a 0xc long-call veneer (ldr ip, [pc]; bx ip;
 * .word func_02046b64) -- no argument shuffling, so the real body's
 * signature IS the call surface. The matched func_02046b64.c takes TWO
 * arguments and no this: the BMD's texture-name table and the BTA
 * object whose name entries it resolves against it. Every matched
 * caller (Tornado, Whirlpool, the ov006 users, daObjMcWater_c) passes
 * exactly those two, and the sibling veneers at 0x0201577c
 * (MaterialChanger) and 0x0201597c (TextureSequence) are called the
 * same way. Declared static below: a static member mangles identically,
 * and the earlier "(this, &model, &file)" reading of this comment cost
 * the PC port a wrong-register hunt.
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

    /* DECLARED, never defined as a method here -- src/_ZN18TextureTransformerC1Ev.cpp
       owns C1 (notes/ctor-migration.md section 2). */
    TextureTransformer();

    /* --- non-virtual --- */
    static void Prepare(BMD_File &model, BTA_File &animFile);
    void Update(ModelComponents &model);
    void SetFile(BTA_File &animFile, int flags, Fix12<int> speed,
                 u32 startFrame);        /* free function, wall 6az */

};

typedef char TextureTransformer_size_must_be_0x14[sizeof(TextureTransformer) == 0x14 ? 1 : -1];

#endif /* __cplusplus */

#endif
