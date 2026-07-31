// @symbol func_ov006_0212b480
// @emits dScMgFlower_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::InitResources - recovered from vtable slot identity */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void *func_020adc74(void *arg);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern void DecompressLZ16(int src, void *dst);
extern void Ov004_Deallocate(int handle);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern char *_ZN3G2S12GetBG2ScrPtrEv(void);
extern char *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);

extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;

int dScMgFlower_c_InitResources(void *arg0)
{
    char *c = (char *)arg0;
    int h;

    *(int *)(c + 0x5ff0) = func_ov004_020ad8b8();

    data_0208ee44 = 1;

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1414;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 3;

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1414;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1414;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    h = (int)func_020adc74(&data_ov006_021401d0);
    DecompressLZ16(h, _ZN2G213GetBG2CharPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_020adc74(&data_ov006_021401f4);
    DecompressLZ16(h, _ZN3G2S13GetBG2CharPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_020adc74(&data_ov006_02140218);
    DecompressLZ16(h, _ZN2G212GetBG2ScrPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_020adc74(&data_ov006_0214023c);
    DecompressLZ16(h, _ZN3G2S12GetBG2ScrPtrEv());
    DecompressLZ16(h, _ZN3G2S12GetBG3ScrPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_020adc74(&data_ov006_02140260);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x1a0);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate(h);

    h = (int)func_020adc74(&data_ov006_02140284);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x1a0);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate(h);

    *(volatile u16 *)0x4000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16 *)0x4001050, 4, 8, 6, 0xa);

    data_0209d45c |= 4;
    data_0209d454 |= 0xc;

    h = (int)func_020adc74(&data_ov006_021402a8);
    DecompressLZ16(h, (void *)0x6400000);
    DecompressLZ16(h, (void *)0x6600000);
    Ov004_Deallocate(h);

    h = (int)func_020adc74(&data_ov006_021402c4);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0x100);
    Ov004_Deallocate(h);

    func_020bc8a8 = 0x40;
    func_020bc898 = 0xa0;
    func_020bc86c = 0xc0;
    func_020bc8a4 = 0xa0;

    *(int *)(c + 0x5fdc) = 0;
    *(int *)(c + 0x5fe0) = 0;
    *(int *)(c + 0x5fe4) = 0;
    func_ov006_0212a764(c);

    data_0209d45c |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    func_ov006_020c3d88(c + 0x51f8);
    func_ov006_020c3b2c(c + 0x4660);

    return 1;
}
