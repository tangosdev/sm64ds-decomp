extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void func_ov063_02119e38(void *c, int a, int b, int d);
extern void func_ov063_02119b1c(char *c);
#define M(p) (p)
void func_ov063_02119960(char *c)
{
    unsigned short *p = (unsigned short *)(int)M(c + 0x5d4);
    *p &= ~0x40;
    if (*(unsigned short *)(c + 0x100) == 0) {
        int r0 = RandomIntInternal(&data_0209e650);
        *(int *)(c + 0x588) = (((unsigned)r0 >> 16) & 0xfff) * 5;
        int ri = RandomIntInternal(&data_0209e650);
        int x = 0xfff & ((unsigned)ri >> 16);
        *(int *)(c + 0x58c) = (int)((((long long)x << 7) + 0x800) >> 12);
    }
    if (*(unsigned short *)(c + 0x5c0) == 0) {
        func_ov063_02119e38(c, -100, (short)(*(int *)(c + 0x58c) + 0x180), 0xfe0);
    }
    func_ov063_02119b1c(c);
}
