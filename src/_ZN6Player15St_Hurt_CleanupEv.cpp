//cpp
// Player::St_Hurt_Cleanup - clear flag bit 0x80 in the u32 at this+0xb0, return 1.
// The u64-mask launder forces the base to materialize (add r2,this,#0xb0) so the
// load/store reuse it, matching the ROM instead of folding the offset into ldr/str.
extern "C" {
int _ZN6Player15St_Hurt_CleanupEv(char *c)
{
    *(unsigned int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80;
    return 1;
}
}
