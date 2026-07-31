#include "types.h"
extern u32 gDmaChannel;

extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void DMASyncHalfTransfer(u32 channel, const void *src, void *dst, u32 count);
extern void MultiCopyHalf(const void *src, void *dst, u32 count);

void func_02056374(const void *src, u32 offset, u32 count)
{
    void *bgPtr;
    u32 ch;

    bgPtr = _ZN3G2S12GetBG2ScrPtrEv();
    ch = gDmaChannel;
    if (ch != (u32)~0)
        DMASyncHalfTransfer(ch, src, (char *)bgPtr + offset, count);
    else
        MultiCopyHalf(src, (char *)bgPtr + offset, count);
}