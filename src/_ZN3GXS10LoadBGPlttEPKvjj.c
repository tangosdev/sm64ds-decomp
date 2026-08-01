// _ZN3GXS10LoadBGPlttEPKvjj - GXS::LoadBGPltt(src, byteOffset, byteSize)
// Loads BG palette data to sub-screen (GXS) hardware VRAM

extern void DMASyncHalfTransfer(int channel, const void* src, void* dest, unsigned int count);
extern void MultiCopyHalf(const void* src, void* dest, unsigned int count);

extern int data_02099fd0;

void _ZN3GXS10LoadBGPlttEPKvjj(const void* src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void*)((char*)0x05000400 + offset), size);
    } else {
        MultiCopyHalf(src, (void*)((char*)0x05000400 + offset), size);
    }
}