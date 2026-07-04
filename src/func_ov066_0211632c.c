extern int data_ov066_0211ad18[];

void func_ov066_0211632c(char *self)
{
    int *p338 = (int *)(((long long)(int)(self + 0x338)) & 0xFFFFFFFFFFFFFFFFLL);
    *p338 &= ~2;
    *(int *)(self + 0x324) = 0x9c000;
    *(int *)(self + 0x328) = 0x164000;
    if (*(int *)(self + 0x49c) == 2)
        data_ov066_0211ad18[0] = 0x55000;
    else
        data_ov066_0211ad18[0] = -0x55000;
    data_ov066_0211ad18[1] = -0xc0000;
    data_ov066_0211ad18[2] = 0x80000;
}
