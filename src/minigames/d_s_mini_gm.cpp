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
 *   TU's InitResources, MiniGm_IsUnlocked, MiniGm_IsNew,
 *   MiniGm_ClearNew, MiniGm_GetRecord, MiniGm_SetRecord
 *   call them; MiniGm_GetSaveTable reads the table's save-slot byte
 *   and hands it over. Naming belongs in arm9.
 * Leftover: func_02012790 is still the linker name of Sound::Play2D(2,
 *   id). This TU's Behavior and tap handlers (UpdateGameSelect /
 *   MiniGm_UpdatePageArrows / MiniGm_UpdateExitButton / MiniGm_UpdateGroupTabs)
 *   call it. Naming belongs in arm9.
 * Leftover: _ZN7dWipe_c13func_0202ec9cEi is the wipe start helper. This
 *   TU's UpdateGameSelect hands it data_0209f61c.
 * Leftover: func_02019028 / func_020233f4 are arm9 graphics-setup
 *   tails. This TU's InitResources calls them.
 * Leftover: sMiniGmGroupBases group-base table,
 *   sMiniGmBgFiles / sMiniGmIconPalettes / sMiniGmIconChars
 *   file-id tables, sMiniGmRecordPos / sMiniGmBadgePos icon
 *   positions, sMiniGmIconScreenOffsets screen-offset table,
 *   sMiniGmRows minigame entries, sMiniGmGroupTabs /
 *   sMiniGmPageArrows / sMiniGmExitButton / sMiniGmDigits /
 *   sMiniGmRecordLabels / sMiniGmNewBadge / sMiniGmSavingCaption /
 *   sMiniGmCaption OAM. Overlay .rodata/.data this TU does not
 *   own.
 * Leftover: g_profile_MINIGAME lives outside this TU (S14).
 * Leftover: OAM::Render Fix12-by-value stays mangled (OAM.h, wall 6az).
 *   This TU's Render.
 * Leftover: TouchOwner is a local overlay of data_0209f5bc. Slot 5 is
 *   IsActive. UpdateGameSelect / MiniGm_SaveIfDirty call the named
 *   method; the other tap helpers still dispatch vt[5] by hand.
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2).
 */

#include "dScMiniGm_c.h"
#include "MgMenuRow.h"
#include "dWipe_c.h"
#include "types.h"
#include "Sound.h"
#include "OAM.h"
#include "decl_Scene.h"
#include "decl_common.h"
#include "G2.h"
#include "GX.h"
#include "CP15.h"
#include "G3X.h"
#include "dScene_c.h"

struct MinigameSaveData;
struct OamAttr;

/* data_0209f5bc is the installed screen fader (a dFader_c). Every input
 * handler first asks its slot 5, IsAtStart, whether no fade is running;
 * three of them spell that call by hand through `(*(int(***)(...))g)[0x14/4]`. */
struct TouchOwner {
    virtual int v0(); virtual int v1(); virtual int v2();
    virtual int v3(); virtual int v4(); virtual int IsActive();
};
struct TouchRec {
    u8 held;
    u8 edge;
    u8 x;
    u8 y;
};
typedef struct Pair { int x, y; } Pair;
struct Off9 { int v[9]; };

namespace SaveData { void SaveMinigames(MinigameSaveData *); }

extern "C" {

/* ---- arm9 globals ---- */
extern u8 gMgSaveDirty;
extern u8 gMgMenuPage;
extern MgEntry gMgMenuRow;
extern MgRecordDefaults gMgMenuRowMid;
extern MinigameSaveData data_0209cae4;
extern u32 data_0209caa0[];
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern short data_0209d474[];
extern short data_0209d494[];
extern int data_0209e650;
extern TouchOwner *data_0209f5bc;
extern u8 data_0209f5f8;
extern dWipe_c data_0209f61c;
extern int data_0208ee44;
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern TouchRec gTouchHeld[];
// local extern: see above.
extern u8 gTouchEdge[];
// local extern: see above.
extern u8 gTouchX[];
// local extern: see above.
extern u8 gTouchY[];
// local extern: see above.
extern u8 gActivePlayerSlot;
extern unsigned short data_020a0e5a[];

/* ---- ov005 statics ---- */
extern int sMiniGmBgFiles[];
extern Pair sMiniGmRecordPos[];
extern Pair sMiniGmBadgePos[];
extern u32 sMiniGmIconPalettes[];
extern int sMiniGmIconChars[];
extern struct Off9 sMiniGmIconScreenOffsets; /* 9-entry screen-offset table, not a matrix */
/* The menu rows (MgEntry, MgMenuRow.h), 36 of them: rows 0-23 are the first
   page of each tab, 24-35 the second. StartSceneFade(id, sceneParam, 0) opens the
   game, so sceneParam becomes the scene's fBase_c param1. Its low byte is the
   variant that dScMgBase_c::IsAltVariant tests. Six games are listed twice:
   Amida (rows 0/24), Snowball (6/34), Smartball (10/22), Coin (14/30), Panel
   (15/35) and Slot1 (17/33). Each first entry has low byte 0 and each second
   1, except Amida's row 24 with 2 (Amida's override asks for exactly 1).
   Sound's only entry (row 19) has 0, so its IsAltVariant branch is not
   reached from this menu. Seen in play: the two Snowballs start with param1
   0x0a00 and 0x2201. Bytes 1 and 2 each give every row a distinct number
   0..35. */
extern const MgEntry sMiniGmRows[];
extern int sMiniGmSaveTable[];
extern int sMiniGmDigits[];
extern void *sMiniGmRecordLabels[];

/* ---- this TU's own members not already declared in decl_common.h ----
 * The file is written in REVERSE ROM order, so every intra-TU call is a
 * forward reference. */
int  MiniGm_GetSaveTable(void *self, int i);
void MiniGm_DrawNumber(void *self, int x, int y, int val);
void MiniGm_LoadIconPalette(char *self, int n);

/* ---- arm9 / cross-overlay externals ---- */
void func_02013568(int a, int b, int v);
int  func_02013580(int a, int b);
void func_02013598(int x);
int  func_020135d0(int r0);
int  func_02013638(int n);
int  RandomIntInternal(int *seed);
extern char *LoadFile(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void MultiStore16(u16 val, void *dst, int nbytes);

extern void OamRender10_d_s_mini_gm(int sub, OamAttr *attr, int x, int y, int pal, int prio, int sx,
                                    int sy, int c, int d);

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
    const MgEntry *tbl;
    int i2;
    int flag;
    int j2, j3;
    const MgEntry *tbl2;
    volatile u16 sp0;
    volatile u16 sp2;
    char *dst;
    char *f;

    LoadTextNarcs();
    LoadArchive(0);
    data_0209f5f8 = 0;
    Sound::LoadGroupAndSetBank(0x27, 5);
    func_02019028();

    if (!(data_0209caa0[0x48 / 4] & 0x80000000)) {
        tbl = sMiniGmRows;
        for (i = 0; i < 0x24; i++) {
            for (j = 0; j < 5; j++) {
                MiniGm_SetRecord((char *)this, i, j, tbl->recordDefaults.v[j]);
            }
            tbl++;
        }
        data_0209caa0[0x48 / 4] |= 0x80000000;
        gMgSaveDirty = 1;
    }

    tbl2 = sMiniGmRows;
    for (i2 = 0; i2 < 0x24; i2++) {
        flag = 0;
        if (tbl2->scoreCap != 0) {
            if (tbl2->ownRecords == 1) {
                if ((u32)MiniGm_GetRecord((char *)this, i2, 0) > (u32)tbl2->scoreCap) {
                    flag = 1;
                }
            } else {
                for (j2 = 0; j2 < 5; j2++) {
                    if ((u32)MiniGm_GetRecord((char *)this, i2, j2) > (u32)tbl2->scoreCap) {
                        flag = 1;
                    }
                }
            }
        }
        if (flag != 0) {
            for (j3 = 0; j3 < 5; j3++) {
                MiniGm_SetRecord((char *)this, i2, j3, tbl2->recordDefaults.v[j3]);
            }
            gMgSaveDirty = 1;
        }
        tbl2++;
    }

    if (gMgMenuPage == 1) {
        mSubBgScrollX = 0xb0;
    } else {
        mSubBgScrollX = 0;
    }
    mGfxSlot = 0;

    GX::DisableAllBanks();
    *(volatile u16 *)0x4000304 |= 0x8000;
    GX::SetBankForTex(1);
    GX::SetBankForBG(2);
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(8);
    GX::SetBankForOBJ(0x10);
    GX::SetBankForTexPltt(0x20);

    data_0209d45c &= ~0x1f;
    data_0209d454 &= ~0x1f;
    *(volatile u32 *)0x4000000 &= ~0x1f00;
    *(volatile u32 *)0x4001000 &= ~0x1f00;
    GX::SetGraphicsMode(1, 0, 1);
    GXS::SetGraphicsMode(0);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    *(volatile u32 *)0x4000000 &= ~0x7000000;
    *(volatile u32 *)0x4000000 &= ~0x38000000;
    GX::DispOn();
    *(volatile u32 *)0x4001000 |= 0x10000;
    G3X::SetFog(false, 0, 2, 0x1000);
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
    DecompressLZ16(f, G2::GetBG2CharPtr());
    Deallocate(f);

    f = LoadFile(0x58);
    CP15::FlushAndInvalidateDataCache((u32)f, 0x200);
    GX::LoadBGPltt((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    f = LoadFile(0x59);
    DecompressLZ16(f, G2::GetBG2ScrPtr());
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
    CP15::FlushAndInvalidateDataCache((u32)f, 0x100);
    GX::LoadOBJPltt((const void *)f, 0, 0x100);
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
    DecompressLZ16(f, G2S::GetBG2CharPtr());
    Deallocate((void *)f);

    f = LoadFile(0x5a);
    CP15::FlushAndInvalidateDataCache((u32)f, 0x200);
    GXS::LoadBGPltt((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 2;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x6010;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = mSubBgScrollX & 0x1ff;
    SetSubBg0Offset(mSubBgScrollX, 0);

    dst = (char *)G2S::GetBG0CharPtr();
    sp0 = 0;
    MultiStore16(sp0, dst, 0x8000);
    dst = (char *)G2S::GetBG0ScrPtr();
    sp2 = 0x1f;
    MultiStore16(sp2, dst, 0x1000);

    MiniGm_LoadGroupIcons((char *)this);

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
    CP15::FlushAndInvalidateDataCache((u32)f, 0x100);
    GXS::LoadOBJPltt((const void *)f, 0, 0x100);
    Deallocate((void *)f);

    mPageFlipped = 0;
    mArrowBobPhase = 0;
    mPrevPageTimer = 0;
    mNextPageTimer = 0;
    mExitTimer = 0;
    mIconBlinkPhase = 0;
    mStartLevel = 0;
    unk_060 = 0;
    unk_0a4 = 0;
    unk_0a8 = 0;
    mScrollDelay = 0;
    mGroupBase = gMgGroupBase;
    mExiting = 0;

    if (gMgSaveDirty == 1) {
        data_0209f1d8 = 1;
    }
    data_0208ee44 = 1;
    if (data_0209b2fc != 0) {
        dScene_c::SetFaders(&data_0209f61c);
    }

    data_0209b2fc = 0;
    data_0209d45c |= 0x11;
    data_0209d454 |= 0x11;
    data_0209d45c &= ~2;
    data_0209d454 &= ~2;
    data_0209d45c |= 4;
    data_0209d45c &= ~8;
    data_0209d454 |= 0xc;
    Sound::LoadAndSetMusic_Layer1(5);
    return 1;
}

// @symbol MiniGm_LoadGroupIcons
extern "C" {
void MiniGm_LoadGroupIcons(void *selfv)
{
    dScMiniGm_c *scene = (dScMiniGm_c *)selfv;
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

    f = LoadFile(sMiniGmBgFiles[gMgGroupBase]);
    DecompressLZ16(f, G2S::GetBG2ScrPtr());
    Deallocate(f);

    f = LoadFile(sMiniGmBgFiles[gMgGroupBase + 4]);
    DecompressLZ16(f, G2S::GetBG3ScrPtr());
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
        tbl = sMiniGmIconScreenOffsets;
        if (MiniGm_IsUnlocked(self, gMgGroupBase + j) == 0) {
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
                    scr = (char *)G2S::GetBG0ScrPtr();
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

            f = LoadFile(sMiniGmIconChars[gMgGroupBase + j]);
            DecompressLZ16(f, (char *)G2S::GetBG0CharPtr() + 0x8000);
            Deallocate(f);

            row = m;
            destOff = row;
            do {
                idx = scene->mGfxSlot;
                ch = (char *)G2S::GetBG0CharPtr();
                scr = ch + (idx / 4) * 0x2400 + (idx % 4) * 0xc0 + destOff;
                ch = (char *)G2S::GetBG0CharPtr();
                MultiCopy_Int((int *)(ch + 0x8000 + destOff), (int *)scr, copyLen);
                destOff += 0x400;
            } while (++row < 9);

            f = LoadFile(sMiniGmIconPalettes[gMgGroupBase + j]);
            CP15::FlushAndInvalidateDataCache((u32)f, flushSize);
            GXS::LoadBGPltt(f, scene->mGfxSlot << 5, plttSize);
            Deallocate(f);

            scene->mGfxSlots[i] = scene->mGfxSlot;

            off = tbl.v[i];
            rowB = pInit;
            p = pInit;
            do {
                q = qInit;
                destOff = off << 1;
                do {
                    char *dstp;
                    idx = scene->mGfxSlot;
                    scr = (char *)G2S::GetBG0ScrPtr();
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
                int *slot = &scene->mGfxSlot;
                *slot = *slot + 1;
                if (scene->mGfxSlot >= 10)
                    scene->mGfxSlot = resetVal;
            }
        }
        j += 4;
        i += 1;
    } while (i < 9);
}
}

// @symbol MiniGm_LoadIconPalette
extern "C" {
void MiniGm_LoadIconPalette(char *self, int index)
{
    char *file = LoadFile(sMiniGmIconPalettes[index]);
    CP15::FlushAndInvalidateDataCache((u32)file, 0x200);
    u32 slot = ((dScMiniGm_c *)self)->mGfxSlots[index / 4];
    GXS::LoadBGPltt(file + 0x20, slot << 5, 0x20);
    Deallocate(file);
}
}

// @symbol _ZN11dScMiniGm_c16CleanupResourcesEv
s32 dScMiniGm_c::CleanupResources()
{
    if (gMgLastSelected[0] >= 0) {
        MiniGm_ClearNew(this, gMgLastSelected[0]);
    }
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

// @symbol _ZN11dScMiniGm_c8BehaviorEv
s32 dScMiniGm_c::Behavior()
{
    if ((data_020a0e5a[gActivePlayerSlot << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (gMgSaveDirty == 1) {
        MiniGm_SaveIfDirty((char *)this);
        return 1;
    }
    RandomIntInternal(&data_0209d4b8);
    RandomIntInternal(&data_0209e650);

    if (mPrevPageTimer > 0) {
        mPrevPageTimer -= 1;
        if (mPrevPageTimer == 0) {
            gMgMenuPage = 0;
            mPageFlipped = 1;
        }
    } else if (mNextPageTimer > 0) {
        mNextPageTimer -= 1;
        if (mNextPageTimer == 0) {
            gMgMenuPage = 1;
            mPageFlipped = 1;
        }
    } else if (mExitTimer > 1) {
        mExitTimer -= 1;
        if (mExitTimer == 1) {
            dScene_c::SetAndStopColorFader();
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
    MiniGm_UpdateGroupTabs((char *)this);
    MiniGm_UpdatePageArrows((char *)this);
    UpdateGameSelect();
    MiniGm_UpdateExitButton((char *)this);
    MiniGm_UpdatePageScroll((char *)this);
    return 1;
}

// @symbol MiniGm_DrawRecords
extern "C" {
void MiniGm_DrawRecords(void *selfv)
{
    dScMiniGm_c *scene = (dScMiniGm_c *)selfv;
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

    if (scene->mPageFlipped == 1)
        return;

    if (gMgMenuPage == 1) {
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
        if (MiniGm_IsUnlocked(self, gMgGroupBase + r8) != 0) {
            type = sMiniGmRows[gMgGroupBase + r8].scoringMode;
            if (type != 0) {
                if (type == 1) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[0], x - 0x10, y, neg1, zero);
                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    MiniGm_DrawNumber(self, x - 4, y, val);
                } else if (type == 2) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[1], x - 0x10, y, neg1, zero);
                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    MiniGm_DrawNumber(self, x - 4, y, val);
                } else if (type == 4) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[2], x - 0x10, y, neg1, zero);
                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    MiniGm_DrawNumber(self, x - 4, y, val);
                } else if (type == 3) {
                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    x = sMiniGmRecordPos[i].x;
                    y = sMiniGmRecordPos[i].y;
                    MiniGm_DrawNumber(self, x - 0x14, y, val);
                } else if (type == 5) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    u = (unsigned int)val;
                    major = u / 60u;
                    MiniGm_DrawNumber(self, x - 0x10, y, (int)major);
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[4], x, y - 8, neg1, zero);

                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u;
                    MiniGm_DrawNumber(self, x + 8, y, (int)(pct / 600u));

                    val = MiniGm_GetRecord(self, gMgGroupBase + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u / 60u;
                    MiniGm_DrawNumber(self, x + 0x10, y, (int)(pct % 10u));
                }
            }
        }
        r8 += 4;
        i += 1;
    } while (i <= end);
}
}

// @symbol MiniGm_DrawNumber
extern "C" {

void MiniGm_DrawNumber(void *a0, int x, int y, int val){
    if(val==0){
        OAM::RenderSub((OamAttr *)sMiniGmDigits[0], x, y, -1, 0);
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
        OAM::RenderSub((OamAttr *)sMiniGmDigits[dig], x+xx, y, -1, 0);
        val=val/10;
        xx-=8;
    }while(val>0);
}
}

// @symbol MiniGm_DrawNewBadges
extern "C" {
void MiniGm_DrawNewBadges(void *selfv)
{
    dScMiniGm_c *scene = (dScMiniGm_c *)selfv;
    char *self = (char *)selfv;
    int g = (scene->mIconBlinkPhase >= 0x20) ? 1 : 0;
    int start, end;
    if (gMgMenuPage == 1) {
        start = 6;
        end = 8;
    } else {
        start = 0;
        end = 5;
    }
    if (scene->mPageFlipped == 1)
        return;
    int i;
    for (i = start; i <= end; i++) {
        if (MiniGm_IsUnlocked(self, gMgGroupBase + i * 4) == 0)
            continue;
        if (MiniGm_IsNew(self, gMgGroupBase + i * 4) == 0)
            continue;
        OAM::RenderSub((OamAttr *)sMiniGmNewBadge[g],
            sMiniGmBadgePos[i].x, sMiniGmBadgePos[i].y + 0x30, -1, 0);
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

    if (gMgSaveDirty == 1) {
        OAM::Render(0, (OamAttr *)sMiniGmSavingCaption, 0x80, 0xa4, -1, 0, 0x1000, 0x1000, 0, -1);
    } else {
        OAM::Render(0, (OamAttr *)sMiniGmCaption, 0x80, 0xa4, -1, 0, 0x1000, 0x1000, 0, -1);
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
            if (MiniGm_IsUnlocked(this, sMiniGmGroupBases[i] + j * 4) != 0 && MiniGm_IsNew(this, j * 4 + sMiniGmGroupBases[i]) != 0) {
                OAM::RenderSub((OamAttr *)sMiniGmNewBadge[blink], 0x18, y1, -1, 0);
                break;
            }
        }
        if (mGroupBase != sMiniGmGroupBases[i])
            k += 4;
        OAM::RenderSub((OamAttr *)sMiniGmGroupTabs[k], 0x18, y0, -1, 0);
        y1 += 0x30;
        y0 += 0x30;
    }

    if (mPageFlipped == 0) {
        if (gMgMenuPage != 0) {
            t = mPrevPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                OAM::Render(1, (OamAttr *)sMiniGmPageArrows[0], 0x50 - mArrowBobPhase / 2, 0x60, -1,
                            0, scale, 0);
            else
                OAM::Render(1, (OamAttr *)sMiniGmPageArrows[0], 0x50 - (0x40 - mArrowBobPhase) / 2,
                            0x60, -1, 0, scale, 0);
        } else if (MiniGm_HasSecondPage(this) != 0) {
            t = mNextPageTimer;
            if (t < 0xa)
                scale = 0x1000;
            else if (t < 0xe)
                scale = (((t - 0xa) << 12) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 12) / 16) + 0x1000;
            if (mArrowBobPhase < 0x20)
                OAM::Render(1, (OamAttr *)sMiniGmPageArrows[1], mArrowBobPhase / 2 + 0xe0, 0x60, -1,
                            0, scale, 0);
            else
                OAM::Render(1, (OamAttr *)sMiniGmPageArrows[1], (0x40 - mArrowBobPhase) / 2 + 0xe0,
                            0x60, -1, 0, scale, 0);
        }
    }

    MiniGm_DrawRecords(this);
    MiniGm_DrawNewBadges(this);

    t = mExitTimer;
    if (t < 0x14)
        scale = 0x1000;
    else if (t < 0x18)
        scale = (((t - 0x14) << 12) / 16) + 0x1000;
    else
        scale = (((0x1c - t) << 12) / 16) + 0x1000;
    OAM::Render(1, (OamAttr *)sMiniGmExitButton, 0xe8, 0xa0, -1, 0, scale, 0);
    return 1;
}

// @symbol _ZN11dScMiniGm_c16OnPendingDestroyEv
void dScMiniGm_c::OnPendingDestroy()
{
}

// @symbol MiniGm_UpdateGroupTabs
extern "C" {

void MiniGm_UpdateGroupTabs(char *self)
{
    dScMiniGm_c *scene = (dScMiniGm_c *)self;
    void* g = (void*)data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0)
        return;
    if (scene->mExiting != 0)
        return;
    if (scene->mPageFlipped == 1)
        return;
    if (scene->mPrevPageTimer > 0)
        return;
    if (scene->mNextPageTimer > 0)
        return;
    if (scene->mExitTimer > 0)
        return;

    if (scene->mScrollDelay > 0) {
        int* pa0 = &scene->mScrollDelay;
        *pa0 = *pa0 - 1;
        if (scene->mScrollDelay == 0) {
            int f58 = scene->mGroupBase;
            if (f58 != gMgGroupBase) {
                gMgGroupBase = f58;
                MiniGm_LoadGroupIcons(self);
                scene->mScrollDelay = 0x1e;
            }
        }
    }

    {
        int flag = 0;
        unsigned char idx = gActivePlayerSlot;
        unsigned char v = ((unsigned char*)gTouchHeld)[idx * 4];
        if (v != 0) {
            if (gTouchEdge[idx * 4] != 0)
                flag = 1;
        }
        if (flag != 0 || (scene->mScrollDelay <= 0 && v != 0)) {
            int i = 0;
            int lo = 2;
            int hi = 0x2e;
            int z = 0;
            int t = 0x1e;
            do {
                int e = sMiniGmGroupBases[i];
                if (gMgGroupBase != e) {
                    unsigned char ix = gActivePlayerSlot;
                    unsigned char* p = (unsigned char*)gTouchHeld + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        gMgGroupBase = e;
                        scene->mGroupBase = e;
                        func_02012790(z);
                        MiniGm_LoadGroupIcons(self);
                        scene->mScrollDelay = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
            return;
        }

        if (scene->mScrollDelay <= 0)
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
                int e = sMiniGmGroupBases[i];
                if (scene->mGroupBase != e) {
                    unsigned char ix = gActivePlayerSlot;
                    unsigned char* p = (unsigned char*)gTouchHeld + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        scene->mGroupBase = e;
                        func_02012790(z);
                        scene->mScrollDelay = t;
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

// @symbol MiniGm_UpdatePageArrows
extern "C" {
void MiniGm_UpdatePageArrows(char *self)
{
    dScMiniGm_c *scene = (dScMiniGm_c *)self;
    int (**vt)(void*);
    void *obj = (void*)data_0209f5bc;
    vt = *(int(***)(void*))obj;
    if (vt[5](obj) == 0) return;
    if (scene->mExiting != 0) return;
    if (scene->mPageFlipped == 1) return;
    if (scene->mPrevPageTimer > 0) return;
    if (scene->mNextPageTimer > 0) return;
    if (scene->mExitTimer > 0) return;
    if (MiniGm_HasSecondPage(self) == 0) return;

    {
        u8 idx = gActivePlayerSlot;
        int off = idx*4;
        int has = 0;
        if (((u8*)gTouchHeld)[off]) {
            if (gTouchEdge[off]) has = 1;
        }
        if (has == 0) return;
        if (gMgMenuPage == 0) {
            u8 v = gTouchX[idx*4];
            if (v < 0xd8) return;
            if (v > 0xf8) return;
            if (gTouchY[idx*4] < 0x40) return;
            if (gTouchY[idx*4] > 0x80) return;
            func_02012790(0);
            scene->mNextPageTimer = 0x12;
            return;
        } else {
            u8 v = gTouchX[idx*4];
            if (v < 0x38) return;
            if (v > 0x58) return;
            if (gTouchY[idx*4] < 0x40) return;
            if (gTouchY[idx*4] > 0x80) return;
            func_02012790(0);
            scene->mPrevPageTimer = 0x12;
            return;
        }
    }
}
}

// @symbol _ZN11dScMiniGm_c16UpdateGameSelectEv
void dScMiniGm_c::UpdateGameSelect()
{
    int sel;
    int ok;
    int row;

    if (data_0209f5bc->IsActive() == 0) return;
    if (mExiting != 0) return;
    if (mPageFlipped == 1) return;
    if (mPrevPageTimer > 0) return;
    if (mNextPageTimer > 0) return;
    if (mExitTimer > 0) return;

    sel = -1;
    ok = 0;
    if (gTouchHeld[gActivePlayerSlot].held != 0) {
        if (gTouchHeld[gActivePlayerSlot].edge != 0) ok = 1;
    }
    if (ok != 0) {
        if (gMgMenuPage == 0) {
            for (row = 0; row < 2; row++) {
                for (int col = 0; col < 3; col++) {
                    if (gTouchHeld[gActivePlayerSlot].x >= 0x40 + col * 0x30 && gTouchHeld[gActivePlayerSlot].x < 0x70 + col * 0x30 &&
                        gTouchHeld[gActivePlayerSlot].y >= 0x18 + row * 0x48 && gTouchHeld[gActivePlayerSlot].y < 0x60 + row * 0x48)
                        sel = gMgGroupBase + (row * 0xc + col * 4);
                }
            }
        } else {
            for (int col = 0; col < 3; col++) {
                if (gTouchHeld[gActivePlayerSlot].x >= 0x60 + col * 0x30 && gTouchHeld[gActivePlayerSlot].x < 0x90 + col * 0x30 &&
                    gTouchHeld[gActivePlayerSlot].y >= 0x38 && gTouchHeld[gActivePlayerSlot].y < 0x80)
                    sel = gMgGroupBase + (0x18 + col * 4);
            }
        }
    }

    if (sel < 0) return;
    if (MiniGm_IsUnlocked(this, sel) == 0) return;

    {
        const MgEntry *src = &sMiniGmRows[sel];
        gMgLastSelected[0] = sel;
        gMgMenuRow.id = src->id;
        gMgMenuRow.sceneParam = src->sceneParam;
        gMgMenuRow.scoringMode = src->scoringMode;
        gMgMenuRow.scoreCap = src->scoreCap;
        gMgMenuRow.ownRecords = src->ownRecords;
        gMgMenuRowMid = src->recordDefaults;
        gMgMenuRow.musicTrack = src->musicTrack;
        gMgMenuRow.musicMode = src->musicMode;
        gMgMenuRow.level = src->level;
    }
    MiniGm_LoadIconPalette((char *)this, sel);
    func_02012790(0x1e);
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.func_0202ec9c(0);
    gMgMenuRow.level = mStartLevel;
    dScene_c::StartSceneFade(sMiniGmRows[gMgLastSelected[0]].id,
                             sMiniGmRows[gMgLastSelected[0]].sceneParam, 0);
    Sound::StopLoadedMusic_Layer1(0x1e);
    mExiting = 1;
    data_0209b2fc = 1;
}

// @symbol MiniGm_UpdateExitButton
extern "C" {

void MiniGm_UpdateExitButton(char *self) {
    dScMiniGm_c *scene = (dScMiniGm_c *)self;
    void* g = (void*)data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0) return;
    if (scene->mExiting != 0) return;
    if (scene->mPageFlipped == 1) return;
    if (scene->mPrevPageTimer > 0) return;
    if (scene->mNextPageTimer > 0) return;
    if (scene->mExitTimer > 0) return;
    unsigned int i = gActivePlayerSlot;
    bool ok = false;
    if (gTouchHeld[i].held != 0) {
        if (gTouchHeld[i].edge != 0) ok = true;
    }
    if (!ok) return;
    unsigned char x = gTouchHeld[i].x;
    if (x < 0xd8) return;
    if (x > 0xf8) return;
    unsigned char y = gTouchHeld[i].y;
    if (y < 0x90) return;
    if (y > 0xb0) return;
    func_02012790(0x63);
    scene->mExitTimer = 0x1c;
}
}

// @symbol MiniGm_UpdatePageScroll
extern "C" {

void MiniGm_UpdatePageScroll(char *self) {
    dScMiniGm_c *scene = (dScMiniGm_c *)self;
    int* g = (int*)data_0209f5bc;
    if ((*(int(***)(int*))g)[0x14/4](g) == 0) return;
    if (scene->mExiting != 0) return;
    if (scene->mPrevPageTimer > 0) return;
    if (scene->mNextPageTimer > 0) return;
    if (scene->mExitTimer > 0) return;
    if (gMgMenuPage == 0) {
        if (scene->mSubBgScrollX >= 0) {
            scene->mSubBgScrollX -= 8;
            if (scene->mSubBgScrollX <= 0) {
                scene->mSubBgScrollX = 0;
                scene->mPageFlipped = 0;
            }
        }
    } else {
        if (scene->mSubBgScrollX <= 0xb0) {
            scene->mSubBgScrollX += 8;
            if (scene->mSubBgScrollX >= 0xb0) {
                scene->mSubBgScrollX = 0xb0;
                scene->mPageFlipped = 0;
            }
        }
    }
    data_0209d494[0] = (short)scene->mSubBgScrollX;
    data_0209d474[0] = (short)scene->mSubBgScrollX;
}
}

// @symbol MiniGm_HasSecondPage
extern "C" {
int MiniGm_HasSecondPage(void *self){
  int i, j;
  for(i=0;i<4;i++){
    char *p = (char*)i + 0x18;
    for(j=6;j<9;j++){
      if(MiniGm_IsUnlocked(self, (int)p)) return 1;
      p += 4;
    }
  }
  return 0;
}
}

// @symbol MiniGm_IsUnlocked
extern "C" {
int MiniGm_IsUnlocked(void *self, int n) {
    if (n < 8) {
        return 1;
    }
    return func_02013638(n - 8);
}
}

// @symbol MiniGm_IsNew
extern "C" {
int MiniGm_IsNew(void *self, int n) {
    if (n < 8) {
        return func_020135d0(n + 0x1c);
    } else {
        return func_020135d0(n - 8);
    }
}
}

// @symbol MiniGm_ClearNew
extern "C" {
void MiniGm_ClearNew(void *self, int n)
{
    int seen = MiniGm_IsNew(self, n);
    if (seen == 0)
        return;
    if (n < 8)
        func_02013598(n + 0x1c);
    else
        func_02013598(n - 8);
    gMgSaveDirty = 1;
}
}

// @symbol MiniGm_GetSaveTable
extern "C" {
int MiniGm_GetSaveTable(void *a, int i) {
    int offset = i * 0x34;
    return (*(unsigned int *)((char *)sMiniGmSaveTable + offset) >> 8) & 0xff;
}
}

// @symbol MiniGm_GetRecord
extern "C" {
int MiniGm_GetRecord(void *a, int b, int c) {
    int res = MiniGm_GetSaveTable(a, b);
    return func_02013580(res, c);
}
}

// @symbol MiniGm_SetRecord
extern "C" {
void MiniGm_SetRecord(void *a0, int a1, int r2, int r3) {
    int r5 = r2;
    int r4 = r3;
    int r0 = MiniGm_GetSaveTable(a0, a1);
    func_02013568(r0, r5, r4);
}
}

// @symbol MiniGm_SaveIfDirty
extern "C" void MiniGm_SaveIfDirty(char *self)
{
    if (gMgSaveDirty != 1) return;
    if (data_0209f5bc->IsActive() == 0) return;
    SaveData::SaveMinigames(&data_0209cae4);
    gMgSaveDirty = 0;
    data_0209f1d8 = 0;
}


