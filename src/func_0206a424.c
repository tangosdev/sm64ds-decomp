void func_0206a424(int *self)
{
    volatile unsigned short *reg = (volatile unsigned short *)(((long long)0x4000204) & 0xFFFFFFFFFFFFFFFFLL);
    *reg = (self[0] << 2) | (*reg & ~0xc);
    *reg = (self[1] << 4) | (*reg & ~0x10);
}
