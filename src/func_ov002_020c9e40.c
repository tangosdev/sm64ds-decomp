// Set byte flag at self+0x709, then set bit 0x4 in the u32 at self+0x2ec. The
// u64-mask launder forces the 0x2ec base to materialize (add r1,self,#0x2ec).
void func_ov002_020c9e40(char *self)
{
    *(unsigned char *)(self + 0x709) = 1;
    *(unsigned int *)(((long long)(int)(self + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4;
}
