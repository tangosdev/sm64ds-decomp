#include "types.h"
#include "private/throw_context.h"
/* C++ exception runtime: unwind one frame. Reload every register the frame
 * header says the frame saved, walking down from the top of its save area,
 * step mSP past the frame, and return the restored return address (mGPR[14]). */
void *ThrowContext::func_02071910(void *) {
    int off = (mFrame.mArgsPushed != 0) ? 0x10 : 0;
    int frameSize = mFrame.mFrameSize;
    int *save = (int *)((int)mFP + frameSize - off);
    int i;

    for (i = 15; i >= 0; i--) {
        u16 flags = mFrame.mSavedRegMask;
        if (flags & (1 << i)) {
            save -= 1;
            mFrame.mGPR[i] = *save;
        }
    }

    mSP = mFP + frameSize;
    return (void *)mFrame.mGPR[14];
}
