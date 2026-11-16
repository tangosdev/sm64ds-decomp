//cpp
#include "nitro/fx/fx_cp.h"

int func_ov007_020c3598(int val, int n, char *buf, int base) {
    int res = 0;
    int fnd = 0;
    int r7 = 1;
    int i;
    for (i = 0; i < (n - 1); i++) {
        r7 = r7 * base;
    }

    for (i = 0; i < n; i++) {
        int pos = (n - 1) - i;
        buf[pos] = FX_DivS32(val, r7);
        val -= r7 * buf[pos];
        r7 = FX_DivS32(r7, base);
        if (fnd == 0) {
            if (buf[pos] != 0) {
                fnd = 1;
                res = pos + 1;
            } else if (pos > 0) {
                buf[pos] = -1;
            }
        }
    }

    return res;
}
