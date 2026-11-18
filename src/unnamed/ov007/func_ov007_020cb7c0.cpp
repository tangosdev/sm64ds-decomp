//cpp
#include "common.h"

extern "C" {
extern void func_ov007_020c368c(Vector3_16 *vectors, int count, int clamp);

static inline s16 f12(int x) {
    return (s16)((x + 0x800) >> 12);
}

static inline int subz(int a, int b) {
    return a - b;
}

/* Kept: indexed-for rewrites over int* or Vector3* points, with or without
 * these, are 0x278..0x2ac against the cartridge's 0x2b8. Re-measured
 * (#821 r17): dropping both is 0x2d4, only strength reduction 0x2dc, only
 * loop invariants 0x288. With loop invariants still off, rowIndex taken as
 * `row * w1` each pass is 0x2b8 but 28 words off (the row base gets its
 * own stack slot, shifting six spills by 4), and the rows indexed as
 * `pts + rowIndex * 3` are 0x2e0. */
#pragma opt_strength_reduction off
/* Kept (#821 r79): with only loop invariants dropped, an inline RowPoints(pts, index) for both
   row pointers is 0x288 and nxt = curr + w1 * 3 is 0x294, against 0x2b8. */
#pragma opt_loop_invariants off

void func_ov007_020cb7c0(Vector3_16 *out, int w, int h, int *pts, s16 a5, s16 a6) {
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
    int *curr;
    int *nxt;
    int *curr1;
    int *nxt1;

    w1 = w + 1;
    flip = 1;
    dst = (s16 *)out;
    row = 0;
    if (h > 0) {
        dx = a5;
        dy = a6;
        rowIndex = 0;
        ndx = -dx;
        ndy = -dy;
        ndxndy = ndx * ndy;
        int halfNdx = ndx / 2;
        int halfDx = dx / 2;
        dxdy = dx * dy;
        do {
            curr = (int *)((char *)pts + rowIndex * 0xc);
            nxt = (int *)((char *)pts + (rowIndex + w1) * 0xc);
            i = 0;
            if (w > 0) {
                do {
                    curr1 = curr + 3;
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
}
