typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_02019028(void);
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX12SetBankForBGEt(u16 v);
extern void _ZN2GX13SetBankForOBJEt(u16 v);
extern void _ZN2GX15SetBankForSubBGEt(u16 v);
extern void _ZN2GX16SetBankForSubOBJEt(u16 v);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern void _ZN2GX6DispOnEv(void);
extern void SetBg0Offset(int a, int b);
extern void SetBg1Offset(int a, int b);
extern void SetBg2Offset(int a, int b);
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern void LoadDebugFont(void);
extern void LoadFont(u8 a);
extern int GetOwnerLanguage(void);
extern int LoadFile(int handle);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN2G212GetBG0ScrPtrEv(void);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern u32 func_02054e88(void);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *p);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_0201cebc(int a);
extern void func_ov003_020b0730(void *self, int a);
extern void _ZN5Sound16LoadInitialGroupEi(int a);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int a);

extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;
extern u8 VS_TIME_UP;
extern int GAME_SPEED_RELATED;
extern short data_ov003_020b1764[];

int func_ov003_020b0b3c(void *arg)
{
    char *c = (char *)arg;
    int f;

    func_02019028();
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX12SetBankForBGEt(1);
    _ZN2GX13SetBankForOBJEt(0x40);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(3);

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x38000000) | 0x8000000;
    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & ~0x20e) | 0x20e;
    _ZN2GX6DispOnEv();

    *(volatile u32 *)0x4001000 |= 0x10000;

    *(volatile u16 *)0x4000008 &= ~3;
    *(volatile u16 *)0x4000008 &= ~0x40;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0xe10;
    SetBg0Offset(0, 0);

    *(volatile u16 *)0x400000a &= ~3;
    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1f1c;
    *(volatile u16 *)0x400000a &= ~0x40;
    SetBg1Offset(0, 0);

    LoadDebugFont();

    *(volatile u16 *)0x400000c &= ~3;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0xd10;
    SetBg2Offset(0, 0);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xb00c);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xac0c);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xa80c);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xa40c);
    } else {
        f = LoadFile(0xa00c);
    }
    DecompressLZ16(f, _ZN2G213GetBG2CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x231);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x232);
    DecompressLZ16(f, _ZN2G212GetBG2ScrPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x25c);
    DecompressLZ16(f, _ZN2G212GetBG0ScrPtrEv());
    Deallocate((void *)f);

    *(volatile u16 *)0x400000e &= ~3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1100;
    *(volatile u16 *)0x400000e &= ~0x40;
    LoadFont(0);

    f = LoadFile(0x233);
    DecompressLZ16(f, (void *)0x6400000);
    Deallocate((void *)f);

    f = LoadFile(0x234);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    TOP_SCREEN_RELATED = 0x14;

    *(volatile u16 *)0x4001008 &= ~3;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x1100;
    SetSubBg0Offset(0, 0);

    *(volatile u16 *)0x400100a &= ~3;
    *(volatile u16 *)0x400100a &= ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0xc00;
    SetSubBg1Offset(0, 0);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xb00d);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xac0d);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xa80d);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xa40d);
    } else {
        f = LoadFile(0xa00d);
    }
    DecompressLZ16(f, (void *)func_02054e88());
    Deallocate((void *)f);

    f = LoadFile(0x9807);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    f = LoadFile(0x9801);
    DecompressLZ16(f, _ZN3G2S12GetBG1ScrPtrEv());
    Deallocate((void *)f);

    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0xd00;
    SetSubBg2Offset(0, 0);

    f = LoadFile(0x228);
    DecompressLZ16(f, _ZN3G2S12GetBG2ScrPtrEv());
    Deallocate((void *)f);

    func_0201cebc(0x27d);
    func_ov003_020b0730(c, 1);

    *(u8 *)(c + 0x90) = 0;
    *(u8 *)(c + 0x91) = 0;
    *(u8 *)(c + 0x92) = 0;
    *(u8 *)(c + 0x93) = 0;
    *(u8 *)(c + 0x94) = 0;
    for (f = 0; f < 8; f++) {
        ((short *)(c + 0x50))[f] = data_ov003_020b1764[f];
        ((short *)(c + 0x60))[f] = 0x60;
        ((short *)(c + 0x70))[f] = 0x30;
        ((short *)(c + 0x80))[f] = 0;
    }

    BOTTOM_SCREEN_RELATED = 0x14;
    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | (TOP_SCREEN_RELATED << 8);
    *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x1400;
    VS_TIME_UP = 1;
    _ZN5Sound16LoadInitialGroupEi(2);
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x15);
    GAME_SPEED_RELATED = 1;
    return 1;
}
