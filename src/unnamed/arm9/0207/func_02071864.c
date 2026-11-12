#include "types.h"
#include "private/throw_context.h"

extern u8 *func_02071a50(u8 *src, int *dst);

/* C++ exception runtime: read the frame header at the action table's start.
 * Byte 0 holds the flags (0x20 arguments pushed, 0x40 frame pointer, 0x80
 * that pointer is r7). Byte 1 holds the saved r4..r11 mask; lr is always
 * saved. Then come the frame size and, with a frame pointer, a second
 * LEB128. mFP becomes the frame pointer's value, or mSP when there is none. */
void func_02071864(ThrowContext *t, ActionIterator *it) {
    u8 *data = it->mActionTable;
    unsigned int flags = data[0];
    int f40 = flags & 0x40;
    int f80;
    u8 *p;
    t->mFrame.mHasFP = (f40 != 0);
    t->mFrame.mArgsPushed = ((flags & 0x20) != 0);
    f80 = flags & 0x80;
    t->mFrame.mFPIsR7 = (f80 != 0);
    t->mFrame.mSavedRegMask = data[1] << 4;
    p = data + 2;
    t->mFrame.mSavedRegMask |= 0x4000;
    p = func_02071a50(p, &t->mFrame.mFrameSize);
    if (f40) {
        p = func_02071a50(p, &t->mFrame.mFPInfo);
    }
    if (f40) {
        if (f80) {
            t->mFP = (char *)t->mFrame.mGPR[7];
        } else {
            t->mFP = (char *)t->mFrame.mGPR[11];
        }
        return;
    }
    t->mFP = t->mSP;
}
