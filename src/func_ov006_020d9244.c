typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int LoadFile(int handle);
extern int func_02054d88(void);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *p);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN3G2S13GetBG3CharPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_02056554(const void *src, u32 offset, u32 count);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, u16 a, u16 b, u16 c, u16 d);
extern void func_ov006_020d907c(void *p);
extern void func_ov006_020d6630(void *p);
extern void func_ov006_020d62e0(void *p);
extern void func_ov006_020d604c(void *p);
extern void func_ov004_020b04d0(int v);
extern void func_ov004_020adb1c(int v);

extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;

int func_ov006_020d9244(void *arg0)
{
    char *c = (char *)arg0;
    char *b;
    volatile u16 sp4;
    int f;
    int r5;

    TOP_SCREEN_RELATED |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 2;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x23);
    DecompressLZ16(f, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)f);

    f = LoadFile(0x24);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x180, 0x80);
    Deallocate((void *)f);

    f = LoadFile(0x25);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    TOP_SCREEN_RELATED |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9410;

    f = LoadFile(3);
    b = _ZN2G212GetBG2ScrPtrEv();
    sp4 = 0xf23f;
    MultiStore16(sp4, b, 0x1000);
    func_020563d4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    TOP_SCREEN_RELATED |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3);
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x5610;

    f = LoadFile(2);
    func_02056554((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    f = LoadFile(7);
    func_02056554((const void *)f, 0x800, 0x800);
    Deallocate((void *)f);

    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 1, 0x1c, 4, 3);

    r5 = LoadFile(0xb5);
    f = LoadFile(0xb6);
    DecompressLZ16(r5, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);

    BOTTOM_SCREEN_RELATED |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x210;

    {
        int f7 = LoadFile(0x23);
        DecompressLZ16(f7, (void *)(_ZN3G2S13GetBG3CharPtrEv() + 0x4000));
        Deallocate((void *)f7);

        f7 = LoadFile(0x24);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)f7, 0x180, 0x80);
        Deallocate((void *)f7);

        f7 = LoadFile(0x22);
        func_020562b4((const void *)f7, 0, 0x800);
        Deallocate((void *)f7);

        DecompressLZ16(r5, (void *)0x6600000);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);
        Deallocate((void *)r5);
        Deallocate((void *)f);
    }

    func_ov006_020d907c(c);
    func_ov006_020d6630(c);
    func_ov006_020d62e0(c);
    func_ov006_020d604c(c);
    *(int *)(c + 0x6000 + 0x2d0) = 1;
    *(u16 *)(c + 0x6200 + 0xee) = 0;
    func_ov004_020b04d0(0x20);
    func_ov004_020adb1c(0);
    return 1;
}
