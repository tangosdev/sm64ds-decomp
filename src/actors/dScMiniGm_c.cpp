//cpp
/**
 * Minigame-select scene.
 *
 * Four character tabs of nine icons on the top screen; the sub screen
 * scrolls by mSubBgScrollX onto the second page. InitResources loads
 * the menu art and seeds unlock state from the minigame table.
 * Behavior runs page and exit timers plus the tap handlers; Render
 * draws tabs, new-game marks, page arrows and scores.
 *
 * dScMiniGm_c_classInit is reconstructed (RTTI dScMiniGm_c, MINIGAME
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: helpers stay func_ov005_* offset soup. This TU's Behavior
 *   and Render call them. Naming belongs with recovered members, not
 *   coined here.
 * Leftover: func_02013568 / func_02013580 / func_02013598 /
 *   func_020135d0 / func_02013638 are arm9 save-table helpers. This
 *   TU's InitResources, func_ov005_020c00b4, func_ov005_020c007c,
 *   func_ov005_020c0030, func_ov005_020bfff4, func_ov005_020bffc8
 *   call them; func_ov005_020c0010 reads the table's save-slot byte
 *   and hands it over. Naming belongs in arm9.
 * Leftover: func_02012790 is still the linker name of Sound::Play2D(2,
 *   id). This TU's Behavior and tap handlers (func_ov005_020c0378 /
 *   func_ov005_020c06cc / func_ov005_020c0250 / func_ov005_020c0878)
 *   call it. Naming belongs in arm9.
 * Leftover: func_0202ec9c is the brightness-fader start helper. This
 *   TU's func_ov005_020c0378 hands it data_0209f61c.
 * Leftover: func_02019028 / func_020233f4 are arm9 graphics-setup
 *   tails. This TU's InitResources calls them.
 * Leftover: data_ov005_020c2250 group-base table,
 *   data_ov005_020c2260 / data_ov005_020c2310 / data_ov005_020c23a0
 *   file-id tables, data_ov005_020c2280 / data_ov005_020c22c8 icon
 *   positions, data_ov005_020c2464 screen-offset table,
 *   data_ov005_020c24d8 minigame entries, data_ov005_020c2c28 /
 *   data_ov005_020c2ea4 / data_ov005_020c2e88 / data_ov005_020c2efc /
 *   data_ov005_020c2f4c / data_ov005_020c2f60 / data_ov005_020c2f88 /
 *   data_ov005_020c2fcc OAM. Overlay .rodata/.data this TU does not
 *   own.
 * Leftover: g_profile_MINIGAME lives outside this TU (S14).
 * Leftover: OAM::Render Fix12-by-value stays mangled (OAM.h, wall 6az).
 *   This TU's Render.
 * Leftover: GX / G2 / G2S / GXS / G3X / CP15 stay mangled. No recovered
 *   members in headers this TU includes. InitResources /
 *   func_ov005_020c16e4 / func_ov005_020c1688 are the callers.
 * Leftover: Sound::LoadGroupAndSetBank / LoadAndSetMusic_Layer1 and
 *   dScene_c::SetFaders / SetAndStopColorFader stay
 *   mangled -- decl_common.h / decl_Scene.h already declare the
 *   extern "C" names this TU includes.
 * Leftover: TouchOwner is a local overlay of data_0209f5bc. Slot 5 is
 *   IsActive. func_ov005_020c0378 / func_ov005_020bff4c call the named
 *   method; the other tap helpers still dispatch vt[5] by hand.
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2).
 */

#include "dScMiniGm_c.h"
#include "types.h"
#include "Sound.h"
#include "OAM.h"
#include "decl_Scene.h"
#include "decl_common.h"


struct MinigameSaveData;
struct OamAttr;

/* data_0209f5bc is the stylus owner. Every member that touches it calls the
 * same vtable slot 5 predicate before doing anything, three of them by hand
 * through `(*(int(***)(...))g)[0x14/4]`. */
struct TouchOwner {
    virtual int v0(); virtual int v1(); virtual int v2();
    virtual int v3(); virtual int v4(); virtual int IsActive();
};
struct TouchRec { u8 pressed; u8 held; u8 x; u8 y; };
struct MgMid { s32 v[5]; };
struct MgEntry {
    u16 id; u16 pad;
    s32 unk4, unk8, unkc, unk10;
    MgMid mid;
    s32 unk28, unk2c, unk30;
};
typedef char MgEntry_size_must_be_0x34[sizeof(MgEntry) == 0x34 ? 1 : -1];
typedef struct Entry {
    int pad0;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    int arr[5];
    int unk28;
    int unk2c;
    int unk30;
} Entry;
typedef struct Pair { int x, y; } Pair;
struct Off9 { int v[9]; };

namespace SaveData { void SaveMinigames(MinigameSaveData *); }

extern "C" {

/* ---- arm9 globals ---- */
extern u8 data_0209b300;
extern u8 data_0209b304;
extern MgEntry data_0209b308;
extern MgMid data_0209b31c;
extern MinigameSaveData data_0209cae4;
extern u32 data_0209caa0[];
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern short data_0209d474[];
extern short data_0209d494[];
extern int data_0209e650;
extern TouchOwner *data_0209f5bc;
extern u8 data_0209f5f8;
extern u8 data_0209f61c;
extern int data_0208ee44;
extern TouchRec data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern u8 data_020a0e40;
extern unsigned short data_020a0e5a[];

/* ---- ov005 statics ---- */
extern int data_ov005_020c2260[];
extern Pair data_ov005_020c2280[];
extern Pair data_ov005_020c22c8[];
extern u32 data_ov005_020c2310[];
extern int data_ov005_020c23a0[];
extern struct Off9 data_ov005_020c2464; /* 9-entry screen-offset table, not a matrix */
extern const MgEntry data_ov005_020c24d8[];
extern int data_ov005_020c24dc[];
extern int data_ov005_020c2efc[];
extern void *data_ov005_020c2f4c[];

/* ---- this TU's own members not already declared in decl_common.h ----
 * The file is written in REVERSE ROM order, so every intra-TU call is a
 * forward reference. */
int  func_ov005_020c0010(void *self, int i);
void func_ov005_020c1030(void *self, int x, int y, int val);
void func_ov005_020c1688(char *self, int n);

/* ---- arm9 / cross-overlay externals ---- */
int  func_02012790(int);
void func_02013568(int a, int b, int v);
int  func_02013580(int a, int b);
void func_02013598(int x);
int  func_020135d0(int r0);
int  func_02013638(int n);
int  RandomIntInternal(int *seed);
extern char *LoadFile(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, OamAttr *attr, int x, int y, int pal, int prio, int sx, int sy, int c, int d);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, OamAttr *attr, int x, int y, int pal, int prio, int s, int c);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX12SetBankForBGEt(u16 v);
extern void _ZN2GX13SetBankForOBJEt(u16 v);
extern void _ZN2GX15SetBankForSubBGEt(u16 v);
extern void _ZN2GX16SetBankForSubOBJEt(u16 v);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
}

// @symbol dScMiniGm_c_classInit
extern "C" dScMiniGm_c *dScMiniGm_c_classInit()
{
    return new dScMiniGm_c();
}

// @symbol _ZN11dScMiniGm_c13InitResourcesEv
s32 dScMiniGm_c::InitResources()
{
    int i, j;
    Entry *tbl;
    int i2;
    int flag;
    int j2, j3;
    Entry *tbl2;
    volatile u16 sp0;
    volatile u16 sp2;
    char *dst;
    char *f;

    LoadTextNarcs();
    LoadArchive(0);
    data_0209f5f8 = 0;
    _ZN5Sound19LoadGroupAndSetBankEii(0x27, 5);
    func_02019028();

    if (!(data_0209caa0[0x48 / 4] & 0x80000000)) {
        tbl = (Entry *)data_ov005_020c24d8;
        for (i = 0; i < 0x24; i++) {
            for (j = 0; j < 5; j++) {
                func_ov005_020bffc8((char *)this, i, j, tbl->arr[j]);
            }
            tbl++;
        }
        data_0209caa0[0x48 / 4] |= 0x80000000;
        data_0209b300 = 1;
    }

    tbl2 = (Entry *)data_ov005_020c24d8;
    for (i2 = 0; i2 < 0x24; i2++) {
        flag = 0;
        if (tbl2->unkC != 0) {
            if (tbl2->unk10 == 1) {
                if ((u32)func_ov005_020bfff4((char *)this, i2, 0) > (u32)tbl2->unkC) {
                    flag = 1;
                }
            } else {
                for (j2 = 0; j2 < 5; j2++) {
                    if ((u32)func_ov005_020bfff4((char *)this, i2, j2) > (u32)tbl2->unkC) {
                        flag = 1;
                    }
                }
            }
        }
        if (flag != 0) {
            for (j3 = 0; j3 < 5; j3++) {
                func_ov005_020bffc8((char *)this, i2, j3, tbl2->arr[j3]);
            }
            data_0209b300 = 1;
        }
        tbl2++;
    }

    if (data_0209b304 == 1) {
        mSubBgScrollX = 0xb0;
    } else {
        mSubBgScrollX = 0;
    }
    mGfxSlot = 0;

    _ZN2GX15DisableAllBanksEv();
    *(volatile u16 *)0x4000304 |= 0x8000;
    _ZN2GX13SetBankForTexEt(1);
    _ZN2GX12SetBankForBGEt(2);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);
    _ZN2GX13SetBankForOBJEt(0x10);
    _ZN2GX17SetBankForTexPlttEt(0x20);

    data_0209d45c &= ~0x1f;
    data_0209d454 &= ~0x1f;
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
    Deallocate(f);

    f = LoadFile(0x58);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x200);
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
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x100);
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
    *(volatile u32 *)0x400101c = mSubBgScrollX & 0x1ff;
    SetSubBg3Offset(mSubBgScrollX, 0);

    f = LoadFile(0x53);
    DecompressLZ16(f, _ZN3G2S13GetBG2CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x5a);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x200);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 2;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x6010;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = mSubBgScrollX & 0x1ff;
    SetSubBg0Offset(mSubBgScrollX, 0);

    dst = (char *)_ZN3G2S13GetBG0CharPtrEv();
    sp0 = 0;
    MultiStore16(sp0, dst, 0x8000);
    dst = (char *)_ZN3G2S12GetBG0ScrPtrEv();
    sp2 = 0x1f;
    MultiStore16(sp2, dst, 0x1000);

    func_ov005_020c16e4((char *)this);

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
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);
    Deallocate((void *)f);

    mPageFlipped = 0;
    mArrowBobPhase = 0;
    mPrevPageTimer = 0;
    mNextPageTimer = 0;
    mExitTimer = 0;
    mIconBlinkPhase = 0;
    unk_05c = 0;
    unk_060 = 0;
    unk_0a4 = 0;
    unk_0a8 = 0;
    mScrollDelay = 0;
    mGroupBase = data_0208a170;
    mExiting = 0;

    if (data_0209b300 == 1) {
        data_0209f1d8 = 1;
    }
    data_0208ee44 = 1;
    if (data_0209b2fc != 0) {
        _ZN8dScene_c9SetFadersEP15FaderBrightness(&data_0209f61c);
    }

    data_0209b2fc = 0;
    data_0209d45c |= 0x11;
    data_0209d454 |= 0x11;
    data_0209d45c &= ~2;
    data_0209d454 &= ~2;
    data_0209d45c |= 4;
    data_0209d45c &= ~8;
    data_0209d454 |= 0xc;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(5);
    return 1;
}

// @symbol func_ov005_020c16e4
extern "C" {
void func_ov005_020c16e4(void *selfv)
{
    char *self = (char *)selfv;
    int i;
    int rowB;
    int j;
    int n1;
    int n2;
    int m;
    int copyLen;
    int flushSize;
    int plttSize;
    int pInit;
    int qInit;
    int resetVal;
    volatile u16 valA; /* pin MultiStore16 src */
    volatile u16 valB; /* pin MultiStore16 src */
    struct Off9 tbl;
    char *f;
    int fill;
    int nbytes;
    int t;

    f = LoadFile(data_ov005_020c2260[data_0208a170]);
    DecompressLZ16(f, _ZN3G2S12GetBG2ScrPtrEv());
    Deallocate(f);

    f = LoadFile(data_ov005_020c2260[data_0208a170 + 4]);
    DecompressLZ16(f, _ZN3G2S12GetBG3ScrPtrEv());
    Deallocate(f);

    i = 0;
    j = 0;
    n2 = 0;
    qInit = 0;
    n1 = 0;
    m = 0;
    pInit = 0;
    resetVal = 0;
    copyLen = 0xc0;
    flushSize = 0x200;
    t = 0x20;
    fill = 0x1f;
    nbytes = 2;
    plttSize = t;

    do {
        tbl = data_ov005_020c2464;
        if (func_ov005_020c00b4(self, data_0208a170 + j) == 0) {
            int off;
            int row;
            int col;
            int destOff;
            char *scr;

            off = tbl.v[i];
            row = n1;
            do {
                col = n2;
                destOff = off << 1;
                do {
                    char *dstp;
                    scr = (char *)_ZN3G2S12GetBG0ScrPtrEv();
                    dstp = scr + destOff;
                    dstp = destOff ? dstp : dstp; /* register-dest pin; arms identical */
                    valA = (u16)fill;
                    MultiStore16(valA, dstp, nbytes);
                    destOff += 2;
                    off += 1;
                    col += 1;
                } while (col < 6);
                off += 0x1a;
                row += 1;
            } while (row < 9);
        } else {
            int row;
            int destOff;
            int idx;
            char *ch;
            char *scr;
            int off;
            int q;
            int p;

            f = LoadFile(data_ov005_020c23a0[data_0208a170 + j]);
            DecompressLZ16(f, (char *)_ZN3G2S13GetBG0CharPtrEv() + 0x8000);
            Deallocate(f);

            row = m;
            destOff = row;
            do {
                idx = *(int *)(self + 0x64);
                ch = (char *)_ZN3G2S13GetBG0CharPtrEv();
                scr = ch + (idx / 4) * 0x2400 + (idx % 4) * 0xc0 + destOff;
                ch = (char *)_ZN3G2S13GetBG0CharPtrEv();
                MultiCopy_Int((int *)(ch + 0x8000 + destOff), (int *)scr, copyLen);
                destOff += 0x400;
            } while (++row < 9);

            f = LoadFile(data_ov005_020c2310[data_0208a170 + j]);
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, flushSize);
            _ZN3GXS10LoadBGPlttEPKvjj(f, *(int *)(self + 0x64) << 5, plttSize);
            Deallocate(f);

            ((int *)(self + 0x68))[i] = *(int *)(self + 0x64);

            off = tbl.v[i];
            rowB = pInit;
            p = pInit;
            do {
                q = qInit;
                destOff = off << 1;
                do {
                    char *dstp;
                    idx = *(int *)(self + 0x64);
                    scr = (char *)_ZN3G2S12GetBG0ScrPtrEv();
                    dstp = scr + destOff;
                    dstp = idx ? dstp : dstp; /* register-dest pin; arms identical */
                    valB = (u16)(q + (p + ((idx % 4) * 6 + ((idx / 4) * 0x120 + (idx << 12)))));
                    MultiStore16(valB, dstp, nbytes);
                    destOff += 2;
                    off += 1;
                    q += 1;
                } while (q < 6);
                off += 0x1a;
                p += 0x20;
                rowB += 1;
            } while (rowB < 9);

            {
                int *slot = (int *)(self + 0x64);
                *slot = *slot + 1;
                if (*(int *)(self + 0x64) >= 10)
                    *(int *)(self + 0x64) = resetVal;
            }
        }
        j += 4;
        i += 1;
    } while (i < 9);
}
}

// @symbol func_ov005_020c1688
extern "C" {
void func_ov005_020c1688(char *self, int index)
{
    char *file = LoadFile(data_ov005_020c2310[index]);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)file, 0x200);
    u32 slot = *(u32 *)(self + (index / 4) * 4 + 0x68);
    _ZN3GXS10LoadBGPlttEPKvjj(file + 0x20, slot << 5, 0x20);
    Deallocate(file);
}
}

// @symbol _ZN11dScMiniGm_c16CleanupResourcesEv
s32 dScMiniGm_c::CleanupResources()
{
    if (data_0208a174[0] >= 0) {
        func_ov005_020c0030(this, data_0208a174[0]);
    }
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

// @symbol _ZN11dScMiniGm_c8BehaviorEv
s32 dScMiniGm_c::Behavior()
{
    if ((data_020a0e5a[data_020a0e40 << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (data_0209b300 == 1) {
        func_ov005_020bff4c((char *)this);
        return 1;
    }
    RandomIntInternal(&data_0209d4b8);
    RandomIntInternal(&data_0209e650);

    if (mPrevPageTimer > 0) {
        mPrevPageTimer -= 1;
        if (mPrevPageTimer == 0) {
            data_0209b304 = 0;
            mPageFlipped = 1;
        }
    } else if (mNextPageTimer > 0) {
        mNextPageTimer -= 1;
        if (mNextPageTimer == 0) {
            data_0209b304 = 1;
            mPageFlipped = 1;
        }
    } else if (mExitTimer > 1) {
        mExitTimer -= 1;
        if (mExitTimer == 1) {
            _ZN8dScene_c20SetAndStopColorFaderEv();
            ExitMinigameMenu();
            Sound::StopLoadedMusic_Layer1(0x1e);
            mExiting = 1;
            return 1;
        }
    } else {
        mArrowBobPhase += 1;
        if (mArrowBobPhase >= 0x40) mArrowBobPhase = 0;
        mIconBlinkPhase += 1;
        if (mIconBlinkPhase >= 0x40) mIconBlinkPhase = 0;
    }
    func_ov005_020c0878((char *)this);
    func_ov005_020c06cc((char *)this);
    func_ov005_020c0378((char *)this);
    func_ov005_020c0250((char *)this);
    func_ov005_020c0140((char *)this);
    return 1;
}

// @symbol func_ov005_020c1130
extern "C" {
void func_ov005_020c1130(void *selfv)
{
    char *self = (char *)selfv;
    int end;
    int i;
    int r8;
    int zero;
    int neg1;
    int type;
    int x, y;
    int val;
    unsigned int u;
    unsigned int major;
    unsigned int rem;
    unsigned int pct;

    if (*(u8 *)(self + 0x54) == 1)
        return;

    if (data_0209b304 == 1) {
        end = 8;
        i = 6;
    } else {
        end = 5;
        i = 0;
    }
    if (i > end)
        return;

    zero = 0;
    neg1 = -1;
    r8 = i * 4;
    do {
        if (func_ov005_020c00b4(self, data_0208a170 + r8) != 0) {
            type = ((Entry *)data_ov005_020c24d8)[data_0208a170 + r8].unk8;
            if (type != 0) {
                if (type == 1) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    OAM::RenderSub((OamAttr *)data_ov005_020c2f4c[0], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    func_ov005_020c1030(self, x - 4, y, val);
                } else if (type == 2) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    OAM::RenderSub((OamAttr *)data_ov005_020c2f4c[1], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    func_ov005_020c1030(self, x - 4, y, val);
                } else if (type == 4) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    OAM::RenderSub((OamAttr *)data_ov005_020c2f4c[2], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    func_ov005_020c1030(self, x - 4, y, val);
                } else if (type == 3) {
                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    x = data_ov005_020c2280[i].x;
                    y = data_ov005_020c2280[i].y;
                    func_ov005_020c1030(self, x - 0x14, y, val);
                } else if (type == 5) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    major = u / 60u;
                    func_ov005_020c1030(self, x - 0x10, y, (int)major);
                    OAM::RenderSub((OamAttr *)data_ov005_020c2f4c[4], x, y - 8, neg1, zero);

                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u;
                    func_ov005_020c1030(self, x + 8, y, (int)(pct / 600u));

                    val = func_ov005_020bfff4(self, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u / 60u;
                    func_ov005_020c1030(self, x + 0x10, y, (int)(pct % 10u));
                }
            }
        }
        r8 += 4;
        i += 1;
    } while (i <= end);
}
}

// @symbol func_ov005_020c1030
extern "C" {

void func_ov005_020c1030(void *a0, int x, int y, int val){
    if(val==0){
        OAM::RenderSub((OamAttr *)data_ov005_020c2efc[0], x, y, -1, 0);
        return;
    }
    int n=1;
    {
        int q=val;
        while(q>=10){
            q=q/10;
            n++;
        }
    }
    int xx=(n-1)*8;
    if(val<=0) return;
    do{
        int dig=val%10;
        OAM::RenderSub((OamAttr *)data_ov005_020c2efc[dig], x+xx, y, -1, 0);
        val=val/10;
        xx-=8;
    }while(val>0);
}
}

// @symbol func_ov005_020c0f38
extern "C" {
void func_ov005_020c0f38(void *selfv)
{
    char *self = (char *)selfv;
    int g = (*(int *)(self + 0x9c) >= 0x20) ? 1 : 0;
    int start, end;
    if (data_0209b304 == 1) {
        start = 6;
        end = 8;
    } else {
        start = 0;
        end = 5;
    }
    if (*(unsigned char *)(self + 0x54) == 1)
        return;
    int i;
    for (i = start; i <= end; i++) {
        if (func_ov005_020c00b4(self, data_0208a170 + i * 4) == 0)
            continue;
        if (func_ov005_020c007c(self, data_0208a170 + i * 4) == 0)
            continue;
        OAM::RenderSub((OamAttr *)data_ov005_020c2f60[g],
            data_ov005_020c22c8[i].x, data_ov005_020c22c8[i].y + 0x30, -1, 0);
    }
}
}

// @symbol _ZN11dScMiniGm_c6RenderEv
s32 dScMiniGm_c::Render()
{
    int i;
    int blink;
    int y1;
    int y0;
    int t;
    int scale;

    if (data_0209b300 == 1) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr *)data_ov005_020c2f88, 0x80, 0xa4, -1, 0, 0x1000, 0x1000, 0, -1);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr *)data_ov005_020c2fcc, 0x80, 0xa4, -1, 0, 0x1000, 0x1000, 0, -1);
    }

    y0 = 0x18;
    y1 = 0x28;
    for (i = 0; i < 4; i++) {
        int k = i;
        int j;
        if (mIconBlinkPhase >= 0x20)
            blink = 1;
        else
            blink = 0;
        for (j = 0; j < 9; j++) {
            if (func_ov005_020c00b4(this, data_ov005_020c2250[i] + j * 4) != 0 && func_ov005_020c007c(this, j * 4 + data_ov005_020c2250[i]) != 0) {
                OAM::RenderSub((OamAttr *)data_ov005_020c2f60[blink], 0x18, y1, -1, 0);
                break;
            }
        }
        if (mGroupBase != data_ov005_020c2250[i])
            k += 4;
        OAM::RenderSub((OamAttr *)data_ov005_020c2c28[k], 0x18, y0, -1, 0);
        y1 += 0x30;
        y0 += 0x30;
    }

    if (mPageFlipped == 0) {
        if (data_0209b304 != 0) {
            t = mPrevPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (OamAttr *)data_ov005_020c2ea4[0], 0x50 - mArrowBobPhase / 2, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (OamAttr *)data_ov005_020c2ea4[0], 0x50 - (0x40 - mArrowBobPhase) / 2, 0x60, -1, 0, scale, 0);
        } else if (func_ov005_020c00e4(this) != 0) {
            t = mNextPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (OamAttr *)data_ov005_020c2ea4[1], mArrowBobPhase / 2 + 0xe0, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (OamAttr *)data_ov005_020c2ea4[1], (0x40 - mArrowBobPhase) / 2 + 0xe0, 0x60, -1, 0, scale, 0);
        }
    }

    func_ov005_020c1130(this);
    func_ov005_020c0f38(this);

    t = mExitTimer;
    if (t < 0x14)
        scale = 0x1000;
    else if (t < 0x18)
        scale = (((t - 0x14) << 12) / 16) + 0x1000;
    else
        scale = (((0x1c - t) << 12) / 16) + 0x1000;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (OamAttr *)data_ov005_020c2e88, 0xe8, 0xa0, -1, 0, scale, 0);
    return 1;
}

// @symbol _ZN11dScMiniGm_c16OnPendingDestroyEv
void dScMiniGm_c::OnPendingDestroy()
{
}

// @symbol func_ov005_020c0878
extern "C" {

void func_ov005_020c0878(char *self)
{
    void* g = (void*)data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0)
        return;
    if (*(unsigned char*)(self + 0xac) != 0)
        return;
    if (*(unsigned char*)(self + 0x54) == 1)
        return;
    if (*(int*)(self + 0x90) > 0)
        return;
    if (*(int*)(self + 0x94) > 0)
        return;
    if (*(int*)(self + 0x98) > 0)
        return;

    if (*(int*)(self + 0xa0) > 0) {
        int* pa0 = (int*)((int)self + 0xa0);
        *pa0 = *pa0 - 1;
        if (*(int*)(self + 0xa0) == 0) {
            int f58 = *(int*)(self + 0x58);
            if (f58 != data_0208a170) {
                data_0208a170 = f58;
                func_ov005_020c16e4(self);
                *(int*)(self + 0xa0) = 0x1e;
            }
        }
    }

    {
        int flag = 0;
        unsigned char idx = data_020a0e40;
        unsigned char v = ((unsigned char*)data_020a0de8)[idx * 4];
        if (v != 0) {
            if (data_020a0de9[idx * 4] != 0)
                flag = 1;
        }
        if (flag != 0 || (*(int*)(self + 0xa0) <= 0 && v != 0)) {
            int i = 0;
            int lo = 2;
            int hi = 0x2e;
            int z = 0;
            int t = 0x1e;
            do {
                int e = data_ov005_020c2250[i];
                if (data_0208a170 != e) {
                    unsigned char ix = data_020a0e40;
                    unsigned char* p = (unsigned char*)data_020a0de8 + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        data_0208a170 = e;
                        *(int*)(self + 0x58) = e;
                        func_02012790(z);
                        func_ov005_020c16e4(self);
                        *(int*)(self + 0xa0) = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
            return;
        }

        if (*(int*)(self + 0xa0) <= 0)
            return;
        if (v == 0)
            return;
        {
            int i = 0;
            int lo = 2;
            int hi = 0x2e;
            int z = 0;
            int t = 0x1e;
            do {
                int e = data_ov005_020c2250[i];
                if (*(int*)(self + 0x58) != e) {
                    unsigned char ix = data_020a0e40;
                    unsigned char* p = (unsigned char*)data_020a0de8 + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        *(int*)(self + 0x58) = e;
                        func_02012790(z);
                        *(int*)(self + 0xa0) = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
        }
    }
}
}

// @symbol func_ov005_020c06cc
extern "C" {
void func_ov005_020c06cc(char *self)
{
    int (**vt)(void*);
    void *obj = (void*)data_0209f5bc;
    vt = *(int(***)(void*))obj;
    if (vt[5](obj) == 0) return;
    if (*(u8*)(self+0xac) != 0) return;
    if (*(u8*)(self+0x54) == 1) return;
    if (*(int*)(self+0x90) > 0) return;
    if (*(int*)(self+0x94) > 0) return;
    if (*(int*)(self+0x98) > 0) return;
    if (func_ov005_020c00e4(self) == 0) return;

    {
        u8 idx = data_020a0e40;
        int off = idx*4;
        int has = 0;
        if (((u8*)data_020a0de8)[off]) {
            if (data_020a0de9[off]) has = 1;
        }
        if (has == 0) return;
        if (data_0209b304 == 0) {
            u8 v = data_020a0dea[idx*4];
            if (v < 0xd8) return;
            if (v > 0xf8) return;
            if (data_020a0deb[idx*4] < 0x40) return;
            if (data_020a0deb[idx*4] > 0x80) return;
            func_02012790(0);
            *(int*)(self+0x94) = 0x12;
            return;
        } else {
            u8 v = data_020a0dea[idx*4];
            if (v < 0x38) return;
            if (v > 0x58) return;
            if (data_020a0deb[idx*4] < 0x40) return;
            if (data_020a0deb[idx*4] > 0x80) return;
            func_02012790(0);
            *(int*)(self+0x90) = 0x12;
            return;
        }
    }
}
}

// @symbol func_ov005_020c0378
extern "C" {
void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 n);

void func_ov005_020c0378(char *selfc)
{
    dScMiniGm_c *self = (dScMiniGm_c *)selfc;
    int sel;
    int ok;
    int row;

    if (data_0209f5bc->IsActive() == 0) return;
    if (self->mExiting != 0) return;
    if (self->mPageFlipped == 1) return;
    if (self->mPrevPageTimer > 0) return;
    if (self->mNextPageTimer > 0) return;
    if (self->mExitTimer > 0) return;

    sel = -1;
    ok = 0;
    if (data_020a0de8[data_020a0e40].pressed != 0) {
        if (data_020a0de8[data_020a0e40].held != 0) ok = 1;
    }
    if (ok != 0) {
        if (data_0209b304 == 0) {
            for (row = 0; row < 2; row++) {
                for (int col = 0; col < 3; col++) {
                    if (data_020a0de8[data_020a0e40].x >= 0x40 + col * 0x30 && data_020a0de8[data_020a0e40].x < 0x70 + col * 0x30 &&
                        data_020a0de8[data_020a0e40].y >= 0x18 + row * 0x48 && data_020a0de8[data_020a0e40].y < 0x60 + row * 0x48)
                        sel = data_0208a170 + (row * 0xc + col * 4);
                }
            }
        } else {
            for (int col = 0; col < 3; col++) {
                if (data_020a0de8[data_020a0e40].x >= 0x60 + col * 0x30 && data_020a0de8[data_020a0e40].x < 0x90 + col * 0x30 &&
                    data_020a0de8[data_020a0e40].y >= 0x38 && data_020a0de8[data_020a0e40].y < 0x80)
                    sel = data_0208a170 + (0x18 + col * 4);
            }
        }
    }

    if (sel < 0) return;
    if (func_ov005_020c00b4(self, sel) == 0) return;

    {
        const MgEntry *src = &data_ov005_020c24d8[sel];
        data_0208a174[0] = sel;
        data_0209b308.id = src->id;
        data_0209b308.unk4 = src->unk4;
        data_0209b308.unk8 = src->unk8;
        data_0209b308.unkc = src->unkc;
        data_0209b308.unk10 = src->unk10;
        data_0209b31c = src->mid;
        data_0209b308.unk28 = src->unk28;
        data_0209b308.unk2c = src->unk2c;
        data_0209b308.unk30 = src->unk30;
    }
    func_ov005_020c1688(selfc, sel);
    func_02012790(0x1e);
    _ZN8dScene_c9SetFadersEP15FaderBrightness(&data_0209f61c);
    func_0202ec9c(&data_0209f61c, 0);
    data_0209b308.unk30 = self->unk_05c;
    dScene_c::StartSceneFade(data_ov005_020c24d8[data_0208a174[0]].id, data_ov005_020c24d8[data_0208a174[0]].unk4, 0);
    _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
    self->mExiting = 1;
    data_0209b2fc = 1;
}
}

// @symbol func_ov005_020c0250
extern "C" {

void func_ov005_020c0250(char *self) {
    void* g = (void*)data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0) return;
    if (*(unsigned char*)(self+0xac) != 0) return;
    if (*(unsigned char*)(self+0x54) == 1) return;
    if (*(int*)(self+0x90) > 0) return;
    if (*(int*)(self+0x94) > 0) return;
    if (*(int*)(self+0x98) > 0) return;
    unsigned int i = data_020a0e40;
    bool ok = false;
    if (data_020a0de8[i].pressed != 0) {
        if (data_020a0de8[i].held != 0) ok = true;
    }
    if (!ok) return;
    unsigned char x = data_020a0de8[i].x;
    if (x < 0xd8) return;
    if (x > 0xf8) return;
    unsigned char y = data_020a0de8[i].y;
    if (y < 0x90) return;
    if (y > 0xb0) return;
    func_02012790(0x63);
    *(int*)(self+0x98) = 0x1c;
}
}

// @symbol func_ov005_020c0140
extern "C" {

void func_ov005_020c0140(char *self) {
    int* g = (int*)data_0209f5bc;
    if ((*(int(***)(int*))g)[0x14/4](g) == 0) return;
    if (*(unsigned char*)(self + 0xac) != 0) return;
    if (*(int*)(self + 0x90) > 0) return;
    if (*(int*)(self + 0x94) > 0) return;
    if (*(int*)(self + 0x98) > 0) return;
    if (data_0209b304 == 0) {
        if (*(int*)(self + 0x50) >= 0) {
            *(int*)((int)self + 0x50) -= 8;
            if (*(int*)(self + 0x50) <= 0) {
                *(int*)(self + 0x50) = 0;
                *(unsigned char*)(self + 0x54) = 0;
            }
        }
    } else {
        if (*(int*)(self + 0x50) <= 0xb0) {
            *(int*)((int)self + 0x50) += 8;
            if (*(int*)(self + 0x50) >= 0xb0) {
                *(int*)(self + 0x50) = 0xb0;
                *(unsigned char*)(self + 0x54) = 0;
            }
        }
    }
    data_0209d494[0] = (short)*(int*)(self + 0x50);
    data_0209d474[0] = (short)*(int*)(self + 0x50);
}
}

// @symbol func_ov005_020c00e4
extern "C" {
int func_ov005_020c00e4(void *self){
  int i, j;
  for(i=0;i<4;i++){
    char *p = (char*)i + 0x18;
    for(j=6;j<9;j++){
      if(func_ov005_020c00b4(self, (int)p)) return 1;
      p += 4;
    }
  }
  return 0;
}
}

// @symbol func_ov005_020c00b4
extern "C" {
int func_ov005_020c00b4(void *self, int n) {
    if (n < 8) {
        return 1;
    }
    return func_02013638(n - 8);
}
}

// @symbol func_ov005_020c007c
extern "C" {
int func_ov005_020c007c(void *self, int n) {
    if (n < 8) {
        return func_020135d0(n + 0x1c);
    } else {
        return func_020135d0(n - 8);
    }
}
}

// @symbol func_ov005_020c0030
extern "C" {
void func_ov005_020c0030(void *self, int n)
{
    int seen = func_ov005_020c007c(self, n);
    if (seen == 0)
        return;
    if (n < 8)
        func_02013598(n + 0x1c);
    else
        func_02013598(n - 8);
    data_0209b300 = 1;
}
}

// @symbol func_ov005_020c0010
extern "C" {
int func_ov005_020c0010(void *a, int i) {
    int offset = i * 0x34;
    return (*(unsigned int*)((char*)data_ov005_020c24dc + offset) >> 8) & 0xff;
}
}

// @symbol func_ov005_020bfff4
extern "C" {
int func_ov005_020bfff4(void *a, int b, int c) {
    int res = func_ov005_020c0010(a, b);
    return func_02013580(res, c);
}
}

// @symbol func_ov005_020bffc8
extern "C" {
void func_ov005_020bffc8(void *a0, int a1, int r2, int r3) {
    int r5 = r2;
    int r4 = r3;
    int r0 = func_ov005_020c0010(a0, a1);
    func_02013568(r0, r5, r4);
}
}

// @symbol func_ov005_020bff4c
extern "C" void func_ov005_020bff4c(char *self)
{
    if (data_0209b300 != 1) return;
    if (data_0209f5bc->IsActive() == 0) return;
    SaveData::SaveMinigames(&data_0209cae4);
    data_0209b300 = 0;
    data_0209f1d8 = 0;
}


