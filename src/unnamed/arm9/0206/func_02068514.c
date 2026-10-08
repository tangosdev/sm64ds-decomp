#include "types.h"
int func_02068514(u16 *p)
{
    u16 v = *p++;
    int count = 0;
    while (v != 0) {
        v = *p++;
        count++;
    }
    return count;
}
