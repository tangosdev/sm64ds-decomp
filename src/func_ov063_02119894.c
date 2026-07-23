typedef long long s64;
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void func_ov063_02119cc0(void *c, int a, int b, int d);
extern void func_ov063_02119b1c(char *c);

void func_ov063_02119894(char *c)
{
    *(unsigned short *)(((int)c + 0x5d4) & 0xFFFFFFFFFFFFFFFF) &= ~0x40;
    if (*(unsigned short *)(c + 0x100) == 0) {
        unsigned r0 = (unsigned)RandomIntInternal(&data_0209e650);
        *(int *)(c + 0x588) = (int)(((r0 >> 16) & 0xfff) * 5);
        r0 = (unsigned)RandomIntInternal(&data_0209e650);
        {
            int x = (int)((r0 >> 16) & 0xfff);
            *(int *)(c + 0x58c) = (int)(((s64)x << 7) + 0x800 >> 12);
        }
    }
    if (*(unsigned short *)(c + 0x5c0) == 0) {
        func_ov063_02119cc0(c, -100, (short)(*(int *)(c + 0x58c) + 0x180), 0xc00);
    }
    func_ov063_02119b1c(c);
}
