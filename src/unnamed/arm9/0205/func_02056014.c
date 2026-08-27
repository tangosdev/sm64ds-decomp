/* func_02056014 at 0x02056014 (size 0x60)
 * Copies count words from src to GetBGChar_0x2054d88()-based VRAM region + offset.
 * Uses DMA channel if available, else CPU copy.
 */

extern unsigned int data_02099fd0;

extern void *func_02054d88(void);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

void func_02056014(const void *src, unsigned int offset, unsigned int count)
{
    void *base = func_02054d88();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int(src, (char *)base + offset, count);
    }
}
