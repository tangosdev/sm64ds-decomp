extern void _Z14ApproachLinearRsss(short *dst, short target, short step);
extern void func_ov062_0211c658(void *c, void *p);
extern int data_ov062_0211e17c;
int func_ov062_0211b800(char *c) {
    *(int*)(c + 0xa8) = 0x14000;
    _Z14ApproachLinearRsss((short*)(c + 0x92), 0, 0x300);
    _Z14ApproachLinearRsss((short*)(c + 0x96), 0, 0x300);
    if (*(int*)(c + 0x60) >= *(int*)(c + 0x434) - 0x2000) {
        *(int*)(c + 0xa4) = 0;
        *(int*)(c + 0xa8) = 0;
        *(int*)(c + 0xac) = 0;
        *(int*)(c + 0x60) = *(int*)(c + 0x434);
        *(int*)(c + 0x390) = 0x1000;
        func_ov062_0211c658(c, &data_ov062_0211e17c);
    }
    return 1;
}
