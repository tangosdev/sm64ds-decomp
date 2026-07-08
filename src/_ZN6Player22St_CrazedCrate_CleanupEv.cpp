//cpp
// Player::St_CrazedCrate_Cleanup - clear bit 0x20 in the u32 at this+0x2ec, zero the
// byte at this+0x6e1, return 1. The u64-mask launder forces the 0x2ec base to
// materialize (add r3,this,#0x2ec) instead of folding the offset.
extern "C" {
int _ZN6Player22St_CrazedCrate_CleanupEv(char *c)
{
    *(unsigned int *)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20;
    *(unsigned char *)(c + 0x6e1) = 0;
    return 1;
}
}
