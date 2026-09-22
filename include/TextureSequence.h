#ifndef TEXTURESEQUENCE_H
#define TEXTURESEQUENCE_H

#include "types.h"
#include "Animation.h"
#include "math/Fix12.h"

/* Animation child that drives BTP-file playback, vtable _ZTV15TextureSequence at 0x0208e7d4:
 * two slots, the destructor pair, nothing else. Update is a plain method;
 * Prepare is static, for the reason set out below.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 stays a C file.
 *
 * PREPARE IS STATIC. Its ROM body is a 0xc long-call veneer (ldr ip, [pc];
 * bx ip; .word func_02046d50) that shuffles no arguments, so the real body's
 * register use IS the call surface -- and func_02046d50 reads only r0 and r1:
 *
 *     push {r4-r7,lr} / sub sp,sp,#4
 *     mov  r6, r1      <- ldrh [r6,#2], ldr [r6,#4], ldrh [r6,#8],
 *     mov  r7, r0         ldr [r6,#0xc], ldrh [r6,#0x1c], ldr [r6,#0x20]
 *
 * r2 is never touched, which rules out three arguments; and the offsets r1 is
 * read at are exactly BTP_File's fields below, so r1 is the ANIMATION file --
 * the second argument, not the third. r0 is the BMD, forwarded to the three
 * name-lookup helpers. Two arguments and no this. The veneer body is a pure
 * tail jump that reproduces under either signature, so the definition file
 * never constrained this; the callee and the callers do, and they agree.
 *
 * A static member mangles identically, so the symbol is unchanged. This is the
 * same correction already applied to the sibling TextureTransformer, whose
 * header names this veneer and MaterialChanger's as called the same way.
 * SetFile's definition stays a mangled free function (wall 6az,
 * Fix12<int> in the signature); the declaration below is the real one.
 */

#ifdef __cplusplus

struct ModelComponents;
struct BMD_File;
struct SharedFilePtr;

/* Only the header words UpdateFileOffsets touches are typed. */
struct BTP_File {
    u16 unk_00;
    u16 numTexRecords;         /* 0x02 - 8-byte records at unk_04 */
    char *unk_04;              /* 0x04 - fixed up on load */
    u16 numPalRecords;         /* 0x08 - 8-byte records at unk_0c */
    u16 pad_0a;
    char *unk_0c;              /* 0x0c */
    char *unk_10;              /* 0x10 */
    char *unk_14;              /* 0x14 */
    char *unk_18;              /* 0x18 */
    u16 numSeqRecords;         /* 0x1c - 0xc-byte records at unk_20; the ROM reads a halfword */
    u16 pad_1e;
    char *unk_20;              /* 0x20 */
};
struct TextureSequence : Animation {
    BTP_File *file;           /* 0x10 */

    /* --- vtable: the destructor pair only. --- */
    virtual ~TextureSequence();                       /* slots 0 (D1), 1 (D0) */

    /* HOST-ONLY VTABLE-SHAPE PIN. The ROM's _ZTV15TextureSequence has TWO
       slots, D1 at 0 and D0 at 1 (0208e7d4 = 02015a2c, 0208e7d8 =
       02015a00; the words are quoted in hal/model_dtor_seat.cpp). MSVC
       folds the destructor pair into one ??_E slot, so the host table had
       ONE slot -- and the ROM's own destructors for daDemo_c::anmModel_c
       hand-index slot 1 of this table, (*(VFN)((*(int **)p)[1]))(p), which
       on the host read the next class's RTTI pointer out of .rdata and
       called it (c0000005 at RVA 0x0043e298 = &??_R4MaterialChanger, on
       the opening cutscene). This declaration gives the host table the
       ROM's second slot, holding the ROM's own body for it, the DELETING
       half. __cdecl because the only readers are those four ROM bodies and
       their VFN typedef is a plain void (*)(void *) with the receiver
       pushed as a stack word; nothing MSVC generates ever indexes 1. Host
       only: mwccarm never sees it, so no matched body changes. */
#ifdef SM64DS_PLATFORM_PC
    virtual void __cdecl RomSlot1D0();      /* ROM _ZTV15TextureSequence[1] */
#endif

    /* DECLARED, never defined as a method here -- src/_ZN15TextureSequenceC1Ev.cpp
       owns C1 (notes/ctor-migration.md section 2). */
    TextureSequence();

    /* --- non-virtual --- */
    void Update(ModelComponents &model);
    void SetFile(BTP_File &animFile, int flags, Fix12<int> speed,
                 u32 startFrame);        /* free function, wall 6az */

    /* --- static --- */
    static void Prepare(BMD_File &model, BTP_File &animFile);
    static void *LoadFile(SharedFilePtr &ptr);
    static void UpdateFileOffsets(BTP_File &file);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char TextureSequence_size_must_be_0x14[sizeof(TextureSequence) == 0x14 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
