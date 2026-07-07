//cpp
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {

extern int func_02054d88(void);
extern u32 LoadCompressedFileAt(u16 fileID, void *target);
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void Deallocate(void *ptr);
extern void *_ZN2G212GetBG1ScrPtrEv(void);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern void *func_02054de8(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
extern void func_ov006_020c2154(char *c);
extern void func_ov006_020c1eb4(char *c);
extern int RandomIntInternal(int *seed);
extern void func_ov004_020b04d0(int v);

extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0209e650;
extern int data_0208ee44;
extern int func_020bc88c;
extern int func_020bc878;
extern int func_020bc8b8;
extern int func_020bc888;
extern int func_020bc860;
extern int func_020bc890;
extern int func_020bc8b4;
extern int func_020bc864;

}

struct Obj {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int a);
};

extern "C" int func_ov006_0210bdb0(void *arg0)
{
    char *c = (char *)arg0;

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1118;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1218;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1418;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~3;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~3;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    LoadCompressedFileAt(0x77, (void *)func_02054d88());

    {
        int h = LoadFile(0x78);
        _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(0x75, _ZN2G212GetBG1ScrPtrEv());
    LoadCompressedFileAt(0x7a, _ZN2G212GetBG2ScrPtrEv());

    {
        int h = LoadFile(0x79);
        func_02056314((void *)h, 0, 0x800);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(0xeb, (void *)0x6400000);

    data_0209d45c = 0x1f;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;

    LoadCompressedFileAt(0x77, func_02054de8());
    LoadCompressedFileAt(0x76, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(0xeb, (void *)0x6600000);

    {
        int h = LoadFile(0xec);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)h, 0xe0);
        _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x14;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16 *)0x4000050, 0, 0xc, 0xc, 4);

    func_ov006_020c2154(c + 0x4f38);
    func_ov006_020c1eb4(c + 0x4660);

    *(int *)(c + 0x5004) = *(int *)(c + 0xbc);

    ((struct Obj *)c)->m48(3);

    *(u8 *)(c + 0x5000 + 0x3b) = *(u8 *)(c + 0x5000 + 0x3c);
    *(int *)(c + 0x5000 + 0xc) = 0;

    for (int i = 0; i < 3; i++) {
        int rnd = RandomIntInternal(&data_0209e650);
        u32 divisor = *(u8 *)(c + 0x5000 + 0x3a);
        u32 val = ((u32)rnd >> 16) % divisor;
        ((u32 *)(c + 0x4fe4))[i] = val * 0x50000;
    }

    func_020bc88c = 0x80;
    func_020bc860 = 0xa0;
    func_020bc878 = 0x80;
    func_020bc890 = 0xa0;
    func_020bc8b8 = 0x80;
    func_020bc8b4 = 0x60;
    func_020bc888 = 0x80;
    func_020bc864 = ~0x1b;

    func_ov004_020b04d0(0x20);

    data_0208ee44 = 1;

    *(u16 *)(c + 0x5018) = 0;
    *(u16 *)(c + 0x501a) = 0;
    return 1;
}
