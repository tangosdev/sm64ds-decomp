#ifndef PRIVATE_THROW_CONTEXT_H
#define PRIVATE_THROW_CONTEXT_H

#include "types.h"
#include "MWCPlusLib.h"
#include "ds_width.h"

/* The catch record in a handler's frame, at a frame offset an action names.
 * func_02071ccc fills the first five words for a catch: the thrown object,
 * its type string and destructor, and the object the handler sees, which for
 * a '*' (pointer) type is mPointerCopy, a copy of the adjusted pointer.
 * func_02071f1c fills the first three and mStackTop for an exception
 * specification; func_02072168 (case 0xd) and func_02072014 read the first
 * three back. Names follow CodeWarrior's runtime record; mSublocation,
 * mPointerCopy and mStackTop are named from it, not from a reader in this
 * image (medium). */
typedef struct CatchInfo {
    int mLocation;    /* 0x00 -- ThrowContext.mLocation */
    int mTypeInfo;    /* 0x04 -- ThrowContext.mThrowType */
    int mDtor;        /* 0x08 -- ThrowContext.mDtor */
    int mSublocation; /* 0x0c */
    int mPointerCopy; /* 0x10 */
    void *mStackTop;  /* 0x14 */
} CatchInfo;

/* An exception-specification action as func_02071d3c decodes it (case 15):
 * the allowed-type count and list func_02071f88 checks, the unexpected
 * handler's offset from the function start, and the frame offset of its
 * CatchInfo. */
typedef struct ExSpecification {
    int mSpecCount;        /* 0x00 */
    int mPCOffset;         /* 0x04 */
    int mCatchInfoRef;     /* 0x08 */
    unsigned char *mSpecs; /* 0x0c */
} ExSpecification;

/* The frame the unwinder stands in: its registers and what func_02071864 read
 * from its frame header. func_02071d3c and func_02072014 copy a ThrowContext
 * member by member, and in the ROM this block is one 0x54-byte copy after
 * seven word copies, so it is a struct of its own. Offsets in brackets are
 * from the ThrowContext. */
typedef struct ThrowFrame {
    /* 0x00 [0x1c] -- r0..r15; func_02071910 restores the saved registers
     * here, mGPR[14] is the return address */
    int mGPR[16];
    /* 0x40 [0x5c] -- __rethrow stores the throwing caller's sp here, and
     * func_02071790 enters the landing pad with sp = mCallerSP - mFPInfo */
    char *mCallerSP;
    /* 0x44 [0x60] -- frame size, an unsigned LEB128 from the header; the
     * save area ends at mFP + mFrameSize */
    int mFrameSize;
    /* 0x48 [0x64] -- a second unsigned LEB128, present only when header
     * flag 0x40 is set (func_02071864) */
    int mFPInfo;
    /* 0x4c [0x68] -- the saved registers, bit n = rn: header byte 1 << 4
     * (r4..r11) plus lr (0x4000) */
    u16 mSavedRegMask;
    /* 0x4e [0x6a] -- header flag 0x20: the save area ends 0x10 bytes lower,
     * as when a prologue pushes r0..r3 above the saved registers (medium) */
    u8 mArgsPushed;
    /* 0x4f [0x6b] -- header flag 0x40: the frame keeps a frame pointer, so
     * func_02071864 takes mFP from r7 or r11 rather than mSP */
    u8 mHasFP;
    /* 0x50 [0x6c] -- header flag 0x80: that frame pointer is r7 (Thumb)
     * rather than r11 */
    u8 mFPIsR7;
} ThrowFrame;

/* CodeWarrior exception runtime: the register/frame state the unwinder walks
 * (func_02072168 interprets actions against it, func_02071910 unwinds one
 * frame, func_02071f1c hands the exception to a handler). 0x70 bytes, the
 * frame __rethrow builds on its stack. */
typedef struct ThrowContext {
    int mThrowType; /* 0x00 -- type string; func_02071ccc tests it for '*' */
    int mLocation;  /* 0x04 -- thrown object; matched against a CatchInfo */
    int mDtor;      /* 0x08 -- thrown object's destructor (case 0xd) */
    /* 0x0c -- a rethrow's catch record, which func_02072014 finds and copies
     * the type and object back out of; 0 for a fresh throw */
    struct CatchInfo *mCatchInfo;
    /* 0x10 -- the throwing call's return address (__rethrow stores lr);
     * func_02071be8 looks its action table up by it */
    int mReturnAddr;
    char *mSP; /* 0x14 -- func_02071910: mFP + frame size after unwind */
    /* 0x18 -- base for frame-relative operands; set from mSP, r7 or r11 by
     * func_02071864 */
    char *mFP;
    ThrowFrame mFrame; /* 0x1c */
} ThrowContext;

#ifndef SM64DS_PLATFORM_PC
typedef char ThrowContext_args_pushed_must_be_at_0x6a
    [DS_IADDR(&((ThrowContext *)0)->mFrame.mArgsPushed) == 0x6a ? 1 : -1];
typedef char ThrowContext_must_be_0x70_bytes[sizeof(ThrowContext) == 0x70 ? 1 : -1];
#endif

#endif
