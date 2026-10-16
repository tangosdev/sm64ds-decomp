#include "types.h"

/* The field used here, named as in include/dCamera_c.h. */
struct dCamera_c {
    char pad_000[0xb0];
    Vector3 savedLookAt; /* 0x0b0 */
};

extern void _Z14ApproachLinearRiii(int *dest, int a, int b);

int func_020074e8(struct dCamera_c *o) {
    _Z14ApproachLinearRiii(&o->savedLookAt.z, -0x28000, 0x2000);
    o->savedLookAt.x = -0x5000;
    return 1;
}
