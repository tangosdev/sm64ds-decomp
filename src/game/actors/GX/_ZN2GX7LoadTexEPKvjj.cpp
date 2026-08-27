//cpp
/* _ZN2GX7LoadTexEPKvjj (GX::LoadTex) @ 0x02056a50 (arm9, size 0x14c)   [mwccarm 1.2/sp2p3]
 * VRAM texture upload with banked destination: below-top goes to the base window,
 * above-top to the secondary bank, straddling splits into two DMA (or CPU copy)
 * halves. Lever that matched: base/top typed as plain int (r4/r5 web identity),
 * comparisons left to natural unsigned promotion for lo/hs condition codes.
 */
extern "C" void DMASyncWordTransfer(unsigned int channel, void const *src, void *dest, unsigned int len);
extern "C" void func_02059fd0(int ch, int src, int dst, unsigned int size, void (*cb)(int), int cbarg);
extern "C" void MultiCopy_Int(int *src, int *dst, int len);

extern unsigned int data_020a60bc;
extern unsigned int data_020a60ac;
extern unsigned int data_020a60c0;
extern int data_02099fd0;

namespace GX {
void LoadTex(void const *src, unsigned int offset, unsigned int size) {
    unsigned int dst;
    int base = data_020a60bc;
    if (base == 0) {
        dst = data_020a60ac + offset;
    } else {
        int top = data_020a60c0;
        if (offset + size < top) {
            dst = data_020a60ac + offset;
        } else if (offset >= top) {
            dst = base + offset - top;
        } else {
            top -= offset;
            unsigned int d0 = data_020a60ac + offset;
            int ch = data_02099fd0;
            if (ch != -1) {
                DMASyncWordTransfer(ch, src, (void*)d0, top);
                func_02059fd0(data_02099fd0, (int)((char*)src + top), base, size - top, 0, 0);
                return;
            }
            MultiCopy_Int((int*)src, (int*)d0, top);
            MultiCopy_Int((int*)((char*)src + top), (int*)base, size - top);
            return;
        }
    }
    if (data_02099fd0 != -1) {
        func_02059fd0(data_02099fd0, (int)src, dst, size, 0, 0);
        return;
    }
    MultiCopy_Int((int*)src, (int*)dst, size);
}
}
