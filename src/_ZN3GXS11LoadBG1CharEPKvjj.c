/* _ZN3GXS11LoadBG1CharEPKvjj at 0x02056134 (size 0x60)
 * GXS::LoadBG1Char(src, offset, size) - loads character data into SUB BG1 VRAM.
 * Calls G2S::GetBG1CharPtr() to get base pointer, then DMA or CPU copy.
 */

extern unsigned int data_02099fd0;

extern void *_ZN3G2S13GetBG1CharPtrEv(void);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

void _ZN3GXS11LoadBG1CharEPKvjj(const void *src, unsigned int offset, unsigned int count)
{
    void *base = _ZN3G2S13GetBG1CharPtrEv();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int(src, (char *)base + offset, count);
    }
}
