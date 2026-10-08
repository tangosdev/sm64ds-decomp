#include "common.h"

extern void func_ov007_020c368c(s16 *p, int n, int k);

static inline s16 f12(int x)
{
    return (s16)((x + 0x800) >> 12);
}

static inline int subz(int a, int b)
{
    return a - b;
}

#pragma opt_strength_reduction off
#pragma opt_loop_invariants off

void func_ov007_020cb7c0(s16 *out, int w, int h, int *pts, s16 a5, s16 a6)
{
    s16 *dst;
    int i;
    int row;
    int flip;
    int w1;
    int rowIndex;
    int ndx;
    int ndy;
    int ndxndy;
    int dx;
    int dy;
    int dxdy;
    int z0;
    int *curr;
    int *nxt;
    int *new_var;
    int *nxt1;

    w1 = w + 1;
    flip = 1;
    dst = out;
    row = 0;
    if (h > 0) {
        dx = a5;
        dy = a6;
        rowIndex = row;
        ndx = -dx;
        ndy = -dy;
        ndxndy = ndx * ndy;
        int halfNdx = ndx / 2;
        int halfDx = dx / 2;
        dxdy = dx * dy;
        z0 = row;
        do {
            curr = (int *)((char *)pts + rowIndex * 12);
            nxt = (int *)((char *)pts + (rowIndex + w1) * 12);
            i = z0;
            if (w > 0) {
                do {
                    int *curr1 = new_var = curr + 3;
                    nxt1 = nxt + 3;
                    if (flip == 1) {
                        int ze = curr1[2];
                        int t = ndx * subz(nxt[2], ze);
                        int d0 = subz(curr[2], ze);
                        dst[0] = f12(-d0 * ndy);
                        dst[1] = f12((d0 * halfNdx) - t);
                        dst[2] = f12(ndxndy - halfNdx);
                        int zs = nxt[2];
                        int t2 = dx * subz(curr1[2], zs);
                        int d1 = subz(nxt1[2], zs);
                        dst[3] = f12(-d1 * dy);
                        dst[4] = f12((d1 * halfDx) - t2);
                        dst[5] = f12(dxdy - halfDx);
                    } else {
                        int zs = nxt[2];
                        int t = dx * subz(curr[2], zs);
                        int d0 = subz(nxt1[2], zs);
                        dst[0] = f12(-d0 * dy);
                        dst[1] = f12((d0 * halfDx) - t);
                        dst[2] = f12(dxdy - halfDx);
                        int ze = curr1[2];
                        int t2 = ndx * subz(nxt1[2], ze);
                        int d1 = subz(curr[2], ze);
                        dst[3] = f12(-d1 * ndy);
                        dst[4] = f12((d1 * halfNdx) - t2);
                        dst[5] = f12(ndxndy - halfNdx);
                    }
                    nxt = nxt1;
                    curr = curr1;
                    dst = dst + 6;
                    i = i + 1;
                } while (i < w);
            }
            flip = -flip;
            rowIndex = rowIndex + w1;
            row = row + 1;
        } while (row < h);
    }
    func_ov007_020c368c(out, w * h * 2, 0);
}
