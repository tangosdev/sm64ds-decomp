void func_0204dc84(char *self)
{
    unsigned char *flags6 = (unsigned char *)(((long long)(int)(self + 6)) & 0xFFFFFFFFFFFFFFFFLL);
    *flags6 &= ~3;
    *flags6 &= ~4;
    *(short *)(self + 4) = 0;
    *(unsigned char *)(self + 7) = 0;
    *(short *)(self + 2) = 0;
    *(short *)(self) = *(short *)(self + 2);
    *(int *)(self + 8) = 0;
}
