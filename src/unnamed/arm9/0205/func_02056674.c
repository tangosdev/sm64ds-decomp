/* func_02056674 at 0x02056674 (size 0x68)
 * Copies count words from src to VRAM address (0x07000400 + offset).
 * Uses DMA channel if available, else CPU copy.
 * Destination 0x07000400 is a NDS texture slot VRAM address (ARM9 view).
 */

extern unsigned int data_02099fd0;

extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

void func_02056674(const void *src, unsigned int offset, unsigned int count)
{
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (void *)(0x07000400 + offset), count);
    } else {
        MultiCopy_Int(src, (void *)(0x07000400 + offset), count);
    }
}
