typedef int s32;

void func_ov002_020bbc78(char *self)
{
    *(s32 *)(self + 0x98) = 0x50000;
    *(s32 *)(self + 0xa8) = 0xa000;
    *(s32 *)(self + 0x5a0) = *(s32 *)(self + 0x59c);
    *(s32 *)(self + 0x59c) = 0;
    {
        s32 *p = (s32 *)(((int)self + 0x338));
        *p |= 0x2000;
        *p &= ~0x4000000;
    }
}
