#include "types.h"
extern unsigned char func_02071988(void);

int func_020719ec(int unused, unsigned char *buf, unsigned int *pcount)
{
    unsigned int i;
    unsigned int n = *pcount;
    i = 0;
    while (i < n) {
        unsigned char c;
        buf[i] = func_02071988();
        c = buf[i];
        if (c == 0xd || c == 0xa) {
            *pcount = i + 1;
            break;
        }
        i++;
    }
    return 0;
}
