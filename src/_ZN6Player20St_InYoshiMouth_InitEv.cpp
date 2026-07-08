//cpp
// Player::St_InYoshiMouth_Init - call a helper, clear a run of byte flags, set bit 0x2
// in the u32 at this+0x2ec (laundered base), clear the byte at this+0x713, return 1.
extern "C" {
extern int func_ov002_020d62f8_cb(void *, int);
int _ZN6Player20St_InYoshiMouth_InitEv(char *c)
{
    func_ov002_020d62f8_cb(c, 0);
    *(unsigned char *)(c + 0x6e5) = 0;
    *(unsigned char *)(c + 0x6e6) = 0;
    *(unsigned int *)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x2;
    *(unsigned char *)(c + 0x713) = 0;
    return 1;
}
}
