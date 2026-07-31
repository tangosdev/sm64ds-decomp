typedef unsigned int u32;
typedef int s32;

extern s32 RENDER_DMA_CHANNEL;

void *_ZN2G212GetBG3ScrPtrEv(void);
void DMASyncHalfTransfer(s32 channel, void *dst, void *src, u32 len);
void MultiCopyHalf(void *dst, void *src, u32 len);

void func_02056314(void *dst, u32 offset, u32 len) {
    void *ip = _ZN2G212GetBG3ScrPtrEv();
    s32 chan = RENDER_DMA_CHANNEL;
    if (chan != -1) {
        DMASyncHalfTransfer(chan, dst, (char *)ip + offset, len);
    } else {
        MultiCopyHalf(dst, (char *)ip + offset, len);
    }
}
