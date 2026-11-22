#include "types.h"
#include "private/throw_context.h"

u8 *func_02071a50(u8 *src, int *dst);

/* C++ exception runtime: read the frame header at the action table's start.
 * Byte 0 holds the flags (0x20 arguments pushed, 0x40 frame pointer, 0x80
 * that pointer is r7). Byte 1 holds the saved r4..r11 mask; lr is always
 * saved. Then come the frame size and, with a frame pointer, a second
 * LEB128. mFP becomes the frame pointer's value, or mSP when there is none. */
void ThrowContext::func_02071864(ActionIterator *it) {
    u8 *data = it->mActionTable;
    unsigned int flags = data[0];
    int f40 = flags & 0x40;
    int f80;
    u8 *p;
    mFrame.mHasFP = (f40 != 0);
    mFrame.mArgsPushed = ((flags & 0x20) != 0);
    f80 = flags & 0x80;
    mFrame.mFPIsR7 = (f80 != 0);
    mFrame.mSavedRegMask = data[1] << 4;
    p = data + 2;
    mFrame.mSavedRegMask |= 0x4000;
    p = func_02071a50(p, &mFrame.mFrameSize);
    if (f40) {
        p = func_02071a50(p, &mFrame.mFPInfo);
    }
    if (f40) {
        if (f80) {
            mFP = (char *)mFrame.mGPR[7];
        } else {
            mFP = (char *)mFrame.mGPR[11];
        }
        return;
    }
    mFP = mSP;
}
