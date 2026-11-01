//cpp
#include "CP15.h"

extern "C" {

typedef short s16;

/* The capture-effect state that func_02049ba8 receives as its callback
   argument (data_020a4c64, set by func_02049cd8/func_02049c48). This effect
   keeps the last two L-R differences of the previous buffer in history. */
struct Ctx {
    int kind;       /* 0x00 -- 0 none, 1 this effect, 2 func_020497a0 */
    void *fn;       /* 0x04 */
    int arg;        /* 0x08 */
    int history[2]; /* 0x0c */
};

/* Capture effect 1, run on each captured stereo buffer (len bytes per channel,
   16-bit samples). Each sample gets the L-R difference from two samples
   earlier added to left and subtracted from right, saturated to s16, which
   widens the stereo image. The first two samples take their differences from
   ctx->history, and the buffer's last two differences are saved there for the
   next call. Both buffers are then flushed from the data cache, since the
   sound hardware reads them. */


void func_020499bc(s16 *left, s16 *right, unsigned int len, Ctx *ctx) {
    s16 carry[2];
    int n = len >> 1;
    int i;

    for (i = 0; i < 2; i++) {
        int idx = i + n - 2;
        int d = left[idx] - right[idx];
        if (d < -0x8000)
            d = -0x8000;
        else if (d >= 0x8000)
            d = 0x7fff;
        carry[i] = d;
    }

    for (i = n - 1; i >= 2; i--) {
        int d = left[i - 2] - right[i - 2];
        if (d >= 0) {
            int hi = d + left[i];
            left[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            {
                int lo = right[i] - d;
                right[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            }
        } else {
            int lo = d + left[i];
            left[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            {
                int hi = right[i] - d;
                right[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            }
        }
    }

    for (i = 1; i >= 0; i--) {
        int v = left[i] + ctx->history[i];
        if (v < -0x8000)
            v = -0x8000;
        else if (v >= 0x8000)
            v = 0x7fff;
        left[i] = v;
        {
            int w = right[i] - ctx->history[i];
            if (w < -0x8000)
                w = -0x8000;
            else if (w >= 0x8000)
                w = 0x7fff;
            right[i] = w;
        }
    }

    for (i = 0; i < 2; i++) {
        ctx->history[i] = carry[i];
    }

    CP15::FlushAndInvalidateDataCache((unsigned int)left, len);
    CP15::FlushAndInvalidateDataCache((unsigned int)right, len);
}
}
