//cpp
#include "common.h"
#include "nitro/cp/sqrt.h"
#include "math/CalcLib.h"


/* recovered: shared common types */
int func_ov007_020c30ac(struct Vector3 *arr, int count, struct Vector3 *target, int arg4,
                        int *out) {
    struct Vector3 diff;
    long long d2;
    int i;
    int bestIdx;
    long long best = 0x7fffffffffffffffLL;
    for (i = 0; i < count; i++) {
        VEC_Subtract(arr, target, &diff);
        d2 = (long long)diff.x * diff.x + (long long)diff.y * diff.y + (long long)diff.z * diff.z;
        arr++;
        if (d2 < best) {
            bestIdx = i;
            best = d2;
        }
    }
    if (arg4 < 0 || (long long)arg4 * arg4 >= best) {
        CP_SetSqrt64((u64)best << 2);
        if (out != 0) {
            *out = ((s32)CP_GetSqrtResult32() + 1) >> 1;
        }
        return bestIdx;
    }
    return -1;
}

