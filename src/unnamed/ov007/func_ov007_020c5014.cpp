//cpp
#include "Ov007StrokePath.h"
#include "cstd.h"

extern "C" {

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file and func_ov007_020c684c, only the slots this function touches. */
typedef struct Ov007Picker {
    char pad_00[8];
    int groupCount; /* 0x08 */
    char pad_0c[0x2c];
    Ov007StrokePath **groups; /* 0x38: committed strokes */
    char pad_3c[0x70];
    /* 0xac/0xb0: the draw limit; func_ov007_020c5a6c's flagged picks stop
     * before it, func_ov007_020c4dfc draws whole groups up to limitGroup and
     * that group's first limitIndex points */
    int limitGroup;
    int limitIndex;
    /* 0xb4: Q12 progress past point limitIndex - 1 toward limitIndex, which
     * func_ov007_020c4dfc hands to func_ov007_020c19cc as its `t` */
    s16 limitFraction;
} Ov007Picker;

typedef char
    Ov007Picker_limitFraction_must_be_at_0xb4[(int)&((Ov007Picker *)0)->limitFraction == 0xb4 ? 1
                                                                                              : -1];

/* Advances the draw limit along the committed strokes' progress
 * thresholds to progress x: 0 inside a stroke, 1 when it steps on to the
 * next stroke, 2 once it passes the last stroke. */
#pragma opt_common_subs off
int func_ov007_020c5014(char *c, int x) {
    Ov007Picker *self = (Ov007Picker *)c;
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
            self->limitFraction = 0x1000 - cstd::div(d << 12, den);
        }
        return 0;
    }
    return 2;
}
}
