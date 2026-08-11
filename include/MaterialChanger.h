#ifndef MATERIALCHANGER_H
#define MATERIALCHANGER_H

#include "types.h"
#include "Animation.h"
#include "math/Fix12.h"

/* Animation child that drives BMA-file playback, vtable _ZTV15MaterialChanger at 0x0208e7f4:
 * two slots, the destructor pair, nothing else. Update is a plain method;
 * Prepare is static, for the reason set out below.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 stays a C file.
 *
 * PREPARE IS STATIC, on exactly the evidence set out in TextureSequence.h for
 * the sibling veneer. Its ROM body is a 0xc long-call veneer (ldr ip, [pc];
 * bx ip; .word func_020470e8) that shuffles no arguments, and func_020470e8
 * reads only r0 and r1:
 *
 *     push {r4-r7,lr} / sub sp,sp,#4
 *     mov  r6, r1      <- ldrh [r6,#8] is the loop count,
 *     mov  r7, r0         ldr  [r6,#0xc] the 0x3c-stride records it walks
 *
 * r2 is never touched, so there is no third argument and hence no this; r0 is
 * the BMD, forwarded to the name-lookup helper. A static member mangles
 * identically, so the symbol is unchanged.
 * SetFile's definition stays a mangled free function (wall 6az,
 * Fix12<int> in the signature); the declaration below is the real one.
 */

#ifdef __cplusplus

struct ModelComponents;
struct BMD_File;
struct BMA_File { u16 numFrames; };
struct MaterialChanger : Animation {
    BMA_File *file;           /* 0x10 */

    /* --- vtable: the destructor pair only. --- */
    virtual ~MaterialChanger();                       /* slots 0 (D1), 1 (D0) */

    /* --- non-virtual --- */
    void Update(ModelComponents &model);
    void SetFile(BMA_File &animFile, int flags, Fix12<int> speed,
                 u32 startFrame);        /* free function, wall 6az */

    /* --- static --- */
    static void Prepare(BMD_File &model, BMA_File &animFile);
};

typedef char MaterialChanger_size_must_be_0x14[sizeof(MaterialChanger) == 0x14 ? 1 : -1];

#endif /* __cplusplus */

#endif
