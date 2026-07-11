/* _ZN2GX7LoadOBJEPKvjj (GX::LoadOBJ) at 0x02056614 (arm9), size 0x60
 * Compiler mwccarm 1.2/sp2p3 */
extern unsigned int RENDER_DMA_CHANNEL;

extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

#pragma opt_propagation off
void _ZN2GX7LoadOBJEPKvjj(const void *src, unsigned int offset, unsigned int count)
{
    unsigned int channel = RENDER_DMA_CHANNEL;
    char *base = (char *)0x6400000;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, base + offset, count);
    } else {
        MultiCopy_Int(src, base + offset, count);
    }
}
