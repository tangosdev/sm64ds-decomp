#ifndef DEXTANMTEXSRT_C_H
#define DEXTANMTEXSRT_C_H

#include "types.h"
#include "dExtFrameCtrl_c.h"
#include "math/Fix12.h"

/* dExtFrameCtrl_c child that drives BTA-file playback, vtable _ZTV15dExtAnmTexSRT_c at 0x0208e7c4:
 * two slots, the destructor pair, nothing else. Update and Prepare are
 * plain methods.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/dExtModel_c.h for the key-function rule and the objisolate exemption
 * to it. D0 emits beside it from the one definition.
 *
 * Prepare's ROM body is a 0xc long-call veneer (ldr ip, [pc]; bx ip;
 * .word func_02046b64) -- no argument shuffling, so the real body's
 * signature IS the call surface. The matched func_02046b64.c takes TWO
 * arguments and no this: the BMD's texture-name table and the BTA
 * object whose name entries it resolves against it. Every matched
 * caller (daTor_c, daWater_Tatumaki_c, the ov006 users, daObjMcWater_c) passes
 * exactly those two, and the sibling veneers at 0x0201577c
 * (dExtAnmMaterial_c) and 0x0201597c (dExtAnmTexPat_c) are called the
 * same way. Declared static below: a static member mangles identically,
 * and the earlier "(this, &model, &file)" reading of this comment cost
 * the PC port a wrong-register hunt.
 * SetFile is a real member under its scalar mangling: speed is a Fix12i
 * and startFrame a u16. Measured against the ROM body, a u32 startFrame
 * adds a truncation (0x5c, not 0x54) and Fix12<int> by value spills it
 * through the stack (0x6c); the sibling dExtAnmTexPat_c::SetFile is the
 * same body under the same spelling.
 */

#ifdef __cplusplus

struct ModelComponents;
struct BMD_File;
struct BTA_File { u16 numFrames; };
struct dExtAnmTexSRT_c : dExtFrameCtrl_c {
    BTA_File *file;           /* 0x10 */

    /* --- vtable: the destructor pair only. --- */
    virtual ~dExtAnmTexSRT_c(); /* slots 0 (D1), 1 (D0) */

    dExtAnmTexSRT_c();

    /* --- non-virtual --- */
    static void Prepare(BMD_File &model, BTA_File &animFile);
    void Update(ModelComponents &model);
    void SetFile(BTA_File &animFile, int flags, Fix12i speed, u16 startFrame);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtAnmTexSRT_c_size_must_be_0x14[sizeof(dExtAnmTexSRT_c) == 0x14 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
