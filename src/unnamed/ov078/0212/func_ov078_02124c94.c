extern int func_ov078_02125c24(void* c, int a);
extern void func_0200fa8c(int *t, int a);
int func_ov078_02124c94(char *p) {
    *(int*)(p+0x9c) = -0x2000;
    *(int*)(p+0xa8) = 0x1e000;
    *(int*)(p+0x98) = 0x14000;
    *(char*)(p+0x499) = 0;
    *(short*)(p+0x100) = 5;
    *(int*)(p+0xb0) = 0x10000000;
    func_ov078_02125c24(p, 0xfa0000);
    func_0200fa8c((int*)p, 1);
    return 1;
}
