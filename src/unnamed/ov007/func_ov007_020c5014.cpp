//cpp
#include "Ov007StrokePath.h"
#include "Ov007Picker.h"
#include "nitro/fx/fx_cp.h"

/* Advances the draw limit along the committed strokes' progress
 * thresholds to progress x: 0 inside a stroke, 1 when it steps on to the
 * next stroke, 2 once it passes the last stroke. */
/* opt_common_subs stays off: the cartridge re-reads the stroke's threshold on
   each pass of the advance loop, and CSE hoists it (0xf0 against 0xf8). */
#pragma opt_common_subs off
int Ov007Picker::func_ov007_020c5014(int x) {
    Ov007Picker *self = this;
    Ov007StrokePath *obj;
    int i;
    int j;
    int *arr;
    int d, den;
    self->limitFraction = 0x1000;
    i = self->limitGroup;
    if (i < self->groupCount) {
        obj = self->groups[i];
        j = self->limitIndex;
        if (x >= obj->progressThresholds[j]) {
            for (;;) {
                self->limitIndex += 1;
                j = self->limitIndex;
                if (j >= obj->pointCount) {
                    self->limitGroup += 1;
                    {
                        int cnt = self->groupCount;
                        if (self->limitGroup >= cnt) {
                            self->limitGroup = cnt;
                            self->limitIndex = obj->pointCount;
                            return 2;
                        }
                    }
                    self->limitIndex = 0;
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
            self->limitFraction = 0x1000 - FX_DivS32(d << 12, den);
        }
        return 0;
    }
    return 2;
}
