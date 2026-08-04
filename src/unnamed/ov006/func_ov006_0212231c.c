#include "types.h"
extern int LoadFile(int handle);
extern s32 GetGameLanguage(void);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void DecompressLZ16(int src, void *dst);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void Deallocate(void *ptr);
extern void func_ov006_02120f18(void *self, int a);

extern int data_ov006_0213fadc[];

void func_ov006_0212231c(void *arg0)
{
    void *sb = arg0;
    int r8, r7, r6, r5, r4;
    volatile u16 sp0;

    r8 = LoadFile(0xa5);
    r7 = LoadFile(0xa7);
    r6 = LoadFile(0xa6);
    r5 = LoadFile(data_ov006_0213fadc[GetGameLanguage()]);
    r4 = LoadFile(0x108);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9110;
    *(volatile u16 *)0x400000c &= ~0x40;

    {
        char *dst = _ZN2G213GetBG2CharPtrEv();
        sp0 = 0;
        MultiStore16(sp0, dst, 0x4000);
    }
    DecompressLZ16(r8, (void *)(_ZN2G213GetBG2CharPtrEv() + 0x4000));
    DecompressLZ16(r7, _ZN2G212GetBG2ScrPtrEv());
    _ZN2GX10LoadBGPlttEPKvjj((const void *)r6, 0x1a0, 0x60);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1318;
    *(volatile u16 *)0x400000e &= ~0x40;

    DecompressLZ16(r5, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)r4, 0, 0x60);

    Deallocate((void *)r8);
    Deallocate((void *)r7);
    Deallocate((void *)r6);
    Deallocate((void *)r5);
    Deallocate((void *)r4);

    func_ov006_02120f18(sb, 3);
}
