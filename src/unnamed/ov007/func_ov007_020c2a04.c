#include "Ov007StrokePath.h"

extern void MultiCopy_Int(int *src, int *dst, int len);
extern Ov007StrokePath *func_ov007_020c2c68(int a, int b, int cc);
extern void func_ov007_020c27b4(Ov007StrokePath *path);
extern void func_ov007_020c26fc(Ov007StrokePath *path);

/* Copies points start .. end of src into a new path of the same
 * representation and flags (func_ov007_020c2c68), then recomputes its
 * segments and extrema. */
Ov007StrokePath *func_ov007_020c2a04(Ov007StrokePath *src, int start, int end) {
    Ov007StrokePath *d;
    int n;
    n = end - start + 1;
    d = func_ov007_020c2c68(src->representation, n, src->flags);
    d->pointCount = n;
    switch (src->representation) {
    case OV007_PATH_PLANAR:
        MultiCopy_Int(src->xCoords + start, d->xCoords, n << 2);
        MultiCopy_Int(src->yCoords + start, d->yCoords, n << 2);
        break;
    case 1:
        MultiCopy_Int((int *)((char *)src->pointStorage + start * 0xc), (int *)d->pointStorage,
                      n * 0xc);
        break;
    }
    if (src->flags & OV007_PATH_HAS_PROGRESS) {
        MultiCopy_Int(src->progressThresholds + start, d->progressThresholds, n << 2);
    }
    func_ov007_020c27b4(d);
    func_ov007_020c26fc(d);
    return d;
}
