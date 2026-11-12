/* func_02071f88 at 0x02071f88
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
#include "private/throw_context.h"

extern int __throw_catch_compare(const unsigned char *throwtype, const unsigned char *catchtype,
                                 int *offset_result);

int func_02071f88(void *a, ExSpecification *spec) {
    unsigned char *p;
    unsigned int i;
    unsigned int buf[2];

    p = spec->mSpecs;
    for (i = 0; i < spec->mSpecCount; i++) {
        unsigned int val = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
        buf[0] = val;
        if (__throw_catch_compare((unsigned char *)a, (unsigned char *)val, (int *)&buf[1]))
            return 1;
        p += 4;
    }
    return 0;
}
