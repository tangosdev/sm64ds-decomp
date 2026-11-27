//cpp
#include "common.h"
#include "nitro/fx/fx.h"

extern "C" {
extern void func_ov007_020c368c(Vector3_16 *vectors, int count, int clamp);

/* Fixed-point rounding after the cross-product multiplies. */
static inline s16 Fx12Round(int x) {
    return (s16)((x + 0x800) >> 12);
}

/* Kept as an inline helper: spelling the same z differences as plain subtraction
 * is size-exact but moves 13 words. */
static inline int ZDiff(int a, int b) {
    return a - b;
}

/* Computes the two triangular face normals of each heightfield cell. The
 * strength-reduction and loop-invariant brackets are byte-load-bearing:
 * with the typed vectors above, the respective bodies are 0x2d8, 0x288 and
 * 0x2d0 against the cartridge's 0x2b8. */
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off

void func_ov007_020cb7c0(Vector3_16 *out, int w, int h, const VecFx32 *points, s16 cellWidth,
                         s16 cellHeight) {
    Vector3_16 *normal;
    int column;
    int row;
    int orientation;
    int pointStride;
    int rowStart;
    int negWidth;
    int negHeight;
    int negWidthHeight;
    int width;
    int height;
    int widthHeight;
    const VecFx32 *curRow;
    const VecFx32 *nextRow;
    const VecFx32 *currRight;
    const VecFx32 *nextRight;

    pointStride = w + 1;
    orientation = 1;
    normal = out;
    row = 0;
    if (h > 0) {
        width = cellWidth;
        height = cellHeight;
        rowStart = 0;
        negWidth = -width;
        negHeight = -height;
        negWidthHeight = negWidth * negHeight;
        int halfNegWidth = negWidth / 2;
        int halfWidth = width / 2;
        widthHeight = width * height;
        do {
            curRow = points + rowStart;
            nextRow = points + (rowStart + pointStride);
            column = 0;
            if (w > 0) {
                do {
                    currRight = curRow + 1;
                    nextRight = nextRow + 1;
                    if (orientation == 1) {
                        int zEast = currRight->z;
                        int t = negWidth * ZDiff(nextRow->z, zEast);
                        int d0 = ZDiff(curRow->z, zEast);
                        normal[0].x = Fx12Round(-d0 * negHeight);
                        normal[0].y = Fx12Round((d0 * halfNegWidth) - t);
                        normal[0].z = Fx12Round(negWidthHeight - halfNegWidth);
                        int zSouth = nextRow->z;
                        int t2 = width * ZDiff(currRight->z, zSouth);
                        int d1 = ZDiff(nextRight->z, zSouth);
                        normal[1].x = Fx12Round(-d1 * height);
                        normal[1].y = Fx12Round((d1 * halfWidth) - t2);
                        normal[1].z = Fx12Round(widthHeight - halfWidth);
                    } else {
                        int zSouth = nextRow->z;
                        int t = width * ZDiff(curRow->z, zSouth);
                        int d0 = ZDiff(nextRight->z, zSouth);
                        normal[0].x = Fx12Round(-d0 * height);
                        normal[0].y = Fx12Round((d0 * halfWidth) - t);
                        normal[0].z = Fx12Round(widthHeight - halfWidth);
                        int zEast = currRight->z;
                        int t2 = negWidth * ZDiff(nextRight->z, zEast);
                        int d1 = ZDiff(curRow->z, zEast);
                        normal[1].x = Fx12Round(-d1 * negHeight);
                        normal[1].y = Fx12Round((d1 * halfNegWidth) - t2);
                        normal[1].z = Fx12Round(negWidthHeight - halfNegWidth);
                    }
                    nextRow = nextRight;
                    curRow = currRight;
                    normal += 2;
                    column = column + 1;
                } while (column < w);
            }
            orientation = -orientation;
            rowStart = rowStart + pointStride;
            row = row + 1;
        } while (row < h);
    }
    func_ov007_020c368c(out, w * h * 2, 0);
}
}
