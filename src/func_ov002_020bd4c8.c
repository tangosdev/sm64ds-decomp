// Set bits 0x24000000 in the u32 flag word at self+0xb0 and return 1. The u64-mask
// launder forces the base+offset to materialize into a register (add r2,self,#0xb0)
// so the load/store reuse it, matching the ROM instead of folding the offset.
int func_ov002_020bd4c8(char *self)
{
    *(unsigned int *)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x24000000;
    return 1;
}
