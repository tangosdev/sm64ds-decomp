#include "types.h"
#include "private/throw_context.h"
/* C++ exception runtime: unwind one frame. Reload every register the frame
 * header (read by func_02071864) says the frame saved, walking down from the
 * top of its save area, step mSP past the frame, and return the restored
 * return address (mGPR[14]) for func_02072dac to look up the caller's
 * action table. The view extends func_02072168.cpp's ThrowContext with the
 * frame-header fields func_02071864 fills.
 *
 * func_02071910 at 0x02071910, size 0x64
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
void *func_02071910(ThrowContext *c, void *) {
    int off = (c->mFrame.mArgsPushed != 0) ? 0x10 : 0;
    int frameSize = c->mFrame.mFrameSize;
    int *save = (int *)((int)c->mFP + frameSize - off);
    int i;

    for (i = 15; i >= 0; i--) {
        u16 flags = c->mFrame.mSavedRegMask;
        if (flags & (1 << i)) {
            save -= 1;
            c->mFrame.mGPR[i] = *save;
        }
    }

    c->mSP = c->mFP + frameSize;
    return (void *)c->mFrame.mGPR[14];
}
