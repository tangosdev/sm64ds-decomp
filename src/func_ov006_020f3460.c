typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern int func_020adc74(void* p);
extern int func_02054d88(void);
extern void DecompressLZ16(int src, void* dst);
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void Deallocate(void* p);
extern void func_02056314(void* dst, u32 offset, u32 len);
extern char* _ZN2G213GetBG2CharPtrEv(void);
extern char* _ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char* dst, int nbytes);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern unsigned func_02054de8(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_02056374(const void* src, u32 offset, u32 count);
extern char* _ZN3G2S12GetBG3ScrPtrEv(void);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_ov006_020f2ec0(void* c);
extern void func_ov006_020f2e20(char* c);
extern void func_ov004_020adc5c(int a);
extern void func_ov004_020b04d0(int v);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;
extern void* data_ov006_0213cfa0;
extern int func_020bc888;
extern int func_020bc864;

int func_ov006_020f3460(void* arg0) {
    char* c = (char*)arg0;
    char* b;
    volatile u16 sp8;
    volatile u16 spA;
    int r6;
    int f;
    int r5;

    r6 = func_020adc74(&data_ov006_0213cfa0);
    if (r6 == 0) return 0;

    TOP_SCREEN_RELATED |= 8;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & ~3) | 2;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1218;

    DecompressLZ16(r6, (void*)func_02054d88());

    f = LoadFile(0x48);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)f, 0x60, 0x1a0);
    Deallocate((void*)f);

    f = LoadFile(0x49);
    func_02056314((void*)f, 0, 0x800);
    Deallocate((void*)f);

    TOP_SCREEN_RELATED |= 4;
    *(volatile u16*)0x400000c &= ~3;
    *(volatile u16*)0x400000c &= ~0x40;
    *(volatile u32*)0x4000018 = 0;
    *(volatile u16*)0x400000c = (*(volatile u16*)0x400000c & 0x43) | 0x1410;

    f = LoadFile(0x4b);
    DecompressLZ16(f, (void*)(_ZN2G213GetBG2CharPtrEv() + 0x4000));
    Deallocate((void*)f);

    f = LoadFile(0x4c);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)f, 0x40, 0x20);
    Deallocate((void*)f);

    b = _ZN2G212GetBG2ScrPtrEv();
    sp8 = 0x23e1;
    MultiStore16(sp8, b, 0x800);

    r5 = LoadFile(0xd3);
    f = LoadFile(0xd4);
    DecompressLZ16(r5, (void*)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void*)f, 0, 0x100);

    *(volatile u16*)0x400100c &= ~3;
    *(volatile u16*)0x400100c &= ~0x40;
    *(volatile u32*)0x4001018 = 0;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0x1210;

    {
        int f8;
        f8 = LoadFile(0x4b);
        DecompressLZ16(f8, (void*)(func_02054de8() + 0x4000));
        Deallocate((void*)f8);

        f8 = LoadFile(0x4c);
        _ZN3GXS10LoadBGPlttEPKvjj((const void*)f8, 0x1e0, 0x20);
        Deallocate((void*)f8);

        f8 = LoadFile(0x4d);
        func_02056374((const void*)f8, 0, 0x800);
        Deallocate((void*)f8);

        BOTTOM_SCREEN_RELATED |= 8;
        *(volatile u16*)0x400100e &= ~3;
        *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0x1410;

        f8 = LoadFile(0x4a);
        b = _ZN3G2S12GetBG3ScrPtrEv();
        spA = 0xf3e1;
        MultiStore16(spA, b, 0x800);
        Deallocate((void*)f8);
    }

    DecompressLZ16(r5, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void*)f, 0, 0x100);
    Deallocate((void*)r5);
    Deallocate((void*)f);

    *(u8*)(c + 0x5000 + 0x457) = 0;
    func_ov006_020f2ec0(arg0);
    *(volatile u16*)(c + 0x5100 + 0x74) = 0xff;
    func_ov006_020f2e20(c);

    *(int*)(c + 0x4000 + 0xf78) = 0;
    func_ov004_020adc5c(r6);
    func_ov004_020b04d0(0x30);
    *(volatile u16*)(c + 0x5100 + 0x72) = 0xa;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    func_020bc888 = 0x80;
    func_020bc864 = ~0x1b;
    *(int*)(c + 0xb4) = 0;
    return 1;
}
