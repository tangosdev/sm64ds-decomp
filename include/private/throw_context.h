#ifndef PRIVATE_THROW_CONTEXT_H
#define PRIVATE_THROW_CONTEXT_H

#include "types.h"

/* CodeWarrior exception runtime: the register/frame state the unwinder walks
 * (func_02072168 interprets actions against it, func_02071910 unwinds one
 * frame, func_02071f1c hands the exception to a handler). Total size is not
 * evidenced; the last field read is mArgsPushed at 0x6a. */
typedef struct ThrowContext {
    int mThrowType; /* 0x00 -- type string; func_02071ccc tests it for '*' */
    int mLocation;  /* 0x04 -- thrown object; matched against a CatchInfo */
    int mDtor;      /* 0x08 -- thrown object's destructor (case 0xd) */
    char pad_0c[0x8];
    char *mSP; /* 0x14 -- func_02071910: mFP + frame size after unwind */
    /* 0x18 -- base for frame-relative operands; set from mSP, r7 or r11 by
     * func_02071864 */
    char *mFP;
    /* 0x1c -- r0..r15; func_02071910 restores the saved registers here,
     * mGPR[14] is the return address */
    int mGPR[16];
    char pad_5c[4];
    /* 0x60 -- frame size, an unsigned LEB128 from the header; the save area
     * ends at mFP + mFrameSize */
    int mFrameSize;
    char pad_64[4];
    /* 0x68 -- the saved registers, bit n = rn: header byte 1 << 4 (r4..r11)
     * plus lr (0x4000) */
    u16 mSavedRegMask;
    /* 0x6a -- header flag 0x20: the save area ends 0x10 bytes lower, as when
     * a prologue pushes r0..r3 above the saved registers (medium) */
    u8 mArgsPushed;
} ThrowContext;

#ifndef SM64DS_PLATFORM_PC
typedef char ThrowContext_args_pushed_must_be_at_0x6a[(int)&((ThrowContext *)0)->mArgsPushed == 0x6a
                                                          ? 1
                                                          : -1];
#endif

#endif
