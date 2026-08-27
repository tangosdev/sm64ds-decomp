extern int _Z14ApproachLinearRiii(int *p, int a, int b);
extern int ApproachAngle(void *p, int a, int b, int c, int d);
extern int Math_Function_0203b14c(void *p, int a, int b, int c, int d);
extern int DecIfAbove0_Short(void *p);
extern int func_ov014_02111ebc(void *c, int i);

int func_ov014_0211150c(char *c) {
    _Z14ApproachLinearRiii((int*)(c + 0x80), 0x1000, 0x500);
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);
    ApproachAngle(c + 0x8c, -0x4000, 4, 0x1000, 0x400);
    if (Math_Function_0203b14c(c + 0x5f8, 0x64000, 0x800, 0x10000, 0x800) != 0) return;
    if (DecIfAbove0_Short(c + 0x5fc) != 0) return;
    func_ov014_02111ebc(c, 1);
}
