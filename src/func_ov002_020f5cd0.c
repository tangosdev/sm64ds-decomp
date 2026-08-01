#include "types.h"
struct E {
    s32 f0;       /* 0x00 */
    s32 f4;       /* 0x04 */
    char p8[0x28-8];
    s32 f28;      /* 0x28 */
    u16 f2c;      /* 0x2c */
    char p2e[0x30-0x2e];
    u16 f30;      /* 0x30 */
    u16 f32;      /* 0x32 */
    char p34[0x3a-0x34];
    u16 f3a;      /* 0x3a */
    char p3c[0x4c-0x3c];
};
void func_ov002_020f5cd0(struct E *c)
{
    int i;
    if (*(u8*)((char*)c + 0x44) == 0)
        return;
    c->f0 = 0x80000;
    c->f4 = 0x60000;
    c->f30 = 0;
    *(u8*)((char*)c + 0x46) = 1;
    c->f28 = 0xccc;
    c->f2c = 0x155;
    for (i = 0; i < 4; i++) {
        c[i].f3a = 0;
        c[i].f32 = 0;
    }
}
