#include "types.h"
int func_0206e28c(const u8* a, const u8* b, u32 n) {
    while (n != 0) {
        u8 ca = *a++;
        u8 cb = *b++;
        if (ca != cb) {
            if (a[-1] < b[-1]) return -1;
            return 1;
        }
        n--;
    }
    return 0;
}
