// The ROM's OS lock-id allocator, in one file every binary links.
//
// src/func_02057020.c (OS_GetLockID) is a `clz` search over the two 32-bit
// words at 0x027fffb0, clearing the highest free bit and returning 0x40+index
// (first word) or 0x60+index (second), or -3 when both are full. The port
// hosts it here instead of in src/ because MSVC cannot assemble the `clz`
// block; the arithmetic below is the exact equivalent, verified against the
// asm block instruction by instruction (lane BOOT wrote it in hal/boot_hw.cpp;
// it moved here at integration because SaveData::SaveDataToCart and
// ReadDataFromCart call it from smoke_player too, which never links the boot
// gate, and lane SAVE's constant-id face in ntr/backup.cpp collided with the
// faithful body on walk_window).
//
// On the ROM, func_02057320 seeds both words before the first allocation.
// The host also runs FS_Init from a static initializer, before the ordinary
// boot span. That caller seeds the words explicitly after the shared page is
// mapped. The later boot call must preserve IDs already handed to FS, so the
// stores run once per process. The function-local initializer also serializes
// simultaneous seed calls; allocation keeps the ROM caller's synchronization
// contract. Exhausting the pool does not make it eligible for another seed.
//
// PORT_HOST_ABI: ARM `clz` primitive (func_02057020); the seed stands in for
//                func_02057320's two stores until the handshake arm runs.

extern "C" {

static volatile unsigned int *lock_words(void)
{
    return (volatile unsigned int *)0x027fffb0u;
}

static bool seed_lock_words(void)
{
    volatile unsigned int *w = lock_words();
    w[0] = 0xffffffffu;   /* ids 0x40..0x5f all free */
    w[1] = 0xffff0000u;   /* ids 0x60..0x6f free, 0x70..0x7f reserved */
    return true;
}

void port_os_lock_words_seed(void)
{
    static const bool seeded = seed_lock_words();
    (void)seeded;
}

int func_02057020(void) {
    volatile unsigned int *w = lock_words();
    unsigned int base;
    int idx;
    unsigned int v = w[0];
    if (v != 0) {
        base = 0x40u; idx = 0;
    } else {
        v = w[1];
        if (v == 0) return -3;
        base = 0x60u; idx = 1;
    }
    unsigned int lz = 0;
    while ((v & 0x80000000u) == 0) { v <<= 1; ++lz; }
    w[idx] &= ~(0x80000000u >> lz);
    return (int)(base + lz);
}

}
