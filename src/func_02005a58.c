extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX12SetBankForBGEt(unsigned short b);
extern void _ZN2GX13SetBankForOBJEt(unsigned short b);
extern void _ZN2GX15SetBankForSubBGEt(unsigned short b);
extern void _ZN2GX16SetBankForSubOBJEt(unsigned short b);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern void _ZN2GX6DispOnEv(void);
extern void SetBg0Offset(int a, int b);
extern void* func_02054efc(void);
extern void DecompressLZ16(const void* src, void* dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
extern void* _ZN2G212GetBG0ScrPtrEv(void);
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern unsigned func_02054de8(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
extern void* _ZN3G2S12GetBG2ScrPtrEv(void);
extern void func_020233f4(void);
extern void _ZN5Sound6Play2DEjj(unsigned int a, unsigned int b);
extern int func_0201a244(void* fn, int a, int b, int c, int d);

extern char data_020918c4;
extern char data_020914e0;
extern char data_020916d8;
extern char data_02091570;
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern char data_0208ee44;
extern char data_0209f5e8;
extern void func_0201a2f8(void);
extern unsigned char data_0209f1e8;

typedef unsigned int u32;
typedef unsigned short u16;

int func_02005a58(char* c)
{
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX12SetBankForBGEt(1);
    _ZN2GX13SetBankForOBJEt(2);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);

    *(volatile u32*)0x4000000 &= 0xffcfffef;
    *(volatile u32*)0x4001000 &= 0xffcfffef;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32*)0x4000000 = (*(volatile u32*)0x4000000 & ~0x38000000) | 0x8000000;
    *(volatile u16*)0x4000304 = (*(volatile u16*)0x4000304 & 0xfffffdf1) | 0x20e;
    _ZN2GX6DispOnEv();

    *(volatile u32*)0x4001000 |= 0x10000;
    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & ~3) | 1;
    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & 0x43) | 0x1710;
    *(volatile u16*)0x4000008 = *(volatile u16*)0x4000008 & ~0x40;
    SetBg0Offset(0, 0);

    DecompressLZ16(&data_020918c4, func_02054efc());
    _ZN2GX10LoadBGPlttEPKvjj(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_020916d8, _ZN2G212GetBG0ScrPtrEv());

    *(volatile u16*)0x400000e = *(volatile u16*)0x400000e & ~3;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1100;
    *(volatile u16*)0x400000e = *(volatile u16*)0x400000e & ~0x40;
    *(volatile u16*)0x4001008 = *(volatile u16*)0x4001008 & ~3;
    *(volatile u16*)0x4001008 = *(volatile u16*)0x4001008 & ~0x40;
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & 0x43) | 0x18;
    SetSubBg0Offset(0, 0);

    *(volatile u16*)0x400100a = *(volatile u16*)0x400100a & ~3;
    *(volatile u16*)0x400100a = *(volatile u16*)0x400100a & ~0x40;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & 0x43) | 0x118;
    SetSubBg1Offset(0, 0);

    *(volatile u16*)0x400100c = *(volatile u16*)0x400100c & ~3;
    *(volatile u16*)0x400100c = *(volatile u16*)0x400100c & ~0x40;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0x310;
    SetSubBg2Offset(0, 0);

    DecompressLZ16(&data_020918c4, (void*)func_02054de8());
    _ZN3GXS10LoadBGPlttEPKvjj(&data_020914e0, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj(&data_020914e0, 0x1c0, 0x40);
    DecompressLZ16(&data_02091570, _ZN3G2S12GetBG2ScrPtrEv());

    data_0209d45c = 1;
    data_0209d454 = 4;
    *(volatile u32*)0x4000000 = (*(volatile u32*)0x4000000 & ~0x1f00) | 0x100;
    *(volatile u32*)0x4001000 = (*(volatile u32*)0x4001000 & ~0x1f00) | 0x400;

    *(unsigned short*)(c + 0x50) = 0x3c;
    *(unsigned char*)(c + 0x52) = 0;
    *(unsigned char*)(c + 0x54) = 0;
    *(unsigned char*)(c + 0x55) = 0;
    *(int*)(&data_0208ee44) = 1;
    func_020233f4();

    *(unsigned short*)(&data_0209f5e8 + 0xc) = 0x7fff;
    _ZN5Sound6Play2DEjj(4, 0);
    data_0209f1e8 = (unsigned char)func_0201a244((void*)func_0201a2f8, 0, 0xf, 0, 0x1000);
    return 1;
}
