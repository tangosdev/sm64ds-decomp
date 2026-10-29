#ifndef DEXTANMMATERIAL_C_H
#define DEXTANMMATERIAL_C_H

#include "types.h"
#include "dExtFrameCtrl_c.h"
#include "math/Fix12.h"

/* dExtFrameCtrl_c child that drives BMA-file playback, vtable _ZTV17dExtAnmMaterial_c at 0x0208e7f4:
 * two slots, the destructor pair, nothing else. Update is a plain method;
 * Prepare is static, for the reason set out below.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/dExtModel_c.h for the key-function rule and the objisolate exemption
 * to it. One ~dExtAnmMaterial_c() emits D0 and D1 together; the whole class
 * folds into src/engine/model/d_ext_model.cpp.
 *
 * PREPARE IS STATIC, on exactly the evidence set out in dExtAnmTexPat_c.h for
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
 * SetFile is a real member under its scalar mangling: speed is a Fix12i
 * and startFrame a u16. Measured against the ROM body, a u32 startFrame
 * adds a truncation (0x5c, not 0x54) and Fix12<int> by value spills it
 * through the stack (0x6c); the sibling dExtAnmTexPat_c::SetFile is the
 * same body under the same spelling.
 */

#ifdef __cplusplus

struct ModelComponents;
struct BMD_File;
struct BMA_File { u16 numFrames; };
struct dExtAnmMaterial_c : dExtFrameCtrl_c {
    BMA_File *file;           /* 0x10 */

    /* --- vtable: the destructor pair only. --- */
    virtual ~dExtAnmMaterial_c(); /* slots 0 (D1), 1 (D0) */

    /* DEFINED in the promoted TU -- one dExtAnmMaterial_c() emits C1 (and a
       deadstripped C2 sibling); see notes/ctor-migration.md section 2. */
    dExtAnmMaterial_c();

    /* --- non-virtual --- */
    void Update(ModelComponents &model);
    void SetFile(BMA_File &animFile, int flags, Fix12i speed, u16 startFrame);

    /* --- static --- */
    static void Prepare(BMD_File &model, BMA_File &animFile);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtAnmMaterial_c_size_must_be_0x14[sizeof(dExtAnmMaterial_c) == 0x14 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
