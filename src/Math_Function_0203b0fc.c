#include "nitro/fx/fx.h"
void Math_Function_0203b0fc(int *p, int target, int scale, int max)
{
    int delta;
    int d;
    if (*p == target) return;
    d = target - *p;
    delta = FX_MUL(d, scale);
    if (delta > max) {
        delta = max;
    } else if (delta < -max) {
        delta = -max;
    }
    *p += delta;
}
