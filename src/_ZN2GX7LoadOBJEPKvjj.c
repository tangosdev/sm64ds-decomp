/* _ZN2GX7LoadOBJEPKvjj at 0x020565b4 (size 0x60)
 * Copies count words from src to VRAM address (0x6400000 + offset).
 * Uses DMA channel if available, else CPU copy.
 */

extern unsigned int data_02099fd0;

extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

#pragma opt_propagation off

void _ZN2GX7LoadOBJEPKvjj(const void *src, unsigned int offset, unsigned int count)
{
    unsigned int channel = data_02099fd0;
    unsigned int base = 0x6400000;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(base + offset), count);
    } else {
        MultiCopy_Int(src, (void *)(base + offset), count);
    }
}
