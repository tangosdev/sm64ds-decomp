//cpp
/* engine/gx/G3X.cpp — G3X TU (arm9 0x020554bc..0x020556d0)
 *
 * G3X is the 3D engine's fog and clear-color register writer set. All three
 * members are static (no this); func_020555a4 uploads a 32-entry toon table
 * from a caller buffer to TOON_TABLE (0x4000380), and the run's two free
 * helpers bracket the members (fog fill below, FIFO drain above).
 */
// leftovers: SetClearColor reads the bool slot as an int (byte-proven
// widening; the header's bool spelling emits a narrower compare at the call).
// The three func_ helpers keep address names (no recovered names exist).
// func_020554bc's split volatile zeroing (init for a, store for b) is as
// built: merging b into an init reschedules the body.
// data_02099fd0 is the shared render DMA channel (named at its GX.cpp use).
#include "G3X.h"

extern "C" {
void MultiCopyHalf(const void *src, void *dst, int size);
void Copy32Bytes(void *src, void *dst);
extern int data_02099fd0;
void func_0205a064(unsigned int ch, unsigned int src, unsigned int dst,
                   unsigned int size, void (*cb)(unsigned int), unsigned int cbArg);
void DMASyncFillTransfer(unsigned int channel, void *dst, unsigned int data,
                         unsigned int size);
void MultiStore_Int(int val, int *dst, int len);
extern int func_02055490(int *out);
extern int func_02055464(int *out);
}

// @symbol func_02055624
extern "C" void func_02055624(void)
{
    int a;
    int b;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    while (func_02055490(&a));
    while (func_02055464(&b));
    *(volatile unsigned int *)0x4000440 = 3;
    *(volatile unsigned int *)0x4000454 = 0;
    *(volatile unsigned int *)0x4000440 = 0;
    if (b != 0) *(volatile unsigned int *)0x4000448 = b;
    *(volatile unsigned int *)0x4000440 = 2;
    *(volatile unsigned int *)0x4000448 = a;
    *(volatile unsigned int *)0x4000454 = 0;
}

// @symbol _ZN3G3X6SetFogEiiii
/* The Nitro SDK's G3X_SetFog(BOOL enable, GXFogBlend, GXFogSlope, int): the
   flag is an int. The body tests it the same either way, and dScStage_c's
   RenderFog passes the stage's fog byte with no 0/1 conversion, which a bool
   parameter would insert. */
void G3X::SetFog(int enable, int a, int b, int c) {
    if (enable) {
        *(volatile unsigned short *)0x400035c = (unsigned short)c;
        *(volatile unsigned short *)0x4000060 =
            (b << 8) | (a << 6) | 0x80 | (*(volatile unsigned short *)0x4000060 & ~0x3f40);
    } else {
        *(volatile unsigned short *)0x4000060 = *(volatile unsigned short *)0x4000060 & 0xcf7f;
    }
}

// FOG_TABLE (0x4000360) takes the whole 32-byte table from the caller buffer.
// @symbol _ZN3G3X11SetFogTableEPv
void G3X::SetFogTable(void *table) {
    Copy32Bytes(table, (void *)0x4000360);
}

// @symbol func_020555a4
extern "C" void func_020555a4(void *dst) {
    MultiCopyHalf(dst, (void *)0x04000380, 0x40);
}

// @symbol _ZN3G3X13SetClearColorEtiiib
void G3X::SetClearColor(unsigned short a, int b, int c, int d, bool e) {
    unsigned int v = (a | (b << 16)) | (d << 24);
    if (*(int *)&e) v |= 0x8000;  // bool slot reads whole-word (see leftovers)
    *(volatile unsigned int *)0x4000350 = v;
    *(volatile unsigned short *)0x4000354 = (unsigned short)c;
}

// @symbol func_020554bc
extern "C" void func_020554bc(void)
{
    int i;
    if (data_02099fd0 != -1) {
        func_0205a064(data_02099fd0, 0x4000330, 0, 0x10, 0, 0);
        DMASyncFillTransfer(data_02099fd0, (void *)0x4000360, 0, 0x50);
    } else {
        volatile int a = 0;
        volatile int b;
        MultiStore_Int(a, (int*)0x4000330, 0x10);
        b = 0;
        MultiStore_Int(b, (int*)0x4000360, 0x50);
    }
    for (i = 0; i < 0x20; i++)
        *(volatile int*)0x40004d0 = 0;
}
