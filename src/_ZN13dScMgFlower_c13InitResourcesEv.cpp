//cpp
// @symbol _ZN13dScMgFlower_c13InitResourcesEv
#include "types.h"
#include "decl_common.h"
#include "dScMgFlower_c.h"
/* dScMgFlower_c::InitResources -- vtable slot 0.
 *
 * Attributed by the ROM's vtable, and this one the old `recovered name:` comment
 * got right: slot 0 is one of the three where Flower's table really does differ
 * from dScMgSingle3DBase_c's (the others are 6 and 9). The four addresses that
 * used to claim `dScMgFlower_c_*` for slots 2/5/7/10 were the parent's and moved
 * up in commit 4f7406b9c; see include/dScMgFlower_c.h.
 *
 * Sets up both screens' BG2/BG3 layers, decompresses the tile and screen data
 * behind each, loads the palettes, then hands off to the class's own state reset.
 * The bare 0x040000xx stores are the 2D engine's BGxCNT / BGxOFS registers and
 * the 0x0640_0000 / 0x0660_0000 destinations are the two OBJ VRAM banks.
 *
 * WAS A C99 FILE, so every declaration below moves inside `extern "C"`: in C++
 * these names would mangle and resolve to nothing. The three `extern` variables
 * keep that keyword deliberately -- a variable declaration inside `extern "C" {}`
 * without it is a definition and collides with the delinked gap object. */

extern "C" {
void *func_ov004_020adc74(void *arg);
char *_ZN2G213GetBG2CharPtrEv(void);
void DecompressLZ16(int src, void *dst);
void Ov004_Deallocate(int handle);
void *_ZN3G2S13GetBG2CharPtrEv(void);
char *_ZN2G212GetBG2ScrPtrEv(void);
char *_ZN3G2S12GetBG2ScrPtrEv(void);
char *_ZN3G2S12GetBG3ScrPtrEv(void);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);

extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMgFlower_c::InitResources()
{
    char *c = (char *)this;
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

    h = (int)func_ov004_020adc74(&data_ov006_021401d0);
    DecompressLZ16(h, _ZN2G213GetBG2CharPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_021401f4);
    DecompressLZ16(h, _ZN3G2S13GetBG2CharPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_02140218);
    DecompressLZ16(h, _ZN2G212GetBG2ScrPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_0214023c);
    DecompressLZ16(h, _ZN3G2S12GetBG2ScrPtrEv());
    DecompressLZ16(h, _ZN3G2S12GetBG3ScrPtrEv());
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_02140260);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x1a0);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_02140284);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x1a0);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate(h);

    *(volatile u16 *)0x4000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 4, 8, 6, 0xa);

    data_0209d45c |= 4;
    data_0209d454 |= 0xc;

    h = (int)func_ov004_020adc74(&data_ov006_021402a8);
    DecompressLZ16(h, (void *)0x6400000);
    DecompressLZ16(h, (void *)0x6600000);
    Ov004_Deallocate(h);

    h = (int)func_ov004_020adc74(&data_ov006_021402c4);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(h, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0x100);
    Ov004_Deallocate(h);

    data_ov004_020bc8a8 = 0x40;
    data_ov004_020bc898 = 0xa0;
    data_ov004_020bc86c = 0xc0;
    data_ov004_020bc8a4 = 0xa0;

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
