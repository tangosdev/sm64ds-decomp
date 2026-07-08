//cpp
// Player::St_Thrown_Cleanup - clear flag bit 0x2000 in the u32 at this+0x2ec, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0x2ec) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
extern "C" {
int _ZN6Player17St_Thrown_CleanupEv(char *c)
{
    *(unsigned int *)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x2000;
    return 1;
}
}
