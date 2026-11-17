//cpp
#include "Ov007StrokePath.h"
#include "Ov007Picker.h"
#include "nitro/fx/fx_cp.h"

/* Advances the draw limit along the committed strokes' progress
 * thresholds to progress x: 0 inside a stroke, 1 when it steps on to the
 * next stroke, 2 once it passes the last stroke. */
/* opt_common_subs stays off: the cartridge re-reads the stroke's threshold on
   each pass of the advance loop, and CSE hoists it (0xf0 against 0xf8).
   Re-measured (#821 r17): `++`, `= limitIndex + 1` and `j = ++limitIndex`
   for the step, and the loop as a while on the threshold test, are all
   0xf0 (0xf4 with j assigned in the condition); the progressThresholds
   pointer stays cached in a register across the loop. */
#pragma opt_common_subs off
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
        if (x >= obj->progressThresholds[j]) {
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
                if (x < obj->progressThresholds[j])
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
