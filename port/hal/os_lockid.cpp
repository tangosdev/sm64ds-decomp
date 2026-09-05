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
// WHO SEEDS THE WORDS. On the ROM, func_02057320 (hand-written ARM, no src
// body) runs inside func_02058c84's ARM9<->ARM7 handshake and leaves the two
// words at -1 and -0x10000 before anything asks for a lock. The port's pre-main
// span (hal/boot_os.cpp) skips that arm because it spins on the ARM7's
// readiness bits, so port_os_lock_words_seed() writes those same two values at
// the point the ROM's own order puts func_02057320. That is the ROM's
// post-init state, not a guess: func_02057320's two stores are the only writes
// to the pair before the first allocation. When the PXI layer lets that arm
// run as ROM code, this seed retires with the rest of the span's stand-ins.
//
// PORT_HOST_ABI: ARM `clz` primitive (func_02057020); the seed stands in for
//                func_02057320's two stores until the handshake arm runs.
#include <cstdio>

extern "C" {

static volatile unsigned int *lock_words(void)
{
    return (volatile unsigned int *)0x027fffb0u;
}

void port_os_lock_words_seed(void)
{
    volatile unsigned int *w = lock_words();
    w[0] = 0xffffffffu;   /* ids 0x40..0x5f all free */
    w[1] = 0xffff0000u;   /* ids 0x60..0x6f free, 0x70..0x7f reserved */
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
