// Zero two u32 fields, set a u16 field to 0xf, then clear bit 0x1 in the u32 at
// self+0xb0. The u64-mask launder forces the 0xb0 base to materialize (add r1,self,#0xb0).
void func_ov002_020ae9f8(char *self)
{
    *(unsigned int *)(self + 0x98) = 0;
    *(unsigned int *)(self + 0xa8) = 0;
    *(unsigned short *)(self + 0x102) = 0xf;
    *(unsigned int *)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x1;
}
