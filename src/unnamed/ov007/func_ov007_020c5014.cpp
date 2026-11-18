//cpp
#include "Ov007StrokePath.h"
#include "Ov007Picker.h"
#include "nitro/fx/fx_cp.h"

/* Advances the draw limit along the committed strokes' progress
 * thresholds to progress x: 0 inside a stroke, 1 when it steps on to the
 * next stroke, 2 once it passes the last stroke. */
/* The stroke's progress threshold at point k. Read through this accessor,
   the advance loop re-reads the threshold on each pass as the cartridge
   does; that took the place of opt_common_subs off (#821 r79). */
static inline int GetProgressThreshold(const Ov007StrokePath *path, int k) {
    return path->progressThresholds[k];
}

int Ov007Picker::func_ov007_020c5014(int x) {
    Ov007StrokePath *obj;
    int i;
    int j;
    int *arr;
    int d, den;
    limitFraction = 0x1000;
    i = limitGroup;
    if (i < groupCount) {
        obj = groups[i];
        j = limitIndex;
        if (x >= GetProgressThreshold(obj, j)) {
            for (;;) {
                limitIndex += 1;
                j = limitIndex;
                if (j >= obj->pointCount) {
                    limitGroup += 1;
                    {
                        int cnt = groupCount;
                        if (limitGroup >= cnt) {
                            limitGroup = cnt;
                            limitIndex = obj->pointCount;
                            return 2;
                        }
                    }
                    limitIndex = 0;
                    return 1;
                }
                if (x < GetProgressThreshold(obj, j))
                    break;
            }
        }
        if (j > 0) {
            arr = obj->progressThresholds;
            d = arr[j] - x;
            den = arr[j] - arr[j - 1];
            if (den == 0)
                den = 1;
            limitFraction = 0x1000 - FX_DivS32(d << 12, den);
        }
        return 0;
    }
    return 2;
}
