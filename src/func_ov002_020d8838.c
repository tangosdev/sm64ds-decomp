// Set bit 0x20 in the halfword flag at self+0x6ce and return. The u64-mask launder
// forces the base+offset to materialize into a register (pool-loaded 0x6ce + add) so
// the ldrh/strh reuse it, matching the ROM instead of folding the address.
void func_ov002_020d8838(char *self)
{
    *(unsigned short *)(self + 0x6ce) |= 0x20;
}
