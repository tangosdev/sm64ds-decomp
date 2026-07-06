typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int LoadFile(int handle);
extern s32 func_ov004_020ad674(void);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void DecompressLZ16(int src, void *dst);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void Deallocate(void *ptr);
extern void func_ov006_02122e20(void *self, int a);

extern int data_ov006_0213fc34[];

void func_ov006_021245a8(void *arg0)
{
    void *sb = arg0;
    int r8, r7, r6, r5, r4;
    volatile u16 sp0;

    r8 = LoadFile(data_ov006_0213fc34[func_ov004_020ad674()]);
    r7 = LoadFile(0x108);
    r6 = LoadFile(0xa2);
    r5 = LoadFile(0xa4);
    r4 = LoadFile(0xa3);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9110;
    *(volatile u16 *)0x400000c &= ~0x40;

    {
        char *dst = _ZN2G213GetBG2CharPtrEv();
        sp0 = 0;
        MultiStore16(sp0, dst, 0x4000);
    }
    DecompressLZ16(r6, (void *)(_ZN2G213GetBG2CharPtrEv() + 0x4000));
    DecompressLZ16(r5, _ZN2G212GetBG2ScrPtrEv());
    _ZN2GX10LoadBGPlttEPKvjj((const void *)r4, 0x1a0, 0x60);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1318;
    *(volatile u16 *)0x400000e &= ~0x40;

    DecompressLZ16(r8, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)r7, 0, 0x60);

    Deallocate((void *)r6);
    Deallocate((void *)r5);
    Deallocate((void *)r4);
    Deallocate((void *)r8);
    Deallocate((void *)r7);

    func_ov006_02122e20(sb, 3);
}
