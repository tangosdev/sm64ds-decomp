//cpp
// @symbol _ZN9BootScene13InitResourcesEv
/* BootScene::InitResources -- vtable slot 0, arm9 0x02005a58.
 *
 * The very first thing the game draws. It hands the four VRAM banks to BG/OBJ
 * on both screens, brings up a text-mode main BG and three sub BGs, decompresses
 * the same 0x020918c4 tile set into both screens' character memory with the
 * 0x020914e0 palette, starts the boot countdown at 0x3c frames, and kicks off a
 * fade through func_0201a244.
 *
 * The register writes stay as literal volatile stores to the ARM7/9 I/O block:
 * this tree has no register header, and the read-modify-write masks are the
 * ROM's own.
 */
#include "BootScene.h"
#include "decl_common.h"

extern "C" {
extern u8   data_0209d45c;
extern u8   data_0209d454;
extern char data_0208ee44;
extern char data_0209f5e8;
extern u8   data_0209f1e8;
extern void func_0201a2f8(void);

void _ZN2GX12SetBankForBGEt(u16 bank);
void _ZN2GX13SetBankForOBJEt(u16 bank);
void _ZN2GX15SetBankForSubBGEt(u16 bank);
void _ZN2GX16SetBankForSubOBJEt(u16 bank);
void DecompressLZ16(const void *src, void *dst);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 offset, u32 size);
void *_ZN2G212GetBG0ScrPtrEv(void);
u32 _ZN3G2S13GetBG2CharPtrEv(void);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 offset, u32 size);
void *_ZN3G2S12GetBG2ScrPtrEv(void);
int func_0201a244(void *fn, int a, int b, int c, int d);
}

s32 BootScene::InitResources()
{
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX12SetBankForBGEt(1);
    _ZN2GX13SetBankForOBJEt(2);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x38000000) | 0x8000000;
    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    _ZN2GX6DispOnEv();

    *(volatile u32 *)0x4001000 |= 0x10000;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x1710;
    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~0x40;
    SetBg0Offset(0, 0);

    DecompressLZ16(&data_020918c4, func_02054efc());
    _ZN2GX10LoadBGPlttEPKvjj(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_020916d8, _ZN2G212GetBG0ScrPtrEv());

    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1100;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~3;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~0x40;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x18;
    SetSubBg0Offset(0, 0);

    *(volatile u16 *)0x400100a = *(volatile u16 *)0x400100a & ~3;
    *(volatile u16 *)0x400100a = *(volatile u16 *)0x400100a & ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x118;
    SetSubBg1Offset(0, 0);

    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~3;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x310;
    SetSubBg2Offset(0, 0);

    DecompressLZ16(&data_020918c4, (void *)_ZN3G2S13GetBG2CharPtrEv());
    _ZN3GXS10LoadBGPlttEPKvjj(&data_020914e0, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_02091570, _ZN3G2S12GetBG2ScrPtrEv());

    data_0209d45c = 1;
    data_0209d454 = 4;
    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x100;
    *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x400;

    mFadeTimer = 0x3c;
    mState = 0;
    mButtonFlashTimer = 0;
    mInputLockTimer = 0;
    *(int *)(&data_0208ee44) = 1;
    func_020233f4();

    *(u16 *)(&data_0209f5e8 + 0xc) = 0x7fff;
    _ZN5Sound6Play2DEjj(4, 0);
    data_0209f1e8 = (u8)func_0201a244((void *)func_0201a2f8, 0, 0xf, 0, 0x1000);
    return 1;
}
