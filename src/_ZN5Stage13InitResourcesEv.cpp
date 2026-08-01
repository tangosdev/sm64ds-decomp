//cpp
// NONMATCHING (TERMINAL-FLOOR): functionally-verified C at the proven compiler floor.
// Stage::InitResources @ 0x0202cc0c (arm9, size 0xa84). 4 words diverge: one arg-register
// phi coalesce (bank web colored r1 vs ROM r2 at +0x524, one shared merge copy). Identical
// on every owned build; pragmas, siblings, flags, launders, TU composition, goto-pin CFG
// all closed (notes 6av/6ay, DB row). Register-rename-only delta: functionally identical.
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Stage {
    int InitResources();
};

extern "C" {
extern u8 data_0209f2d8;
extern u8 data_0208a0e0;
extern u8 data_0209f21c;
extern u8 data_0209f250;
extern s8 data_02092110;
extern s8 data_0209f2c4;
extern s8 data_0209f2f8;
extern u8 data_0209f26c;
extern s32 data_0209f34c;
extern s32 data_0209f4f8[3][16];
extern u8 data_0209f30c[16];
extern u8 data_0209f310[16];
extern s16 data_0209f358[16];
extern u8 data_0209f2fc;
extern s8 data_02092124;
extern s8 data_02092118;
extern u8 data_0209f264;
extern u8 data_0209f268;
extern u8 data_0209f220;
extern u8 data_0209f1f0;
extern s8 data_02092120;
extern void *data_0209f314;
extern s32 data_0209f32c;
extern s8 data_0209f274;
extern s8 data_0209f2bc;
extern s16 data_0209f304;
extern s16 data_0209f308;
extern s32 data_0209f40c[12];
extern s32 data_0209f3e8[9];
extern s32 data_0209f3a4[8];
extern u8 data_0209f1f8;
extern u8 data_0209f2d0;
extern u8 data_0209f258;
extern u8 data_0209f2e8;
extern u8 data_0209f25c;
extern s32 data_0209f338;
extern s32 data_02092134;
extern u8 data_02075769[];
extern s8 data_0207576a[];
extern void *data_020756f0[12];
extern u16 data_02075600[7];
extern u16 data_020755f0[7];
extern u16 data_020755e0[7];
extern s32 data_0209f324;
extern s32 data_02092208[];
extern s32 data_0209f340;
extern s32 data_0208ee44;
extern s32 data_0209caa0[];
extern void *data_0209f394[];
extern u8 data_0209d454;
extern void *data_0209d4a8;
extern s32 data_0209f3c4;
extern u8 data_0209f204;
extern s32 data_0209b454;
extern void *data_020a0eac;
extern s32 data_0209fc48;
extern s32 data_0209fc68;
extern s16 data_0209f300;
extern u8 data_0209f284;
extern u8 data_0209f2d4;
extern u8 data_0209f20c;
extern u8 data_0209f290;
extern u8 data_0209d45c;
extern u8 data_0209f294;
extern s32 data_0209cee8;
extern s32 data_0209cef0;
extern u8 data_02092778;
extern s32 data_0209e650;
extern u8 data_0209f208;
extern s32 data_0209f344;
extern char data_02075720[][0xC];

void Enable3dEngines(void);
void ResetInput(void);
int SublevelToLevel(int i);
void UnloadArchive(int i);
void LoadTextNarcs(void);
int LoadArchive(int idx);
void Initialise3dGraphics(int arg);
void InitialiseVramGlobals(void);
void func_02039218(void);
void ResetKuppaScript(void);
int GetSoundGroupID(int level);
void LoadDebugFont(void);
int IsLevelInsideCastle(int level);
int IsLevelTinyHugeIslandOutside(int level);
int func_020308a8(void);
int func_0203da9c(void);
int func_0203da3c(void);
u32 func_0203dad4(void);
void func_0203b9b4(int *p, int v);
void *func_02073470(int a, int b, int c, void *ctor, void *dtor);
void func_ov001_020ab2e4(void);
void _ZN9FaderWipeC1Ev(void *thiz);
void _ZN9FaderWipeD1Ev(void *thiz);

void _ZN5Sound6Player19SetPlayableSeqCountEii(int a, int b);
void _ZN5Scene20Initialise3dGraphicsEv(void);
void *_ZN4Heap10SetDefaultEv(void *self);
void _ZN2GX15DisableAllBanksEv(void);
void _ZN5Stage12SetVramBanksEv(void);
void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
void _ZN3GXS15SetGraphicsModeEi(int a);
void _ZN2GX6DispOnEv(void);
void _Z17LoadLevelOverlaysi(int level);
void _ZN5Sound19LoadGroupAndSetBankEii(int a, int b);
void _ZN5Stage14LoadGraphics2DEbi(int b, int level);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9FaderWipe14LoadAndSetFileEt(int thiz, u16 fileID);
void _ZN5Stage9LoadModelEv(void *thiz);
void _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider(void *ov, u32 flags, void *mc);
void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int x);
int _ZN5Sound8SetMusicEjj(u32 a, u32 b);
void _ZN5Stage7LoadFogEv(void *thiz);
void _ZN5Stage23LoadTextureTransformersEv(void *thiz);
void _ZN5Stage10LoadSkyboxEv(void *thiz);
void _ZN8Particle10SysTracker10InitialiseEv(void *thiz);
void _ZN11ShadowModel8CleanAllEv(void);
}

int Stage::InitResources()
{
    Stage *thiz = this;

    if (*(s32*)((char*)thiz + 0x9c4) == 0) {
        u8 modeByte = data_0209f2d8;
        int v0 = (modeByte == 1) ? 1 : 0;
        if (v0) {
            _ZN5Sound6Player19SetPlayableSeqCountEii(2, 5);
            int n = 3;
            _ZN5Sound6Player19SetPlayableSeqCountEii(n, n);
        }
        _ZN5Scene20Initialise3dGraphicsEv();
        Enable3dEngines();
        data_0209f21c = data_0208a0e0;
        data_0209f250 = (u8)func_0203da9c();
        ResetInput();
        data_0209f2c4 = 0;

        int temp_r4 = SublevelToLevel(data_02092110);
        int temp_r0 = SublevelToLevel(data_0209f2f8);
        u32 bits = ((u8)data_02092110 + 0xDC) & 0xFF;
        int v1 = 0;
        if (bits <= 0xD) {
            if ((1 << bits) & 0x2A15)
                v1 = 1;
        }
        modeByte = data_0209f2d8;
        int b1 = (modeByte == 1) ? 1 : 0;

        if (b1 || data_0209f26c == 2 || v1 || temp_r4 == 0x1D || temp_r4 != temp_r0) {
            int r8 = 0;
            data_0209f34c = 0;
            do {
                int r7 = 0;
                do {
                    data_0209f4f8[r8][r7] = 0;
                    r7 += 1;
                } while (r7 < 0x10);
                r8 += 1;
            } while (r8 < 3);

            if ((s32)data_0209f21c > 0) {
                int count = data_0209f21c;
                int idx = 0;
                u8 *p1 = data_0209f30c;
                u8 *p2 = data_0209f310;
                int sl = (modeByte == 1) ? 1 : 0;
                u8 f26c = data_0209f26c;
                do {
                    if (sl || (v1 == 0 && f26c != 1)) {
                        data_0209f358[idx] = 0;
                    }
                    *p1 = 0;
                    *p2 = 0;
                    idx += 1;
                    p1 += 1;
                    p2 += 1;
                } while (idx < count);
            }
        }

        data_0209f2fc = data_0209f26c;
        if (data_0209f2fc == 1) {
            data_02092124 = data_0209f2f8;
            data_02092118 = -1;
        }

        s8 prevLevel = data_0209f2f8;
        data_0209f2f8 = data_02092110;
        data_0209f264 = data_0209f268;
        data_0209f220 = data_0209f1f0;
        data_02092120 = -1;
        data_0209f314 = (void*)((char*)thiz + 0x8bc);
        data_0209f32c = 0x80000000;
        data_0209f274 = 0;
        SublevelToLevel(data_0209f2f8);

        s32 archiveIdx = 0xBF;
        if (data_0209f2f8 == 0x33) archiveIdx = 2;
        else if (data_0209f2f8 == 0x2B) archiveIdx = 3;
        else if (data_0209f2f8 == 0x1D) archiveIdx = 4;
        else if (data_0209f2f8 == 0x2A) archiveIdx = 5;

        u32 r7_2 = 2;
        do {
            if (r7_2 != (u32)archiveIdx) {
                UnloadArchive(r7_2);
            }
            r7_2 += 1;
        } while (r7_2 <= 5);

        LoadTextNarcs();
        LoadArchive(0);
        if (data_0209f2f8 == 1) {
            void *saved = _ZN4Heap10SetDefaultEv(data_020a0eac);
            LoadArchive(7);
            _ZN4Heap10SetDefaultEv(saved);
        }
        if (archiveIdx != 0xBF) {
            LoadArchive(archiveIdx);
        }

        Initialise3dGraphics(0x1F);
        _ZN2GX15DisableAllBanksEv();
        _ZN5Stage12SetVramBanksEv();

        volatile u32 *p0 = (volatile u32*)0x04000000;
        volatile u32 *p1 = (volatile u32*)0x04001000;
        *p0 &= 0xFFCFFFEF;
        *p1 &= 0xFFCFFFEF;
        int one = 1;
        _ZN2GX15SetGraphicsModeEiii(one, 0, one);
        _ZN3GXS15SetGraphicsModeEi(3);
        *p0 = (*p0 & ~0x38000000) | 0x08000000;
        *(volatile u16*)0x04000304 = (*(volatile u16*)0x04000304 & 0xFFFFFDF1) | 0x20E;
        _ZN2GX6DispOnEv();
        *p1 |= 0x10000;
        InitialiseVramGlobals();

        volatile u16 *pVram = (volatile u16*)0x0400000A;
        *pVram = *pVram & ~3;
        *pVram = (*pVram & 0x43) | 0x1F1C;
        *pVram = *pVram & ~0x40;
        data_0209d454 = 0x18;
        data_0209f204 = 0;
        data_0209d4a8 = (void*)&data_0209f3c4;
        data_0209b454 = 0;
        func_02039218();
        ResetKuppaScript();

        int v0_3 = (data_0209f2d8 == 1) ? 1 : 0;
        if (v0_3 == 0) goto L_zero304;
        data_0209f2bc = 3;
        data_0209f304 = 0x28;
        data_0209f308 = 0;
        goto L_after304;
L_zero304:
        data_0209f2bc = 0;
        data_0209f304 = 0;
L_after304:
        ;

        {
            int i;
            for (i = 0; i < 0xC; i++) {
                data_0209f40c[i] = 0;
            }
        }
        {
            int i;
            for (i = 0; i < 9; i++) {
                data_0209f3e8[i] = 0;
            }
        }
        func_ov001_020ab2e4();
        {
            int i;
            for (i = 0; i < 8; i++) {
                data_0209f3a4[i] = 0;
            }
        }

        data_0209f1f8 = 0;
        data_0209f2d0 = 0;
        data_0209f258 = 0;
        data_0209f2e8 = 0;
        data_0209f25c = 0;
        data_0209f338 = 0;
        data_02092134 = 0x44444444;
        _Z17LoadLevelOverlaysi(data_0209f2f8);

        int v0_4 = (data_0209f2d8 == 2) ? 1 : 0;
        if (v0_4 == 0) {
            s8 level2 = *(s8*)(unsigned)(((unsigned long long)(unsigned)&data_0209f2f8) & ~0ULL);
            int soundGroup = 0;
            u8 bank = 0x36;
            if (data_0209f220 == 2) {
                if (level2 == 1 || level2 == 0x33) soundGroup = 0x2B;
                else if (level2 == 0x1D) soundGroup = 0x2E;
                else if (level2 == 0x2A) soundGroup = 0x2C;
                else if (level2 == 0x2B) soundGroup = 0x2D;
            }
            if (soundGroup == 0) {
                soundGroup = GetSoundGroupID(level2);
                bank = level2 * 3;
                bank = data_02075769[bank];
            }
            _ZN5Sound19LoadGroupAndSetBankEii(soundGroup, bank);
        }

        *(volatile u16*)0x04000008 = (*(volatile u16*)0x04000008 & ~3) | 2;
        int v0_6 = (data_0209f2d8 == 1) ? 1 : 0;
        _ZN5Stage14LoadGraphics2DEbi(v0_6, data_0209f2f8);

        {
            u32 i;
            for (i = 0; i < 0xC; i++) {
                _ZN5Model8LoadFileER13SharedFilePtr(data_020756f0[i]);
            }
        }

        int v0_7 = (data_0209f2d8 == 1) ? 1 : 0;
        u16 *faderTbl;
        if (v0_7 && archiveIdx != 0xBF) {
            faderTbl = data_02075600;
        } else if (data_0209f2f8 == 5) {
            faderTbl = data_020755f0;
        } else {
            faderTbl = data_020755e0;
        }

        data_0209f324 = (s32)func_02073470(7, 0x60, 8, (void*)&_ZN9FaderWipeC1Ev, (void*)&_ZN9FaderWipeD1Ev);
        int r8_4 = 0;
        int r7_3 = 0;
        do {
            _ZN9FaderWipe14LoadAndSetFileEt(data_0209f324 + r7_3, faderTbl[r8_4]);
            r8_4 += 1;
            r7_3 += 0x60;
        } while (r8_4 < 7);

        data_0209f340 = data_02092208[data_0209f2f8];
        _ZN5Stage9LoadModelEv(thiz);
        data_0208ee44 = 2;
        _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider((void*)data_0209f340, data_0209f264, (char*)thiz + 0x91c);

        int v0_8 = (data_0209f2d8 == 1) ? 1 : 0;
        if (v0_8 == 0) {
            int nofc = (data_0209fc48 != 0) ? 1 : 0;
            if (nofc == 0) {
                if (!IsLevelInsideCastle(data_0209f2f8) || !IsLevelInsideCastle(prevLevel)) {
                    if (!IsLevelTinyHugeIslandOutside(data_0209f2f8) || !IsLevelTinyHugeIslandOutside(prevLevel)) {
                        if (data_0209f2f8 != 2 || (data_0209caa0[2] & 0x200)) {
                            _ZN5Sound22LoadAndSetMusic_Layer1Ei(data_0207576a[data_0209f2f8 * 3]);
                            u8 sndIdx = data_0209f250;
                            if (((u8*)data_0209f394[sndIdx])[0x6ff] != 0) {
                                _ZN5Sound8SetMusicEjj(sndIdx, 0x33);
                            }
                        }
                    }
                }
            }
        }

        if (func_0203da3c() != 2 && archiveIdx != 0xBF) {
            UnloadArchive(archiveIdx);
        }
        LoadDebugFont();
        _ZN5Stage7LoadFogEv(thiz);
        _ZN5Stage23LoadTextureTransformersEv(thiz);
        _ZN5Stage10LoadSkyboxEv(thiz);
        data_0209cef0 = 1;
        _ZN8Particle10SysTracker10InitialiseEv((char*)thiz + 0x50);

        u8 f2fc = data_0209f2fc;
        data_0209f284 = 0;
        data_0209f290 = 0;
        data_0209f294 = 0;
        data_0209f300 = 0;
        if (f2fc != 1) {
            data_0209f2d4 = 0;
            data_0209f20c = 0;
        }
        data_0209d45c = 0x11;

        *p0 = (*p0 & ~0x1F00) | 0x1100;
        int v0_9 = (data_0209f2d8 == 2) ? 1 : 0;
        if (v0_9 == 0) {
            *p1 = (*p1 & ~0x1F00) | (data_0209d454 << 8);
        }
        data_0209cee8 = 0;
        _ZN11ShadowModel8CleanAllEv();
        data_02092110 = -1;

        if (data_0209fc68 != 0) {
            *(s32*)((char*)thiz + 0x9c4) = 1;
        }
    }

    if (*(s32*)((char*)thiz + 0x9c4) != 0) {
        if (func_020308a8() == 0)
            return -1;
    }

    data_02092778 = 0;
    func_0203b9b4(&data_0209e650, func_0203dad4());
    data_0209f208 = 0;
    u32 idx = func_0203dad4() % 6;
    data_0209f344 = (s32)&data_02075720[idx];

    return 1;
}
