extern void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3)
{
    func_ov004_020b1de8(a1, a2, a3, -1);
    int v = *(int*)(a0+0xac);
    if (v < 0x64) {
        func_ov004_020b2444(a1+0x12, a2, v, a3, -1, 2, 0);
    } else {
        func_ov004_020b1ea4(a1+0xc, a2, v, a3, -1, 2, 0x32);
    }
}
