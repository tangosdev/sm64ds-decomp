#include "types.h"
/* C++ exception runtime: unwind one frame. Reload every register the frame
 * header (read by func_02071864) says the frame saved, walking down from the
 * top of its save area, step mSP past the frame, and return the restored
 * return address (mGPR[14]) for func_02072dac to look up the caller's
 * action table. The view extends func_02072168.c's ThrowContext with the
 * frame-header fields func_02071864 fills.
 *
 * func_02071910 at 0x02071910, size 0x64
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef struct ThrowContext {
    char pad_00[0x14];
    char *mSP;    /* 0x14 */
    char *mFP;    /* 0x18 */
    int mGPR[16]; /* 0x1c -- r0..r15 */
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

#pragma opt_strength_reduction off
void* func_02071910(char* self)
{
    ThrowContext *c = (ThrowContext *)self;
    int off = (c->mArgsPushed != 0) ? 0x10 : 0;
    int frameSize = c->mFrameSize;
    char *save = (char *)((int)c->mFP + frameSize - off);
    int i;

    for (i = 15; i >= 0; i--) {
        u16 flags = c->mSavedRegMask;
        if (flags & (1 << i)) {
            save -= 4;
            c->mGPR[i] = *(int *)save;
        }
    }

    c->mSP = c->mFP + frameSize;
    return (void *)c->mGPR[14];
}
