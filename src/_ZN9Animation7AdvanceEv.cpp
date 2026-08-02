//cpp
// @symbol _ZN9Animation7AdvanceEv
#include "Animation.h"
extern "C" int __aeabi_idivmod(int n, int d);

void Animation::Advance()
{
    u32 f = numFramesAndFlags;
    u32 len = f & ~0xc0000000;
    if ((f & 0xc0000000) == 0) {
        currFrame = (currFrame + speed + (int)len) % (int)len;
    } else {
        /* the ROM materializes the field address for this
           read-modify-write, and reloads currFrame through the member
           afterwards; the launder keeps this store aliasing the member so
           the compiler re-reads it the way the ROM does */
        int *pFrame = (int *)((long long)((int)this + 8));
        *pFrame = *pFrame + speed;
        if (currFrame < 0) {
            currFrame = 0;
        } else if (currFrame >= (int)len) {
            currFrame = (int)len - 1;
        }
    }
}
