//cpp
/* func_ov006_0210a194 @ 0x0210a194 (ov006, size 0x26c)
 * Minigame graphics init variant: sets up main+sub BG control regs, loads and
 * decompresses two BG char/scroll banks + a BG palette, clears sub BG0/BG2
 * scroll offsets, reconfigures the BG priority bits, loads a second pair of
 * files decompressed to fixed OBJ VRAM banks with OBJ palettes for both
 * engines, sets two state bytes, patches main-engine BG0 priority, then runs
 * the camera/actor init hooks and finishes with a virtual init call.
 */
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void m18(int);
};

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;

extern void func_ov004_020b04d0(int);
extern void *LoadFile(int);
extern unsigned func_02054de8(void);
extern void DecompressLZ16(void *, void *);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *, u32, u32);
extern void Deallocate(void *);
extern void SetSubBg0Offset(int, int);
extern void SetSubBg2Offset(int, int);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *, u32, u32);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *, u32, u32);
extern void func_ov006_020c0aa8(char *);
extern int func_ov006_020c1a88(char *);
extern int func_ov006_021085c0(char *);
extern int func_ov004_020ad8b8(void);
extern void func_ov004_020b682c(void);
extern void func_ov006_02107b70(char *);

int func_ov006_0210a194(char *c)
{
    void *f8, *f7, *f6, *f5;

    func_ov004_020b04d0(0x20);

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x404;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x504;

    f8 = LoadFile(0x6f);
    f7 = LoadFile(0x71);
    f6 = LoadFile(0x1e4);
    f5 = LoadFile(0x70);

    DecompressLZ16(f8, (void *)func_02054de8());
    DecompressLZ16(f7, _ZN3G2S12GetBG2ScrPtrEv());
    DecompressLZ16(f6, _ZN3G2S12GetBG0ScrPtrEv());
    _ZN3GXS10LoadBGPlttEPKvjj(f5, 0x1a0, 0x60);

    Deallocate(f8);
    Deallocate(f7);
    Deallocate(f6);
    Deallocate(f5);

    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u16 *)0x400100c &= ~0x40;
    SetSubBg0Offset(0, 0);
    SetSubBg2Offset(0, 0);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 2;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;

    f6 = LoadFile(0xe9);
    f5 = LoadFile(0xea);

    DecompressLZ16(f6, (void *)0x6400000);
    DecompressLZ16(f6, (void *)0x6600000);

    _ZN2GX11LoadOBJPlttEPKvjj(f5, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj(f5, 0, 0x100);

    Deallocate(f6);
    Deallocate(f5);

    data_0209d45c = 0x11;
    data_0209d454 = 0x15;

    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;

    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0)
        return 0;

    if (func_ov006_021085c0(c + 0x530c) == 0)
        return 0;

    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);

    func_ov004_020b682c();

    func_ov006_02107b70(c + 0x52ac);

    ((VtObj *)c)->m18(-1);

    return 1;
}
}
