// Early-out if the byte flag at self+0x5553 is set; otherwise set self+0x51f2 to 0x1e
// and increment the byte at self+0x5553. The check/store share the self+0x5000 base
// (add r2); the laundered increment pool-loads the 0x5553 offset.
void func_ov006_020e81e0(char *self)
{
    if (*(unsigned char *)(self + 0x5553)) return;
    *(unsigned char *)(self + 0x51f2) = 0x1e;
    *(unsigned char *)(((long long)(int)(self + 0x5553)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}
