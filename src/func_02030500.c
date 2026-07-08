typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

extern void func_02019028(void);
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX12SetBankForBGEt(u16 b);
extern void _ZN2GX13SetBankForOBJEt(u16 b);
extern void _ZN2GX15SetBankForSubBGEt(u16 b);
extern void _ZN2GX16SetBankForSubOBJEt(u16 b);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern s32 func_02030fa8(void);
extern void* func_02054ea8(void);
extern void* func_02054e88(void);
extern void DecompressLZ16(const void* src, void* dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void* _ZN2G212GetBG1ScrPtrEv(void);
extern void* _ZN3G2S12GetBG1ScrPtrEv(void);
extern void SetBg1Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);

extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;
extern void* data_020927b0[];
extern void* data_020927c4[];
extern void* data_020927d8[];
extern u8 data_0209440c;
extern u8 data_0209fc58;

void func_02030500(void)
{
    u32 tmp;
    void* p;

    TOP_SCREEN_RELATED = 0;
    BOTTOM_SCREEN_RELATED = 0;
    *(volatile u32*)0x4000000 &= ~0x1f00;
    *(volatile u32*)0x4001000 &= ~0x1f00;

    func_02019028();
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX12SetBankForBGEt(1);
    _ZN2GX13SetBankForOBJEt(2);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);

    tmp = *(volatile u32*)0x4000000 & ~0x7000000;
    *(volatile u32*)0x4000000 = tmp;
    tmp = *(volatile u32*)0x4000000 & ~0x38000000;
    *(volatile u32*)0x4000000 = tmp;
    *(volatile u16*)0x4000304 |= 0x8000;
    *(volatile u16*)0x4000050 = 0;
    *(volatile u16*)0x4001050 = 0;
    tmp = *(volatile u32*)0x4000000 & ~0xe000;
    *(volatile u32*)0x4000000 = tmp;
    *(volatile u32*)0x4001000 &= ~0xe000;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);

    *(volatile u32*)0x4000000 &= 0xffcfffef;
    *(volatile u16*)0x400000a = (*(volatile u16*)0x400000a & 0x43) | 8;
    *(volatile u16*)0x400000a &= ~0x40;
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32*)0x4001000 &= 0xffcfffef;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & 0x43) | 8;
    *(volatile u16*)0x400100a &= ~0x40;

    DecompressLZ16(data_020927b0[func_02030fa8()], func_02054ea8());
    DecompressLZ16(data_020927b0[func_02030fa8()], func_02054e88());

    _ZN2GX10LoadBGPlttEPKvjj(&data_0209440c, 0, 0x40);
    _ZN3GXS10LoadBGPlttEPKvjj(&data_0209440c, 0, 0x40);

    if (data_0209fc58 & 0x30) {
        p = data_020927d8[func_02030fa8()];
    } else {
        p = data_020927c4[func_02030fa8()];
    }
    DecompressLZ16(p, _ZN2G212GetBG1ScrPtrEv());
    DecompressLZ16(p, _ZN3G2S12GetBG1ScrPtrEv());

    *(volatile u16*)0x400000a &= ~3;
    *(volatile u16*)0x400100a &= ~3;
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    TOP_SCREEN_RELATED = 2;
    BOTTOM_SCREEN_RELATED = 2;
    *(volatile u32*)0x4000000 = (*(volatile u32*)0x4000000 & ~0x1f00) | 0x200;
    *(volatile u32*)0x4001000 = (*(volatile u32*)0x4001000 & ~0x1f00) | 0x200;
}
