/* _ZN2GX11LoadBG0CharEPKvjj at 0x02056254 (size 0x60)
 * GX::LoadBG0Char(src, offset, size) - loads character data into MAIN BG0 VRAM.
 * Calls G2::GetBG0CharPtr()-equivalent (0x2054efc) to get base, then DMA or CPU copy.
 */

extern unsigned int data_02099fd0;

extern void *func_02054efc(void);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

void _ZN2GX11LoadBG0CharEPKvjj(const void *src, unsigned int offset, unsigned int count)
{
    void *base = func_02054efc();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int(src, (char *)base + offset, count);
    }
}
