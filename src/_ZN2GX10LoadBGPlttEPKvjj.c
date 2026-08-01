// _ZN2GX10LoadBGPlttEPKvjj - GX::LoadBGPltt(src, byteOffset, byteSize)
// Loads BG palette data to main screen (GX) hardware VRAM

extern void DMASyncHalfTransfer(int channel, const void* src, void* dest, unsigned int count);
extern void MultiCopyHalf(const void* src, void* dest, unsigned int count);

extern int data_02099fd0;

void _ZN2GX10LoadBGPlttEPKvjj(const void* src, unsigned int offset, unsigned int size) {
    int channel = data_02099fd0;
    if (channel != -1) {
        DMASyncHalfTransfer(channel, src, (void*)(offset + 0x05000000), size);
    } else {
        MultiCopyHalf(src, (void*)(offset + 0x05000000), size);
    }
}