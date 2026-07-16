/* DMASyncFillTransfer at 0x0205a1b4
 * Synchronous 32-bit DMA fill: waits for the DMA channel to be free, writes
 * the fill value into the channel's DMA fill register (0x040000E0 + ch*4),
 * then starts a source-fixed word DMA from that register (ctrl 0x85000000 =
 * enable | 32-bit | src-fixed | immediate) and waits for completion.
 *
 * The identity `& 0xFFFFFFFFFFFFFFFF` on the fill-register address is the
 * u64-laundering idiom (see notes/mwccarm-codegen.md, 6m): it routes the
 * address through 64-bit arithmetic so mwccarm cannot fold it into the
 * store's addressing mode, which makes the compiler materialize the address
 * in fillReg's home register (r1, its call-arg slot) and lets the peephole
 * fuse `add r1,r1,r0,lsl#2` + `str r2,[r1]` into the ROM's pre-indexed
 * writeback `str r2,[r1,r0,lsl#2]!` (and, in turn, evacuates dst to ip at
 * entry exactly like the ROM).
 */

typedef unsigned int u32;
typedef volatile u32 vu32;

extern void DMAStartTransferFB(u32 channel, void *src, void *dst, u32 cnt);

void DMASyncFillTransfer(u32 channel, void *dst, u32 data, u32 size)
{
    vu32 *dmaCtrl;
    vu32 *fillReg;
    u32 *dmaBase;

    if (size == 0)
        return;

    dmaBase = (u32 *)0x040000b0;
    dmaCtrl = (vu32 *)dmaBase + (channel * 3 + 2);

    while (*dmaCtrl & 0x80000000)
        ;

    fillReg = (vu32 *)(((u32)0x040000e0 + (channel << 2)) & 0xFFFFFFFFFFFFFFFF);
    *fillReg = data;
    DMAStartTransferFB(channel, (void *)fillReg, dst, (size >> 2) | 0x85000000);

    while (*dmaCtrl & 0x80000000)
        ;
}
