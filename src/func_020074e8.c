extern void _Z14ApproachLinearRiii(int *dest, int a, int b);

int func_020074e8(struct Obj *o) {
    _Z14ApproachLinearRiii((int *)((char *)o + 0xb8), -0x28000, 0x2000);
    *(int *)((char *)o + 0xb0) = -0x5000;
    return 1;
}
