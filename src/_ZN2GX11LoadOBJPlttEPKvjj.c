// _ZN2GX11LoadOBJPlttEPKvjj - GX::LoadOBJPltt(src, byteOffset, byteSize)
// Loads OBJ palette data to main screen (GX) hardware VRAM

extern void DMASyncHalfTransfer(int channel, const void* src, void* dest, unsigned int count);
extern void MultiCopyHalf(const void* src, void* dest, unsigned int count);

// The global at 0x02099fd0: DMA channel number (or -1)
extern int data_02099fd0;

void _ZN2GX11LoadOBJPlttEPKvjj(const void* src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void*)((char*)0x05000200 + offset), size);
    } else {
        MultiCopyHalf(src, (void*)((char*)0x05000200 + offset), size);
    }
}