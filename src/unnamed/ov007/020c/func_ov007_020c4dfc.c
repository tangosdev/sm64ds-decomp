extern void func_ov007_020c4fcc(int r0, int r1);
extern void func_ov007_020c19cc(int a, int b, int c, int d);
void func_ov007_020c4dfc(char *c, int r1, int r2, int r3)
{
    func_ov007_020c4fcc(r2, r3);
    int i = 0;
    while (i < *(volatile int*)(c + 0xac)) {
        int *a38 = *(int**)(c + 0x38);
        int *a28 = *(int**)(c + 0x28);
        func_ov007_020c19cc(*(unsigned short*)(a38[i] + 8), a28[i], r1, 0x1000);
        i++;
    }
    {
        int m = *(volatile int*)(c + 0xac);
        if (m < *(int*)(c + 8)) {
            int *a28 = *(int**)(c + 0x28);
            func_ov007_020c19cc(*(int*)(c + 0xb0), a28[m], r1, *(short*)(c + 0xb4));
        }
    }
}
