#include "types.h"
extern int func_ov002_020f5a94(void);
extern void func_ov002_020f2958(void *c);

void func_ov002_020f5d34(u8 *self)
{
    int n = func_ov002_020f5a94();
    int i = 0;
    u8 *e;
    int v;
    if (n <= 0) goto done;
    e = self;
    v = 0;
    do {
        *(u8 *)(e + 0x44) = 1;
        *(u8 *)(e + 0x45) = 0;
        *(s16 *)(e + 0x2e) = 0;
        *(s16 *)(e + 0x38) = 0;
        *(int *)(e + 0) = 0x80000;
        *(int *)(e + 4) = 0x60000;
        *(s16 *)(e + 0x30) = v;
        *(s16 *)(e + 0x32) = 0;
        *(s16 *)(e + 0x34) = 0;
        *(s16 *)(e + 0x3e) = 0;
        *(int *)(e + 0x28) = 0x199;
        *(s16 *)(e + 0x2c) = 0x199;
        *(s16 *)(e + 0x38) = 0;
        *(u8 *)(e + 0x46) = 0;
        *(u8 *)(e + 0x47) = 0;
        *(u8 *)(e + 0x49) = 0;
        e += 0x4c;
        v += 0x40;
        i++;
    } while (i < n);
done:
    func_ov002_020f2958(self);
}
