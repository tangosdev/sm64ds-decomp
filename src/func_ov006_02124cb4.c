#pragma opt_strength_reduction off

typedef unsigned char u8;

extern int data_ov004_020bfa18;

extern void func_ov004_020ad79c(int a, int b);

void func_ov006_02124cb4(char *o)
{
    int i;
    *(u8 *)(((int)o + 0x51ce) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (*(u8 *)(o + 0x51ce) < 4)
        return;
    *(u8 *)(o + 0x51ce) = 0;
    for (i = 0; i < 2; i++)
        *(u8 *)(((int)o + i + 0x51cc) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (*(u8 *)(o + *(int *)(o + 0x51c4) + 0x51cc) < 4)
        return;
    *(int *)(o + 0x51bc) = 0x1e;
    *(int *)(((int)o + 0x51b8) & 0xFFFFFFFFFFFFFFFF) += 1;
    o[0xc3] = 0;
    {
        u8 x = *(u8 *)(o + *(int *)(o + 0x51c0) + 0x51ca);
        u8 y = *(u8 *)(o + *(int *)(o + 0x51c4) + 0x51ca);
        int c = *(int *)(o + 0xa8);
        int d = *(int *)(o + 0xb4);
        if (x > y) {
            c += data_ov004_020bfa18 * 2;
            d = d + 1;
        } else {
            d = d - 1;
        }
        func_ov004_020ad79c(c, d);
    }
}
