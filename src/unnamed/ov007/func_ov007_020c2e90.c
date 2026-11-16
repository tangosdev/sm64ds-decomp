#include "Ov007StrokePath.h"
#include "nitro/fx/fx_vec.h"

/* Whether a point at a may follow the last point, at b: always while the path
 * is empty or either bound is negative; otherwise only with room left and
 * the distance within minLen .. maxLen (whole units, compared in Q12). */
int func_ov007_020c2e90(Ov007StrokePath *path, const VecFx32 *a, const VecFx32 *b, int minLen,
                        int maxLen) {
    fx32 dist;
    if (path->pointCount != 0 && minLen >= 0 && maxLen >= 0) {
        if (path->pointCount < path->capacity) {
            dist = VEC_Distance(a, b);
            return (minLen << 12) <= dist && dist <= (maxLen << 12);
        }
        return 0;
    }
    return 1;
}
