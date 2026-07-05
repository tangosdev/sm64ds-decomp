int _ZN6Player17St_Squish_CleanupEv(char *self)
{
    int val = 0x1000;
    if (*(unsigned char *)(self + 0x703) != 0)
        val = 0x3000;

    *(int *)(self + 0x80) = val;
    *(int *)(self + 0x84) = val;
    *(int *)(self + 0x88) = val;

    *(int *)(((long long)(int)(self + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~4;
    return 1;
}
