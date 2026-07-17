typedef unsigned char u8;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#pragma opt_common_subs off

void func_ov006_0210446c(char *o, int a1, int a2, int a3)
{
    int i;
    char *q = o;
    for (i = 0; i < 0x40; i++, q += 0x18) {
        if (*(u8 *)(q + 0x46b8) == 0) {
            int off = i * 0x18;
            int *w;
            *(u8 *)(o + off + 0x46b8) = 1;
            *(u8 *)(o + off + 0x46bd) = 1;
            *(u8 *)(o + off + 0x46bc) = 0;
            *(u8 *)(o + off + 0x46ba) = 0;
            *(u8 *)(o + off + 0x46bb) = 0;
            *(u8 *)(o + off + 0x46b9) = a3;
            *(int *)((char *)(((int)o + 0x46b0) & 0xFFFFFFFFFFFFFFFF) + off) = 0;
            *(int *)(o + off + 0x46b4) = 0;
            *(int *)(o + off + 0x46a8) = a1;
            *(int *)(o + off + 0x46ac) = a2;
            w = (int *)((char *)(((int)o + 0x46b0) & 0xFFFFFFFFFFFFFFFF) + off);
            if (a3 != 0) {
                *(u8 *)(o + off + 0x46be) = 0;
            } else {
                int rnd = (int)((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
                rnd = (int)((((unsigned int)rnd) << 1) >> 15);
                *(u8 *)(o + off + 0x46be) = rnd;
            }
            if (a1 >= 0x80000)
                *w = -0x100;
            else
                *w = 0x100;
            return;
        }
    }
}
