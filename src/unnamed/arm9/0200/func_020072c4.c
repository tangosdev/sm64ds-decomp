extern void _Z14ApproachLinearRiii(int *p, int value, int speed);

int func_020072c4(char *self)
{
    _Z14ApproachLinearRiii((int *)(self + 0x174), 0xf000, 0x199);
    _Z14ApproachLinearRiii((int *)(self + 0xb0), 0x7d0000, *(int *)(self + 0x174));
    _Z14ApproachLinearRiii((int *)(self + 0xb4), 0x4b0000, *(int *)(self + 0x174) / 10);
    _Z14ApproachLinearRiii((int *)(self + 0xb8), 0x3e8000, *(int *)(self + 0x174) / 10);
    _Z14ApproachLinearRiii((int *)(self + 0xbc), 0, *(int *)(self + 0x174));
    _Z14ApproachLinearRiii((int *)(self + 0xc0), 0x4b0000, *(int *)(self + 0x174) >> 1);
    _Z14ApproachLinearRiii((int *)(self + 0xc4), 0x3e8000, *(int *)(self + 0x174) * 10 / 15);
    return 1;
}
