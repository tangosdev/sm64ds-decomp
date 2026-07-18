typedef unsigned char u8;

extern void SetBg2Offset(int a, int b);
extern void SetBg3Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern void SetSubBg3Offset(int a, int b);
extern int func_ov004_020ae140(char *self);

#define V (*(int *)(c + 0xab6c) >> 12)

int func_ov006_02128fb8(char *c)
{
    if (*(int *)(c + 0x4628) == 0) {
        if (*(u8 *)(c + 0xb9f8) == 0) {
            *(volatile unsigned short *)0x4000304 |= 0x8000;
            SetBg2Offset(0, V);
            SetBg3Offset(0, V);
            SetSubBg2Offset(0x100, V + 0x110);
            SetSubBg3Offset(0, V + 0x110);
            *(volatile int *)0x4000018 = (V << 16) & 0x1ff0000;
            *(volatile int *)0x400001c = (V << 16) & 0x1ff0000;
            *(volatile int *)0x4001018 = (((V + 0x110) << 16) & 0x1ff0000) | 0x100;
            *(volatile int *)0x400101c = ((V + 0x110) << 16) & 0x1ff0000;
        } else {
            *(volatile unsigned short *)0x4000304 &= ~0x8000;
            SetBg2Offset(0x100, V + 0x110);
            SetBg3Offset(0, V + 0x110);
            SetSubBg2Offset(0, V);
            SetSubBg3Offset(0, V);
            *(volatile int *)0x4000018 = (((V + 0x110) << 16) & 0x1ff0000) | 0x100;
            *(volatile int *)0x400001c = ((V + 0x110) << 16) & 0x1ff0000;
            *(volatile int *)0x4001018 = (V << 16) & 0x1ff0000;
            *(volatile int *)0x400101c = (V << 16) & 0x1ff0000;
        }
    }
    return func_ov004_020ae140(c) != 0;
}
