#include "types.h"
u8 *func_0206e2cc(u8 *p, int c, unsigned int n)
{
    u8 ch = (u8)c;
    if (n != 0) {
        do {
            if (*p++ == ch) return p - 1;
        } while (--n != 0);
    }
    return 0;
}
