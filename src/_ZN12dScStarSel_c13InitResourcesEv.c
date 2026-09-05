// @symbol _ZN12dScStarSel_c13InitResourcesEv
/* recovered: dScStarSel_c::InitResources() -- vtable slot 0. Plain C carries
 * the literal mangled name with no mangling needed -- see
 * include/dScStarSel_c.h.
 *
 * Loads the star-select screen: sound group, VRAM banks, the two models at
 * this+0x64 / this+0xB4, the per-language BG/OBJ graphics, the per-character
 * palette set, then derives the star row (collected bitmask at +0x131, first
 * uncollected slot at +0x115, count at +0x114, per-star x positions at
 * +0x11A) and the character row (+0x124/+0x128/+0x12C, count at +0x130).
 *
 * The star x positions are a u8 running value: the row is centred, so it
 * starts at 0x80 (odd count) or 0x92 (even count) minus half the count
 * times the 0x23 pitch, and each star adds one pitch. Written as one u8 with
 * if/else constant arms, which is what keeps the value in the register the
 * ternary lands in (r3) and lets the loop's +0x23 temp take a fresh r1. */
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef volatile unsigned short vu16;
typedef volatile unsigned int vu32;

#define REG16(a) (*(vu16 *)(a))
#define REG32(a) (*(vu32 *)(a))
#define FB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FH(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FW(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define LB(p) (*(u8 *)(u32)(u64)(u32)(p))

extern void LoadTextNarcs(void);
extern int LoadArchive(int idx);
extern void Enable3dEngines(void);
extern void _ZN5Sound16LoadInitialGroupEi(int);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
extern void func_0201277c(int);
extern void Initialise3dGraphics(int);
extern void _ZN3G3X13SetClearColorEtiiib(u16, int, int, int, int);
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX13SetBankForTexEt(u16);
extern void _ZN2GX17SetBankForTexPlttEt(u16);
extern void _ZN2GX12SetBankForBGEt(u16);
extern void _ZN2GX13SetBankForOBJEt(u16);
extern void _ZN2GX15SetBankForSubBGEt(u16);
extern void _ZN2GX16SetBankForSubOBJEt(u16);
extern void _ZN2GX23SetBankForSubOBJExtPlttEt(u16);
extern void _ZN2GX15SetGraphicsModeEiii(int, int, int);
extern void _ZN3GXS15SetGraphicsModeEi(int);
extern void _ZN2GX6DispOnEv(void);
extern void _ZN3G3X6SetFogEbiii(int, int, int, int);
extern void InitialiseVramGlobals(void);
extern void _ZN5Model14LoadAndSetFileEtii(void *, u16, int, int);
extern void LoadFont(u8);
extern int SublevelToLevel(int);
extern void *LoadFile(int);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void DecompressLZ16(void *, u32);
extern void Deallocate(void *);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *, u32, u32);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *, u32, u32);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void SetBg2Offset(int, int);
extern int GetOwnerLanguage(void);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *, u32, u32);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *, u32, u32);
extern void _ZN3GXS19BeginLoadOBJExtPlttEv(void);
extern void _ZN3GXS14LoadOBJExtPlttEPKvjj(const void *, u32, u32);
extern void _ZN3GXS17EndLoadOBJExtPlttEv(void);
extern u8 CountStarsCollectedInLevelToDisplay(s32);
extern int IsStarCollected(int, int);
extern void _ZN7Message30DisplayCourseNameForStarSelectEj(u32);
extern void _ZN7Message28DisplayStarNameForStarSelectEj(u32);
extern void SetBg3Offset(int, int);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32);

extern s8 data_02092110;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_02092128;
extern s32 data_0208ee44;
extern u16 data_ov003_020b1698;
extern u8 data_ov003_020b16a0[];
extern u8 data_ov003_020b16a4[];
extern u8 data_ov003_020b16a8[];
extern s8 data_ov003_020b16c8[];
extern s16 data_ov003_020b16e4[];
extern u8 data_0209caa0[0x50];

s32 _ZN12dScStarSel_c13InitResourcesEv(void *arg0)
{
    void *file;
    s32 loopI;
    u8 accB;
    s32 anyUnfilled;
    u8 bitPos;
    u32 totalStars;
    u8 level;
    s32 sp8;
    s32 spC;
    s32 varOne;
    u8 *bitsPtr;
    u8 tmpU8;
    u8 colorVal;
    s32 idx2;
    s32 charIdx;
    u8 charCount;
    s32 charCountM1;
    u8 tabSL;
    u8 tabSB;
    s32 varFp;
    s32 idx8;
    s8 *tablePtr;
    s32 varR6;
    u8 tabR5;

    LoadTextNarcs();
    LoadArchive(0);
    Enable3dEngines();
    _ZN5Sound16LoadInitialGroupEi(3);
    if (data_02092110 == 0x24 || data_02092110 == 0x26 || data_02092110 == 0x28) {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x24);
        func_0201277c(0xB8);
    } else {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x16);
    }
    REG16(0x4000304) |= 0x8000;
    Initialise3dGraphics(0);
    _ZN3G3X13SetClearColorEtiiib(0x7FFF, 0x1F, 0x7FFF, 0x3F, 0);
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX13SetBankForTexEt(3);
    _ZN2GX17SetBankForTexPlttEt(0x30);
    _ZN2GX12SetBankForBGEt(4);
    _ZN2GX13SetBankForOBJEt(0x40);
    _ZN2GX15SetBankForSubBGEt(0x80);
    _ZN2GX16SetBankForSubOBJEt(8);
    _ZN2GX23SetBankForSubOBJExtPlttEt(0x100);
    REG32(0x4000000) &= 0xFFCFFFEF;
    REG32(0x4001000) &= 0xFFCFFFEF;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    _ZN3GXS15SetGraphicsModeEi(0);
    REG32(0x4000000) = (REG32(0x4000000) & ~0x38000000) | 0x08000000;
    REG16(0x4000304) = (REG16(0x4000304) & 0xFFFFFDF1) | 0x20E;
    _ZN2GX6DispOnEv();
    REG32(0x4001000) |= 0x10000;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();
    _ZN5Model14LoadAndSetFileEtii((u8 *)arg0 + 0x64, 0x8015, 1, -1);
    _ZN5Model14LoadAndSetFileEtii((u8 *)arg0 + 0xB4, 0x8017, 1, -1);
    REG16(0x4000008) = (REG16(0x4000008) & ~3) | 2;
    data_0209d45c = 0x19;
    REG16(0x400000E) &= ~3;
    REG16(0x400000E) = (REG16(0x400000E) & 0x43) | 0x1700;
    REG16(0x400000E) &= ~0x40;
    LoadFont(0);
    if (SublevelToLevel(data_02092110) >= 0xF) {
        REG16(0x400000C) = (REG16(0x400000C) & ~3) | 1;
        REG16(0x400000C) = (REG16(0x400000C) & 0x43) | 0x1610;
        REG16(0x400000C) &= ~0x40;
        file = LoadFile(0x23F);
        DecompressLZ16(file, (u32)_ZN2G213GetBG2CharPtrEv());
        Deallocate(file);
        file = LoadFile(0x240);
        _ZN2GX10LoadBGPlttEPKvjj(file, 0, 0x1A0);
        _ZN3GXS10LoadBGPlttEPKvjj(file, 0, 2);
        Deallocate(file);
        file = LoadFile(0x241);
        DecompressLZ16(file, (u32)_ZN2G212GetBG2ScrPtrEv());
        Deallocate(file);
        SetBg2Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d45c &= ~1;
    } else {
        REG16(0x400000C) = (REG16(0x400000C) & ~3) | 1;
        REG16(0x400000C) = (REG16(0x400000C) & 0x43) | 0x1610;
        REG16(0x400000C) &= ~0x40;
        if (GetOwnerLanguage() == 5) {
            file = LoadFile(0x262);
        } else if (GetOwnerLanguage() == 4) {
            file = LoadFile(0x261);
        } else if (GetOwnerLanguage() == 3) {
            file = LoadFile(0x260);
        } else if (GetOwnerLanguage() == 2) {
            file = LoadFile(0x25F);
        } else {
            file = LoadFile(0x24A);
        }
        DecompressLZ16(file, (u32)_ZN2G213GetBG2CharPtrEv());
        Deallocate(file);
        file = LoadFile(0x24B);
        _ZN2GX10LoadBGPlttEPKvjj(file, 0, 0x1A0);
        Deallocate(file);
        file = LoadFile(0x24C);
        DecompressLZ16(file, (u32)_ZN2G212GetBG2ScrPtrEv());
        Deallocate(file);
        SetBg2Offset(0, -4);
        data_0209d45c |= 4;
        _ZN3GXS10LoadBGPlttEPKvjj(&data_ov003_020b1698, 0, 2);
    }
    if (GetOwnerLanguage() == 5) {
        file = LoadFile(0x270);
    } else if (GetOwnerLanguage() == 4) {
        file = LoadFile(0x26E);
    } else if (GetOwnerLanguage() == 3) {
        file = LoadFile(0x26C);
    } else if (GetOwnerLanguage() == 2) {
        file = LoadFile(0x26A);
    } else {
        file = LoadFile(0x269);
    }
    DecompressLZ16(file, 0x06400000);
    Deallocate(file);
    file = LoadFile(0x229);
    {
        char *a5 = (char *)0x06400000;
        a5 += 0x2000;
        DecompressLZ16(file, (u32)a5);
    }
    Deallocate(file);
    if (GetOwnerLanguage() == 5) {
        file = LoadFile(0xB00A);
    } else if (GetOwnerLanguage() == 4) {
        file = LoadFile(0xAC0A);
    } else if (GetOwnerLanguage() == 3) {
        file = LoadFile(0xA80A);
    } else if (GetOwnerLanguage() == 2) {
        file = LoadFile(0xA40A);
    } else {
        file = LoadFile(0xA00A);
    }
    _ZN2GX11LoadOBJPlttEPKvjj(file, 0, 0x120);
    Deallocate(file);
    file = LoadFile(0x22A);
    _ZN2GX11LoadOBJPlttEPKvjj(file, 0x120, 0xE0);
    Deallocate(file);
    if (GetOwnerLanguage() == 5) {
        file = LoadFile(0x266);
    } else if (GetOwnerLanguage() == 4) {
        file = LoadFile(0x265);
    } else if (GetOwnerLanguage() == 3) {
        file = LoadFile(0x264);
    } else if (GetOwnerLanguage() == 2) {
        file = LoadFile(0x263);
    } else {
        file = LoadFile(0x24D);
    }
    DecompressLZ16(file, 0x06600000);
    Deallocate(file);
    file = LoadFile(0x24E);
    _ZN3GXS11LoadOBJPlttEPKvjj(file, 0, 0x200);
    Deallocate(file);
    data_02092128 = data_0209caa0[0x41];
    switch (data_0209caa0[0x41]) {
    case 0:
        file = LoadFile(0x244);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(0x245);
        _ZN3GXS19BeginLoadOBJExtPlttEv();
        _ZN3GXS14LoadOBJExtPlttEPKvjj(file, 0, 0x200);
        _ZN3GXS17EndLoadOBJExtPlttEv();
        Deallocate(file);
        break;
    case 1:
        file = LoadFile(0x242);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(0x243);
        _ZN3GXS19BeginLoadOBJExtPlttEv();
        _ZN3GXS14LoadOBJExtPlttEPKvjj(file, 0, 0x200);
        _ZN3GXS17EndLoadOBJExtPlttEv();
        Deallocate(file);
        break;
    case 2:
        file = LoadFile(0x246);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(0x247);
        _ZN3GXS19BeginLoadOBJExtPlttEv();
        _ZN3GXS14LoadOBJExtPlttEPKvjj(file, 0, 0x200);
        _ZN3GXS17EndLoadOBJExtPlttEv();
        Deallocate(file);
        break;
    case 3:
        file = LoadFile(0x248);
        {
            char *a6 = (char *)0x06600000;
            a6 += 0x4000;
            DecompressLZ16(file, (u32)a6);
        }
        Deallocate(file);
        file = LoadFile(0x249);
        _ZN3GXS19BeginLoadOBJExtPlttEv();
        _ZN3GXS14LoadOBJExtPlttEPKvjj(file, 0, 0x200);
        _ZN3GXS17EndLoadOBJExtPlttEv();
        Deallocate(file);
        break;
    }

    level = (u8)SublevelToLevel(data_02092110);
    if ((u32)level <= 0xE) {
        accB = 0;
        anyUnfilled = 0;
        bitPos = 0;
        totalStars = CountStarsCollectedInLevelToDisplay(level);
        if (IsStarCollected(level, 0) != 0) {
            totalStars = (u32)(u8)(totalStars - 1);
        }
        FB(arg0, 0x115) = 0xFF;
        FB(arg0, 0x131) = 0;
        loopI = 1;
        if ((u32)accB < totalStars) {
            sp8 = 1;
            spC = 1;
            varOne = 1;
            bitsPtr = (u8 *)arg0;
            bitsPtr += 0x131;
            do {
                if (IsStarCollected(level, loopI) != 0) {
                    LB((u8 *)arg0 + 0x131) |= (u8)(varOne << bitPos);
                    accB += 1;
                } else {
                    if (FB(arg0, 0x115) == 0xFF) {
                        FB(arg0, 0x115) = bitPos;
                        *bitsPtr |= (u8)(sp8 << bitPos);
                    }
                    anyUnfilled = spC;
                }
                bitPos += 1;
                loopI += 1;
            } while ((u32)accB < totalStars);
        }
        FB(arg0, 0x114) = bitPos;
        if ((anyUnfilled == 0) && (totalStars != 7)) {
            LB((u8 *)arg0 + 0x114) += 1;
            if (FB(arg0, 0x115) == 0xFF) {
                FB(arg0, 0x115) = bitPos;
                LB((u8 *)arg0 + 0x131) |= (u8)(1 << bitPos);
            }
        }
        if (FB(arg0, 0x115) == 0xFF) {
            FB(arg0, 0x115) = 0;
        }
        tmpU8 = FB(arg0, 0x114);
        if (tmpU8 & 1) colorVal = 0x80; else colorVal = 0x92;
        colorVal -= ((s32)tmpU8 >> 1) * 0x23;
        for (idx2 = 0; idx2 < (s32)FB(arg0, 0x114); idx2++) {
            FB((u8 *)arg0 + idx2, 0x11A) = colorVal;
            colorVal += 0x23;
        }
    }

    if (SublevelToLevel(data_02092110) <= 0x14) {
        _ZN7Message30DisplayCourseNameForStarSelectEj((u32)(s16)SublevelToLevel(data_02092110));
        if (SublevelToLevel(data_02092110) <= 0xE) {
            _ZN7Message28DisplayStarNameForStarSelectEj(FB(arg0, 0x115));
        }
        SetBg3Offset(0, -4);
    } else {
        data_0209d45c &= ~8;
    }

    charIdx = 0;
    FB(arg0, 0x130) = 0;
    do {
        if (_ZN8SaveData19IsCharacterUnlockedEj(charIdx) != 0) {
            FB(arg0, 0x130)++;
        }
        charIdx += 1;
    } while (charIdx < 4);

    if (SublevelToLevel(data_02092110) <= 0xE) {
        if ((FB(arg0, 0x130) != 1) && (data_0209caa0[0x41] == 3)) {
            FB(arg0, 0x12B) = 0x88;
            FH(arg0, 0x108) = 0;
        } else {
            FB(arg0, 0x12B) = 0x60;
            FH(arg0, 0x108) = 0;
        }
        charCount = FB(arg0, 0x130);
        charCountM1 = charCount - 1;
        tabR5 = data_ov003_020b16a8[charCountM1];
        tabSL = data_ov003_020b16a0[charCountM1];
        if ((u32)charCount > 1U) {
            varFp = 0;
            varR6 = 0;
            tabSB = data_ov003_020b16a4[charCountM1];
            idx8 = (charCount - 2) * 4;
            if (charCountM1 > 0) {
                tablePtr = &data_ov003_020b16c8[idx8];
                do {
                    if (_ZN8SaveData19IsCharacterUnlockedEj(varR6) != 0) {
                        FB((u8 *)arg0 + varR6, 0x124) = tabR5;
                        FB((u8 *)arg0 + varR6, 0x128) = tabSL;
                        FB((u8 *)arg0 + varR6, 0x12C) = tabSB;
                        varFp += 1;
                        tabR5 = (u8)(tabR5 + data_ov003_020b16e4[idx8]);
                        tabSL = (u8)(tabSL + *tablePtr);
                        tabSB = (u8)(tabSB + 1);
                        tablePtr += 1;
                        idx8 += 1;
                    }
                    varR6 += 1;
                } while (varFp < (s32)(FB(arg0, 0x130) - 1));
            }
        }
    }

    FB(arg0, 0x132) = 0xFF;
    FB(arg0, 0x135) = 0;
    FB(arg0, 0x133) = 0;
    FW(arg0, 0x50) = 0x1000;
    FW(arg0, 0x54) = 0x1000;
    FW(arg0, 0x60) = 0x1000;
    FH(arg0, 0x110) = 0;
    FB(arg0, 0x139) = 0;
    FB(arg0, 0x137) = 0;
    FH(arg0, 0x110) = 0;
    FH(arg0, 0x108) = 0x78;
    FB(arg0, 0x13A) = 0;
    data_0209d454 = 0x10;
    REG32(0x4000000) = (REG32(0x4000000) & ~0x1F00) | (data_0209d45c << 8);
    REG32(0x4001000) = (REG32(0x4001000) & ~0x1F00) | 0x1000;
    FB(arg0, 0x116) = data_0209caa0[0x41];
    data_0208ee44 = 1;
    return 1;
}
