//cpp
/* dScMiniGm_c -- the ov005 minigame-select scene, as one intact translation
 * unit covering 0x020bfec0..0x020c21ec.
 *
 * This file is the canonical source for that whole span. It replaces the 25
 * one-function sources that used to cover it; they were folded in and removed,
 * and their shared-global declarations were reconciled into the single block
 * below (the manifest records each of those reconciliations as RESOLVED).
 * config/tu_manifest.d/ov005/dScMiniGm_c.json licenses the range and records
 * the compiler-only output the TU emits; config/arm9/overlays/ov005/delinks.txt
 * claims the span as one .text entry.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order). The
 * two destructor variants are not written here at all -- see the tail comment.
 *
 * The 25 licensed functions, by ROM address and linker name (ascending, i.e.
 * the reverse of the order they appear in below):
 *   [0] 0x020bfec0  _ZN11dScMiniGm_cD1Ev
 *   [1] 0x020bfefc  _ZN11dScMiniGm_cD0Ev
 *   [2] 0x020bff4c  func_ov005_020bff4c
 *   [3] 0x020bffc8  func_ov005_020bffc8
 *   [4] 0x020bfff4  func_ov005_020bfff4
 *   [5] 0x020c0010  func_ov005_020c0010
 *   [6] 0x020c0030  func_ov005_020c0030
 *   [7] 0x020c007c  func_ov005_020c007c
 *   [8] 0x020c00b4  func_ov005_020c00b4
 *   [9] 0x020c00e4  func_ov005_020c00e4
 *   [10] 0x020c0140  func_ov005_020c0140
 *   [11] 0x020c0250  func_ov005_020c0250
 *   [12] 0x020c0378  func_ov005_020c0378
 *   [13] 0x020c06cc  func_ov005_020c06cc
 *   [14] 0x020c0878  func_ov005_020c0878
 *   [15] 0x020c0b00  _ZN11dScMiniGm_c16OnPendingDestroyEv
 *   [16] 0x020c0b04  _ZN11dScMiniGm_c6RenderEv
 *   [17] 0x020c0f38  func_ov005_020c0f38
 *   [18] 0x020c1030  func_ov005_020c1030
 *   [19] 0x020c1130  func_ov005_020c1130
 *   [20] 0x020c14a0  _ZN11dScMiniGm_c8BehaviorEv
 *   [21] 0x020c1654  _ZN11dScMiniGm_c16CleanupResourcesEv
 *   [22] 0x020c1688  func_ov005_020c1688
 *   [23] 0x020c16e4  func_ov005_020c16e4
 *   [24] 0x020c1a20  _ZN11dScMiniGm_c13InitResourcesEv
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "dScMiniGm_c.h"
#include "types.h"
#include "Sound.h"
#include "decl_Scene.h"
#include "decl_common.h"

/* Reconciled shadow declarations.
 *
 * The 25 one-function files each invented their own view of the shared
 * globals: data_0209b300 was a scalar in five files and a one-element array in
 * a sixth, data_0209f5bc had five different pointee types, and the minigame
 * table was spelled `Entry`, `MgEntry` and `const MgEntry`. A TU can declare
 * each symbol only once, so the better-evidenced spelling is kept here and the
 * members that needed another view cast at the point of use.
 *
 * Everything include/decl_common.h already declares is deliberately absent
 * below and the definitions in this file are spelled to agree with it.
 */

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
int  _ZN8dScene_c14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
int  _ZN3OAM9RenderSubEP7OamAttriiii(void *oam, int x, int y, int a, int b);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN11dScMiniGm_c13InitResourcesEv, 0x020c1a20, size 0x7cc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMiniGm_c13InitResourcesEv
/* recovered: named members + real C++ method */
/* dScMiniGm_c::InitResources() -- vtable slot 0. Builds the minigame menu:
 * loads the per-language art onto both engines, walks the two minigame tables
 * to seed each entry's unlock state, then zeroes the whole
 * mPageFlipped..mExiting field block Behavior() and Render() drive. */
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
    unk_064 = 0;

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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov005_020c16e4, 0x020c16e4, size 0x33c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c16e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov005_020c1688, 0x020c1688, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c1688
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov005_020c1688(char* r6, int r5){
  char* r4 = LoadFile(data_ov005_020c2310[r5]);
  _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)r4, 0x200);
  u32 v = *(u32*)(r6 + (r5/4)*4 + 0x68);
  _ZN3GXS10LoadBGPlttEPKvjj(r4+0x20, v << 5, 0x20);
  Deallocate(r4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN11dScMiniGm_c16CleanupResourcesEv, 0x020c1654, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMiniGm_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScMiniGm_c::CleanupResources() -- vtable slot 3. Tears the running minigame
 * down only if one was actually selected (data_0208a174[0] >= 0), then drops
 * the voice group. */
s32 dScMiniGm_c::CleanupResources()
{
    if (data_0208a174[0] >= 0) {
        func_ov005_020c0030(this, data_0208a174[0]);
    }
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN11dScMiniGm_c8BehaviorEv, 0x020c14a0, size 0x1b4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMiniGm_c8BehaviorEv
/* recovered: named members + real C++ method */
/* dScMiniGm_c::Behavior() -- vtable slot 6. Runs the minigame-menu frame: a
 * touch anywhere plays the click, an active minigame (data_0209b300 == 1)
 * takes the whole frame, and otherwise three countdowns run in priority
 * order -- mPrevPageTimer and mNextPageTimer each flip the page when they
 * expire, mExitTimer closes the menu. With none pending, mArrowBobPhase and
 * mIconBlinkPhase free-run as 0..0x3f animation phases. */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov005_020c1130, 0x020c1130, size 0x370 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c1130
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov005_020c1130(void *slv)
{
    char *sl = (char *)slv;
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

    if (*(u8 *)(sl + 0x54) == 1)
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
        if (func_ov005_020c00b4(sl, data_0208a170 + r8) != 0) {
            type = ((Entry *)data_ov005_020c24d8)[data_0208a170 + r8].unk8;
            if (type != 0) {
                if (type == 1) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[0], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    func_ov005_020c1030(sl, x - 4, y, val);
                } else if (type == 2) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[1], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    func_ov005_020c1030(sl, x - 4, y, val);
                } else if (type == 4) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[2], x - 0x10, y, neg1, zero);
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    func_ov005_020c1030(sl, x - 4, y, val);
                } else if (type == 3) {
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    x = data_ov005_020c2280[i].x;
                    y = data_ov005_020c2280[i].y;
                    func_ov005_020c1030(sl, x - 0x14, y, val);
                } else if (type == 5) {
                    y = data_ov005_020c2280[i].y;
                    x = data_ov005_020c2280[i].x;
                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    major = u / 60u;
                    func_ov005_020c1030(sl, x - 0x10, y, (int)major);
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f4c[4], x, y - 8, neg1, zero);

                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u;
                    func_ov005_020c1030(sl, x + 8, y, (int)(pct / 600u));

                    val = func_ov005_020bfff4(sl, data_0208a170 + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u / 60u;
                    func_ov005_020c1030(sl, x + 0x10, y, (int)(pct % 10u));
                }
            }
        }
        r8 += 4;
        i += 1;
    } while (i <= end);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov005_020c1030, 0x020c1030, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c1030
extern "C" {

void func_ov005_020c1030(void *a0, int x, int y, int val){
    if(val==0){
        _ZN3OAM9RenderSubEP7OamAttriiii((void*)data_ov005_020c2efc[0], x, y, -1, 0);
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
        _ZN3OAM9RenderSubEP7OamAttriiii((void*)data_ov005_020c2efc[dig], x+xx, y, -1, 0);
        val=val/10;
        xx-=8;
    }while(val>0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov005_020c0f38, 0x020c0f38, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0f38
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov005_020c0f38(void* slv){
  char* sl = (char*)slv;
  int g = (*(int*)(sl+0x9c) >= 0x20) ? 1 : 0;
  int start, end;
  if(data_0209b304 == 1){ start = 6; end = 8; }
  else { start = 0; end = 5; }
  if(*(unsigned char*)(sl+0x54) == 1) return;
  int i;
  for(i = start; i <= end; i++){
    if(func_ov005_020c00b4(sl, data_0208a170 + i*4) == 0) continue;
    if(func_ov005_020c007c(sl, data_0208a170 + i*4) == 0) continue;
    _ZN3OAM9RenderSubEP7OamAttriiii((void*)data_ov005_020c2f60[g],
        data_ov005_020c22c8[i].x, data_ov005_020c22c8[i].y + 0x30, -1, 0);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN11dScMiniGm_c6RenderEv, 0x020c0b04, size 0x434 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMiniGm_c6RenderEv
/* recovered: real C++ method against include/dScMiniGm_c.h.

   dScMiniGm_c::Render -- vtable slot 9, ov005 0x020c0b04, 0x434 bytes. The minigame
   select screen: the character tabs down the left, a "new game" icon per tab whose
   nine games (ids base + 4*j) has one both unlocked and unseen, the page arrows and
   the exit button with their bounce scales.

   Rewritten from the disassembly. Credit for the earlier unmatched draft that sat here
   before stays with it: it came in with the minigame class wave (#1741), and its
   extern set and slot identification are what this rewrite started from.
   The flat draft forced the inner loop's two induction registers by hand; they
   fall out of the real source once the tab table is declared what the ROM says it
   is: data_ov005_020c2250 is the first word of ov005's .rodata, so it is `const`,
   and with a const table the compiler keeps ONE load of data_ov005_020c2250[i]
   across the two calls (an anonymous temp, r4) while strength reduction still
   gives the two textual `+ j * 4` uses their own induction registers (r6 for the
   first, r5 + r4 for the second). A named `base` local instead unifies both uses
   into one induction and colours r7. The
   second call spells `j * 4 + table[i]` because the cartridge adds the induction
   temp first. y1 is declared before y0 so y1 keeps fp and y0 spills. */
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
                _ZN3OAM9RenderSubEP7OamAttriiii((void *)data_ov005_020c2f60[blink], 0x18, y1, -1, 0);
                break;
            }
        }
        if (mGroupBase != data_ov005_020c2250[i])
            k += 4;
        _ZN3OAM9RenderSubEP7OamAttriiii((void *)data_ov005_020c2c28[k], 0x18, y0, -1, 0);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN11dScMiniGm_c16OnPendingDestroyEv, 0x020c0b00, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMiniGm_c16OnPendingDestroyEv
/* recovered: real C++ method */
/* dScMiniGm_c::OnPendingDestroy() -- vtable slot 12. Empty override. */
void dScMiniGm_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov005_020c0878, 0x020c0878, size 0x288 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0878
extern "C" {

void func_ov005_020c0878(char* sl)
{
    void* g = (void*)data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0)
        return;
    if (*(unsigned char*)(sl + 0xac) != 0)
        return;
    if (*(unsigned char*)(sl + 0x54) == 1)
        return;
    if (*(int*)(sl + 0x90) > 0)
        return;
    if (*(int*)(sl + 0x94) > 0)
        return;
    if (*(int*)(sl + 0x98) > 0)
        return;

    if (*(int*)(sl + 0xa0) > 0) {
        int* pa0 = (int*)((int)sl + 0xa0);
        *pa0 = *pa0 - 1;
        if (*(int*)(sl + 0xa0) == 0) {
            int f58 = *(int*)(sl + 0x58);
            if (f58 != data_0208a170) {
                data_0208a170 = f58;
                func_ov005_020c16e4(sl);
                *(int*)(sl + 0xa0) = 0x1e;
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
        if (flag != 0 || (*(int*)(sl + 0xa0) <= 0 && v != 0)) {
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
                        *(int*)(sl + 0x58) = e;
                        func_02012790(z);
                        func_ov005_020c16e4(sl);
                        *(int*)(sl + 0xa0) = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
            return;
        }

        if (*(int*)(sl + 0xa0) <= 0)
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
                if (*(int*)(sl + 0x58) != e) {
                    unsigned char ix = data_020a0e40;
                    unsigned char* p = (unsigned char*)data_020a0de8 + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        *(int*)(sl + 0x58) = e;
                        func_02012790(z);
                        *(int*)(sl + 0xa0) = t;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov005_020c06cc, 0x020c06cc, size 0x1ac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c06cc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov005_020c06cc(char *c)
{
    int (**vt)(void*);
    void *obj = (void*)data_0209f5bc;
    vt = *(int(***)(void*))obj;
    if (vt[5](obj) == 0) return;
    if (*(u8*)(c+0xac) != 0) return;
    if (*(u8*)(c+0x54) == 1) return;
    if (*(int*)(c+0x90) > 0) return;
    if (*(int*)(c+0x94) > 0) return;
    if (*(int*)(c+0x98) > 0) return;
    if (func_ov005_020c00e4(c) == 0) return;

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
            *(int*)(c+0x94) = 0x12;
            return;
        } else {
            u8 v = data_020a0dea[idx*4];
            if (v < 0x38) return;
            if (v > 0x58) return;
            if (data_020a0deb[idx*4] < 0x40) return;
            if (data_020a0deb[idx*4] > 0x80) return;
            func_02012790(0);
            *(int*)(c+0x90) = 0x12;
            return;
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov005_020c0378, 0x020c0378, size 0x354 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0378
/* recovered: real types against include/dScMiniGm_c.h.

   func_ov005_020c0378 -- ov005 0x020c0378, 0x354 bytes. The minigame select
   screen's tap handler: when the touch slot data_020a0e40 is pressed and held,
   find which icon cell it lands in (a 2x3 grid on the main page, a single row of
   three on the other), and if that game is unlocked copy its 0x34-byte entry into
   data_0209b308, fade the scene out and start it.

   Rewritten from the disassembly against the register assignment the cartridge
   shows, which pins three things about the source:
     * the touch slot index is not held in a local: data_020a0de8[data_020a0e40]
       is spelled out at every use, and the compiler keeps the one load in r5. A
       named `idx` local colours scratch (r2) and pushes the page base into r5.
     * the page base data_0208a170 is read inside the loops, hoisted by the compiler
       into lr / r3; naming it moves it to a callee-saved register.
     * each loop has its own counter (`int col` per loop). Reusing the 2x3 grid's
       column counter for the single-row loop colours it scratch (r2) instead of
       r8, and the whole row of inductions shifts down by one.
   The hit flag is an int, not a bool: as a bool the compiler folds it into the
   branch structure and the `mov r2, #0 / movne r2, #1` materialisation is lost. */

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
    _ZN8dScene_c14StartSceneFadeEjjt(data_ov005_020c24d8[data_0208a174[0]].id, data_ov005_020c24d8[data_0208a174[0]].unk4, 0);
    _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
    self->mExiting = 1;
    data_0209b2fc = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov005_020c0250, 0x020c0250, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0250
extern "C" {

void func_ov005_020c0250(char* c) {
    void* g = (void*)data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0) return;
    if (*(unsigned char*)(c+0xac) != 0) return;
    if (*(unsigned char*)(c+0x54) == 1) return;
    if (*(int*)(c+0x90) > 0) return;
    if (*(int*)(c+0x94) > 0) return;
    if (*(int*)(c+0x98) > 0) return;
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
    *(int*)(c+0x98) = 0x1c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov005_020c0140, 0x020c0140, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0140
extern "C" {

void func_ov005_020c0140(char* c) {
    int* g = (int*)data_0209f5bc;
    if ((*(int(***)(int*))g)[0x14/4](g) == 0) return;
    if (*(unsigned char*)(c + 0xac) != 0) return;
    if (*(int*)(c + 0x90) > 0) return;
    if (*(int*)(c + 0x94) > 0) return;
    if (*(int*)(c + 0x98) > 0) return;
    if (data_0209b304 == 0) {
        if (*(int*)(c + 0x50) >= 0) {
            *(int*)((int)c + 0x50) -= 8;
            if (*(int*)(c + 0x50) <= 0) {
                *(int*)(c + 0x50) = 0;
                *(unsigned char*)(c + 0x54) = 0;
            }
        }
    } else {
        if (*(int*)(c + 0x50) <= 0xb0) {
            *(int*)((int)c + 0x50) += 8;
            if (*(int*)(c + 0x50) >= 0xb0) {
                *(int*)(c + 0x50) = 0xb0;
                *(unsigned char*)(c + 0x54) = 0;
            }
        }
    }
    data_0209d494[0] = (short)*(int*)(c + 0x50);
    data_0209d474[0] = (short)*(int*)(c + 0x50);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov005_020c00e4, 0x020c00e4, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c00e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov005_020c00b4, 0x020c00b4, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c00b4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov005_020c00b4(void *r0, int n) {
    if (n < 8) {
        return 1;
    }
    return func_02013638(n - 8);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov005_020c007c, 0x020c007c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c007c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov005_020c007c(void *r0, int r1) {
    if (r1 < 8) {
        return func_020135d0(r1 + 0x1c);
    } else {
        return func_020135d0(r1 - 8);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov005_020c0030, 0x020c0030, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0030
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov005_020c0030(void *c, int r4){
  int r=func_ov005_020c007c(c, r4);
  if(r==0) return;
  if(r4<8) func_02013598(r4+0x1c);
  else func_02013598(r4-8);
  data_0209b300=1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov005_020c0010, 0x020c0010, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020c0010
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov005_020c0010(void *a, int i) {
    int offset = i * 0x34;
    return (*(unsigned int*)((char*)data_ov005_020c24dc + offset) >> 8) & 0xff;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov005_020bfff4, 0x020bfff4, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020bfff4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov005_020bfff4(void *a, int b, int c) {
    int res = func_ov005_020c0010(a, b);
    return func_02013580(res, c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov005_020bffc8, 0x020bffc8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020bffc8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov005_020bffc8(void *a0, int a1, int r2, int r3) {
    int r5 = r2;
    int r4 = r3;
    int r0 = func_ov005_020c0010(a0, a1);
    func_02013568(r0, r5, r4);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov005_020bff4c, 0x020bff4c, size 0x7c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov005_020bff4c
extern "C" void func_ov005_020bff4c(char *self)
{
    if (data_0209b300 != 1) return;
    if (data_0209f5bc->IsActive() == 0) return;
    SaveData::SaveMinigames(&data_0209cae4);
    data_0209b300 = 0;
    data_0209f1d8 = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11dScMiniGm_cD1Ev, 0x020bfec0, size 0x3c                */
/* ROM ordinal 0 -- _ZN11dScMiniGm_cD0Ev, 0x020bfefc, size 0x50                */
/* -------------------------------------------------------------------------- */
/* _ZN11dScMiniGm_cD1Ev and _ZN11dScMiniGm_cD0Ev are NOT written here on
 * purpose, and carry no @symbol marker: the inline destructor in
 * include/dScMiniGm_c.h emits D1 then D0 -- the cartridge's order, D1 at
 * 0x020bfec0 below D0 at 0x020bfefc -- and no D2. tools/tiers.py scores both
 * through that inline definition.
 *
 * Splitting this class across 25 one-function files had forced the destructor
 * to be written out twice, once per variant. Written out of line in a single
 * TU the compiler emits the group D0, D1, D2: D0 lands below D1, which is not
 * the cartridge's order, and the homeless D2 has no ROM address at all.
 *
 * D1, the complete-object destructor, stores this class's vtable over the one
 * the base constructor left, destroys the members in reverse declaration order
 * and then runs ~dScene_c. dScMiniGm_c adds only scalars, so there is nothing
 * to destroy and the body stays empty -- the chain follows from
 * `struct dScMiniGm_c : dScene_c` and the member types alone.
 *
 * D0, the deleting destructor, inlines that same teardown -- which is why more
 * than one vptr store appears -- and then returns the object to its heap
 * through an inlined operator delete, which is why nothing here mentions a
 * heap.
 */
