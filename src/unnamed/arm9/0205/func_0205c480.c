#include "types.h"
int func_0205c480(unsigned char *a, unsigned char *b, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        unsigned int ca = a[i] - 0x41;
        unsigned int cb = b[i] - 0x41;
        if (ca <= 0x19) ca += 0x20;
        if (cb <= 0x19) cb += 0x20;
        if (ca != cb) return ca - cb;
    }
    return 0;
}
