#include "types.h"
struct S {
    int x00;    /* 0x00 */
    int x04;    /* 0x04 */
    int x08;    /* 0x08 */
    int x0c;    /* 0x0c */
    int x10;    /* 0x10 */
    int x14;    /* 0x14 */
    int x18;    /* 0x18 */
    int x1c;    /* 0x1c */
    int x20;    /* 0x20 */
    int x24;    /* 0x24 */
    int x28;    /* 0x28 */
    int x2c;    /* 0x2c */
};

void func_0204c0a8(int r0, int r1, struct S* r2) {
    r2->x00 = r1;
    r2->x0c = 0;
    r2->x18 = r0;
    r2->x24 = 0;
    r2->x04 = 0;
    r2->x10 = 0x1000;
    r2->x1c = 0;
    r2->x28 = 0;
    r2->x08 = -r0;
    r2->x14 = 0;
    r2->x20 = r1;
    r2->x2c = 0;
}
