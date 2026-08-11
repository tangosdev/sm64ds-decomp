//cpp
// @symbol _ZN13dScMgTeresa_c13InitResourcesEv
// recovered name: dScMgTeresa_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgTeresa_c.h"
/* dScMgTeresa_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void *LoadFile(int handle);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMgTeresa_c::InitResources()
{
    char *self = (char *)this;
    void *f1;
    void *f2;
    void *t;
    volatile u16 fa;
    volatile u16 fb;
    volatile u16 fc;
    *((volatile u16 *) 0x400000c) &= ~3;
    *((volatile u16 *) 0x400000c) &= ~0x40;
    *((volatile u32 *) 0x4000018) = 0;
    *((volatile u16 *) 0x400000c) = ((*((volatile u16 *) 0x400000c)) & 0x43) | 0x1210;
    {
        void *p = _ZN2G212GetBG2ScrPtrEv();
        fa = 0x5300;
        MultiStore16(fa, p, 0x800);
    }
    data_0209d45c |= 8;
    *((volatile u16 *) 0x400000e) = (*((volatile u16 *) 0x400000e)) & (~3);
    *((volatile u16 *) 0x400000e) &= ~0x40;
    *((volatile u32 *) 0x400001c) = 0;
    *((volatile u16 *) 0x400000e) = ((*((volatile u16 *) 0x400000e)) & 0x43) | 0x9310;
    t = LoadFile(0x9b);
    DecompressLZ16(t, _ZN2G213GetBG2CharPtrEv());
    Deallocate(t);
    t = LoadFile(0x9c);
    _ZN2GX10LoadBGPlttEPKvjj(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0x9e);
    func_02056314(t, 0, 0x800);
    Deallocate(t);
    t = LoadFile(0x9d);
    func_02056314(t, 0x800, 0x800);
    Deallocate(t);
    f1 = LoadFile(0x101);
    f2 = LoadFile(0x102);
    DecompressLZ16(f1, (void *) 0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(f2, 0, 0x100);
    *((volatile u16 *) 0x4001008) = ((*((volatile u16 *) 0x4001008)) & 0x43) | 0x218;
    *((volatile u16 *) 0x4001008) &= ~0x40;
    *((volatile u32 *) 0x4001010) = 0;
    *((volatile u16 *) 0x4001008) &= ~3;
    {
        void *p = _ZN3G2S12GetBG0ScrPtrEv();
        fb = 0;
        MultiStore16(fb, p, 0x800);
    }
    f2 += 0;
    func_ov004_020af2f8(self, 0, 0, 0);
    {
        void *p = _ZN3G2S13GetBG0CharPtrEv();
        fc = 0x1111;
        MultiStore16(fc, p, 0x6000);
    }
    data_0209d454 |= 4;
    *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & (~3)) | 1;
    *((volatile u16 *) 0x400100c) &= ~0x40;
    *((volatile u32 *) 0x4001018) = 0;
    *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & 0x43) | 0x408;
    t = LoadFile(0x9f);
    DecompressLZ16(t, (void *) _ZN3G2S13GetBG2CharPtrEv());
    Deallocate(t);
    t = LoadFile(0xa0);
    _ZN3GXS10LoadBGPlttEPKvjj(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0xa1);
    func_02056374(t, 0, 0x800);
    Deallocate(t);
    DecompressLZ16(f1, (void *) 0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(f2, 0, 0x100);
    Deallocate(f1);
    Deallocate(f2);
    func_ov004_020b04d0(0x20);
    func_ov006_0211fbf8(self);
    *((u8 *) (self + 0x4c23)) = 0xff;
    func_ov006_0211d7b4(self);
    func_ov006_0211dd6c(self);
    func_ov006_0211f77c(self);
    *((int *) (self + 0x4be8)) = 1;
    *((u16 *) (self + 0x4c16)) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    func_020bc880 = 0x80;
    func_020bc884 = -128;
    *((int *) (self + 0xb4)) = 0;
    return 1;
}
