void func_ov063_02119ab0(char *self)
{
    if ((unsigned int)(*(unsigned short *)(self + 0x5d4) << 30) >> 31 == 0)
        return;

    *(int *)(self + 0x504) = *(int *)(self + 0x5c);
    *(int *)(self + 0x508) = *(int *)(self + 0x60);
    *(int *)(self + 0x50c) = *(int *)(self + 0x64);

    if (*(unsigned short *)(self + 0x4a0) == 0xd4) {
        *(int *)(self + 0x508) += 0x3c000;
        return;
    }
    *(int *)(self + 0x508) += 0xa000;
}
