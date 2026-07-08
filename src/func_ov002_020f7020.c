// Clear flag bit 0x2 in the u32 at self+0xb0, return 1. u64-mask launder forces the
// base to materialize (add r2,self,#0xb0) instead of folding the offset.
int func_ov002_020f7020(char *self)
{
    *(unsigned int *)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x2;
    return 1;
}
