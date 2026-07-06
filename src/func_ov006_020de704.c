typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int LoadFile(int handle);
extern int func_02054d88(void);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *p);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN3G2S13GetBG3CharPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_ov006_020ddf9c(char *c);
extern void func_ov006_020dd334(char *c);
extern void func_ov006_020dc7b4(char *c);
extern void func_ov006_020dc2f8(char *c);
extern void func_ov004_020b04d0(int v);
extern int func_ov004_020adc1c(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;

int func_ov006_020de704(void *arg0) {
    char *c = (char*)arg0;
    int a, b, d;

    data_0209d45c |= 8;
    *(volatile u16*)0x400000e &= ~3;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1210;

    a = LoadFile(0xa9);
    DecompressLZ16(a, (void*)(func_02054d88() + 0x4000));
    Deallocate((void*)a);

    a = LoadFile(0xaa);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)a, 0x1e0, 0x20);
    Deallocate((void*)a);

    a = LoadFile(0xab);
    func_02056314((void*)a, 0, 0x800);
    Deallocate((void*)a);

    b = LoadFile(0x10a);
    a = LoadFile(0x10b);
    DecompressLZ16(b, (void*)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void*)a, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 3;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0x1210;

    d = LoadFile(0xa9);
    DecompressLZ16(d, (void*)(_ZN3G2S13GetBG3CharPtrEv() + 0x4000));
    Deallocate((void*)d);

    d = LoadFile(0xaa);
    _ZN3GXS10LoadBGPlttEPKvjj((const void*)d, 0x1e0, 0x20);
    Deallocate((void*)d);

    d = LoadFile(0xa8);
    func_020562b4((const void*)d, 0, 0x800);
    Deallocate((void*)d);

    DecompressLZ16(b, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void*)a, 0, 0x100);
    Deallocate((void*)b);
    Deallocate((void*)a);

    *(int*)(c + 0xa8) = 0;
    *(int*)(c + 0xac) = *(int*)(c + 0xa8);
    data_0208ee44 = 1;
    func_ov006_020ddf9c(c);
    func_ov006_020dd334(c);
    func_ov006_020dc7b4(c);
    func_ov006_020dc2f8(c);
    *(u8*)(c + 0x5000 + 0x1da) = 0;
    *(u8*)(c + 0x5000 + 0x1dc) = 2;
    func_ov004_020b04d0(0x20);
    *(int*)(c + 0x5000 + 0x1d4) = func_ov004_020adc1c();
    *(int*)(c + 0xa4) = 1;
    return 1;
}
