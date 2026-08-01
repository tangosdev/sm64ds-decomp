/* _ZN2GX11LoadBG1CharEPKvjj at 0x02056194 (size 0x60)
 * GX::LoadBG1Char(src, offset, size) - loads character data into MAIN BG1 VRAM.
 * Calls G2::GetBG1CharPtr()-equivalent (0x2054ea8) to get base, then DMA or CPU copy.
 */

extern unsigned int data_02099fd0;

extern void *func_02054ea8(void);
extern void DMASyncWordTransfer(unsigned int channel, const void *src, void *dst, unsigned int count);
extern void MultiCopy_Int(const void *src, void *dst, unsigned int count);

void _ZN2GX11LoadBG1CharEPKvjj(const void *src, unsigned int offset, unsigned int count)
{
    void *base = func_02054ea8();
    unsigned int channel = data_02099fd0;
    if (channel != (unsigned int)-1) {
        DMASyncWordTransfer(channel, src, (char *)base + offset, count);
    } else {
        MultiCopy_Int(src, (char *)base + offset, count);
    }
}
