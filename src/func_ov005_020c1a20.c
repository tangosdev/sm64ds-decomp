typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void LoadTextNarcs(void);
extern int LoadArchive(int idx);
extern void _ZN5Sound19LoadGroupAndSetBankEii(int a, int b);
extern void func_02019028(void);
extern void func_ov005_020bffc8(void *a0, int a1, int r2, int r3);
extern int func_ov005_020bfff4(void *a, int b, int c);
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX13SetBankForTexEt(u16 v);
extern void _ZN2GX12SetBankForBGEt(u16 v);
extern void _ZN2GX15SetBankForSubBGEt(u16 v);
extern void _ZN2GX16SetBankForSubOBJEt(u16 v);
extern void _ZN2GX13SetBankForOBJEt(u16 v);
extern void _ZN2GX17SetBankForTexPlttEt(u16 v);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern void _ZN2GX6DispOnEv(void);
extern void _ZN3G3X6SetFogEbiii(int a, int b, int c, int d);
extern void InitialiseVramGlobals(void);
extern void func_020233f4(void);
extern void SetBg2Offset(int a, int b);
extern int LoadFile(int handle);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void DecompressLZ16(int src, void *dst);
extern void Deallocate(void *p);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern int GetOwnerLanguage(void);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void SetSubBg2Offset(int a, int b);
extern void SetSubBg3Offset(int a, int b);
extern void *func_02054de8(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void SetSubBg0Offset(int a, int b);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void func_ov005_020c16e4(void *self);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(void *p);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int a);

extern u8 data_0209f5f8;
extern u32 SAVE_DATA[];
extern u8 data_0209b300;
extern u8 data_0209b304;
extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;
extern int data_0208a170;
extern u8 data_0209f1d8;
extern u8 data_0209b2fc;
extern int GAME_SPEED_RELATED;
extern char data_0209f61c;

typedef struct Entry {
    int pad0[3];
    int unkC;
    int unk10;
    int arr[5];
    int pad28[3];
} Entry;

extern Entry data_ov005_020c24d8[];

int func_ov005_020c1a20(void *arg0)
{
    char *c = (char *)arg0;
    int i, j;
    Entry *tbl;
    int i2;
    int flag;
    int j2, j3;
    Entry *tbl2;
    volatile u16 sp0;
    volatile u16 sp2;
    char *dst;
    int f;

    LoadTextNarcs();
    LoadArchive(0);
    data_0209f5f8 = 0;
    _ZN5Sound19LoadGroupAndSetBankEii(0x27, 5);
    func_02019028();

    if (!(SAVE_DATA[0x48 / 4] & 0x80000000)) {
        tbl = data_ov005_020c24d8;
        for (i = 0; i < 0x24; i++) {
            for (j = 0; j < 5; j++) {
                func_ov005_020bffc8(c, i, j, tbl->arr[j]);
            }
            tbl++;
        }
        SAVE_DATA[0x48 / 4] |= 0x80000000;
        data_0209b300 = 1;
    }

    tbl2 = data_ov005_020c24d8;
    for (i2 = 0; i2 < 0x24; i2++) {
        flag = 0;
        if (tbl2->unkC != 0) {
            if (tbl2->unk10 == 1) {
                if ((u32)func_ov005_020bfff4(c, i2, 0) > (u32)tbl2->unkC) {
                    flag = 1;
                }
            } else {
                for (j2 = 0; j2 < 5; j2++) {
                    if ((u32)func_ov005_020bfff4(c, i2, j2) > (u32)tbl2->unkC) {
                        flag = 1;
                    }
                }
            }
        }
        if (flag != 0) {
            for (j3 = 0; j3 < 5; j3++) {
                func_ov005_020bffc8(c, i2, j3, tbl2->arr[j3]);
            }
            data_0209b300 = 1;
        }
        tbl2++;
    }

    if (data_0209b304 == 1) {
        *(int *)(c + 0x50) = 0xb0;
    } else {
        *(int *)(c + 0x50) = 0;
    }
    *(int *)(c + 0x64) = 0;

    _ZN2GX15DisableAllBanksEv();
    *(volatile u16 *)0x4000304 |= 0x8000;
    _ZN2GX13SetBankForTexEt(1);
    _ZN2GX12SetBankForBGEt(2);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);
    _ZN2GX13SetBankForOBJEt(0x10);
    _ZN2GX17SetBankForTexPlttEt(0x20);

    TOP_SCREEN_RELATED &= ~0x1f;
    BOTTOM_SCREEN_RELATED &= ~0x1f;
    *(volatile u32 *)0x4000000 &= ~0x1f00;
    *(volatile u32 *)0x4001000 &= ~0x1f00;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    *(volatile u32 *)0x4000000 &= ~0x7000000;
    *(volatile u32 *)0x4000000 &= ~0x38000000;
    _ZN2GX6DispOnEv();
    *(volatile u32 *)0x4001000 |= 0x10000;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();
    *(volatile u32 *)0x40004cc = 0x7fff;
    *(volatile u32 *)0x40004cc = 0x40007fff;
    func_020233f4();

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x410;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    SetBg2Offset(0, 0);

    f = LoadFile(0x4e);
    DecompressLZ16(f, _ZN2G213GetBG2CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x58);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x200);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    f = LoadFile(0x59);
    DecompressLZ16(f, _ZN2G212GetBG2ScrPtrEv());
    Deallocate((void *)f);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xda);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xd7);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xd6);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xd5);
    } else {
        f = LoadFile(0xd8);
    }
    DecompressLZ16(f, (void *)0x6400000);
    Deallocate((void *)f);

    f = LoadFile(0xd9);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);
    Deallocate((void *)f);

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x404;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    SetSubBg2Offset(0, 0);

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x4604;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = *(int *)(c + 0x50) & 0x1ff;
    SetSubBg3Offset(*(int *)(c + 0x50), 0);

    f = LoadFile(0x53);
    DecompressLZ16(f, func_02054de8());
    Deallocate((void *)f);

    f = LoadFile(0x5a);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x200);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 2;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x6010;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = *(int *)(c + 0x50) & 0x1ff;
    SetSubBg0Offset(*(int *)(c + 0x50), 0);

    dst = (char *)_ZN3G2S13GetBG0CharPtrEv();
    sp0 = 0;
    MultiStore16(sp0, dst, 0x8000);
    dst = (char *)_ZN3G2S12GetBG0ScrPtrEv();
    sp2 = 0x1f;
    MultiStore16(sp2, dst, 0x1000);

    func_ov005_020c16e4(c);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xda);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xd7);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xd6);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xd5);
    } else {
        f = LoadFile(0xd8);
    }
    DecompressLZ16(f, (void *)0x6600000);
    Deallocate((void *)f);

    f = LoadFile(0xd9);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);
    Deallocate((void *)f);

    *(u8 *)(c + 0x54) = 0;
    *(int *)(c + 0x8c) = 0;
    *(int *)(c + 0x90) = 0;
    *(int *)(c + 0x94) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x5c) = 0;
    *(int *)(c + 0x60) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xa0) = 0;
    *(int *)(c + 0x58) = data_0208a170;
    *(u8 *)(c + 0xac) = 0;

    if (data_0209b300 == 1) {
        data_0209f1d8 = 1;
    }
    GAME_SPEED_RELATED = 1;
    if (data_0209b2fc != 0) {
        _ZN5Scene9SetFadersEP15FaderBrightness(&data_0209f61c);
    }

    data_0209b2fc = 0;
    TOP_SCREEN_RELATED |= 0x11;
    BOTTOM_SCREEN_RELATED |= 0x11;
    TOP_SCREEN_RELATED &= ~2;
    BOTTOM_SCREEN_RELATED &= ~2;
    TOP_SCREEN_RELATED |= 4;
    TOP_SCREEN_RELATED &= ~8;
    BOTTOM_SCREEN_RELATED |= 0xc;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(5);
    return 1;
}
