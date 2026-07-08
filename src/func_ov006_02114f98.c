// Increment the u32 counter at self+0x5994. The u64-mask launder forces the base to
// materialize via a pool-loaded constant + add (ldr r1,[pc]; add r1,self,r1), matching
// the ROM instead of splitting the large offset.
void func_ov006_02114f98(char *self)
{
    *(unsigned int *)(((long long)(int)(self + 0x5994)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}
