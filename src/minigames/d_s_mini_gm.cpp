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
 *   ClearNew, MiniGm_GetRecord, SetRecord
 *   call them; MiniGm_GetSaveTable reads the table's save-slot byte
 *   and hands it over. Naming belongs in arm9.
 * Leftover: func_02012790 is still the linker name of Sound::Play2D(2,
 *   id). This TU's Behavior and tap handlers (UpdateGameSelect /
 *   UpdatePageArrows / UpdateExitButton / UpdateGroupTabs)
 *   call it. Naming belongs in arm9.
 * Leftover: Disable3dEngines / func_020233f4 are arm9 graphics-setup
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
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2).
 */

#include "dScMiniGm_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "MgMenuRow.h"
#include "dWipe_c.h"
#include "dFdBrightness_c.h"
#include "types.h"
#include "Sound.h"
#include "OAM.h"
#include "decl_Scene.h"
#include "decl_common.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "nitro/os/cache.h"
#include "nitro/gx/g3x.h"
#include "dScene_c.h"
#include "LevelRng.h"
#include "SaveData.h"
#include "nitro/mi/memory.h"

struct OamAttr;

struct TouchRec {
    u8 held;
    u8 edge;
    u8 x;
    u8 y;
};
typedef struct Pair { int x, y; } Pair;
struct Off9 { int v[9]; };

/* ---- arm9 globals ---- */
extern u8 gMgSaveDirty;
extern u8 gMgMenuPage;
extern MgEntry gMgMenuRow;
extern MgRecordDefaults gMgMenuRowMid;
extern MinigameSaveData data_0209cae4;
extern u8 gSubScreenLayers;
extern u8 gMainScreenLayers;
extern short gSubBg3HOffset;
extern short gSubBg0HOffset;
extern dFdBrightness_c *gSceneFader; /* the installed screen fader */
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

/* ---- arm9 / cross-overlay externals ---- */
void func_02013568(int a, int b, int v);
int  func_02013580(int a, int b);
void func_02013598(int x);
int  func_020135d0(int r0);
int  func_02013638(int n);
extern "C" int RandomIntInternal(int *seed);
extern "C" void *LoadFile(int id);
extern "C" void DecompressLZ16(void *src, void *dst);

extern "C" void OamRender10_d_s_mini_gm(int sub, OamAttr *attr, int x, int y, int pal, int prio,
                                        int sx, int sy, int c, int d);

dScMiniGm_c *dScMiniGm_c_classInit() {
    return new dScMiniGm_c();
}

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
    void *f;

    LoadTextNarcs();
    LoadArchive(0);
    data_0209f5f8 = 0;
    Sound::LoadGroupAndSetBank(0x27, 5);
    Disable3dEngines();

    if (!(data_0209caa0.mg.unlocked & 0x80000000)) {
        tbl = sMiniGmRows;
        for (i = 0; i < 0x24; i++) {
            for (j = 0; j < 5; j++) {
                SetRecord(i, j, tbl->recordDefaults.v[j]);
            }
            tbl++;
        }
        data_0209caa0.mg.unlocked |= 0x80000000;
        gMgSaveDirty = 1;
    }

    tbl2 = sMiniGmRows;
    for (i2 = 0; i2 < 0x24; i2++) {
        flag = 0;
        if (tbl2->scoreCap != 0) {
            if (tbl2->ownRecords == 1) {
                if ((u32)MiniGm_GetRecord(this, i2, 0) > (u32)tbl2->scoreCap) {
                    flag = 1;
                }
            } else {
                for (j2 = 0; j2 < 5; j2++) {
                    if ((u32)MiniGm_GetRecord(this, i2, j2) > (u32)tbl2->scoreCap) {
                        flag = 1;
                    }
                }
            }
        }
        if (flag != 0) {
            for (j3 = 0; j3 < 5; j3++) {
                SetRecord(i2, j3, tbl2->recordDefaults.v[j3]);
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
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    GX_SetBankForTex(GX_VRAM_TEX_0_A);
    GX_SetBankForBG(GX_VRAM_BG_128_B);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_F);

    gMainScreenLayers &= ~(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 |
                           DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ);
    gSubScreenLayers &= ~0x1f;
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    GX_DispOn();
    GXS_DispOn();
    G3X_SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
    func_020233f4();

    G2_SetBG2Priority(1);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                         GX_BG_CHARBASE_0x10000);
    G2_BG2Mosaic(false);
    G2_SetBG2Offset(0, 0);
    SetBg2Offset(0, 0);

    f = LoadFile(0x4e);
    DecompressLZ16(f, G2_GetBG2CharPtr());
    Deallocate(f);

    f = LoadFile(0x58);
    DC_FlushRange(f, 0x200);
    GX_LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    f = LoadFile(0x59);
    DecompressLZ16(f, G2_GetBG2ScrPtr());
    Deallocate(f);

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
    DecompressLZ16(f, G2_GetOBJCharPtr());
    Deallocate(f);

    f = LoadFile(0xd9);
    DC_FlushRange(f, 0x100);
    GX_LoadOBJPltt(f, 0, 0x100);
    Deallocate(f);

    G2S_SetBG2Priority(1);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG2Mosaic(false);
    G2S_SetBG2Offset(0, 0);
    SetSubBg2Offset(0, 0);

    G2S_SetBG3Priority(3);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(mSubBgScrollX, 0);
    SetSubBg3Offset(mSubBgScrollX, 0);

    f = LoadFile(0x53);
    DecompressLZ16(f, G2S_GetBG2CharPtr());
    Deallocate(f);

    f = LoadFile(0x5a);
    DC_FlushRange(f, 0x200);
    GXS_LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    G2S_SetBG0Priority(2);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_512x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_23);
    G2S_BG0Mosaic(false);
    G2S_SetBG0Offset(mSubBgScrollX, 0);
    SetSubBg0Offset(mSubBgScrollX, 0);

    dst = (char *)G2S_GetBG0CharPtr();
    sp0 = 0;
    MI_CpuFill16(dst, sp0, 0x8000);
    dst = (char *)G2S_GetBG0ScrPtr();
    sp2 = 0x1f;
    MI_CpuFill16(dst, sp2, 0x1000);

    MiniGm_LoadGroupIcons(this);

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
    DecompressLZ16(f, G2S_GetOBJCharPtr());
    Deallocate(f);

    f = LoadFile(0xd9);
    DC_FlushRange(f, 0x100);
    GXS_LoadOBJPltt(f, 0, 0x100);
    Deallocate(f);

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
        gSoftResetDisabled = 1;
    }
    data_0208ee44 = 1;
    if (data_0209b2fc != 0) {
        dScene_c::SetFaders(&data_0209f61c);
    }

    data_0209b2fc = 0;
    gMainScreenLayers |= DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    gSubScreenLayers |= 0x11;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG1;
    gSubScreenLayers &= ~2;
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
    gSubScreenLayers |= 0xc;
    Sound::LoadAndSetMusic_Layer1(5);
    return 1;
}

void dScMiniGm_c::MiniGm_LoadGroupIcons() {
    dScMiniGm_c *scene = this;
    char *self = (char *)this;
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
    volatile u16 valA; /* pin MIi_CpuClear16 src */
    volatile u16 valB; /* pin MIi_CpuClear16 src */
    struct Off9 tbl;
    void *f;
    int fill;
    int nbytes;
    int t;

    f = LoadFile(sMiniGmBgFiles[gMgGroupBase]);
    DecompressLZ16(f, G2S_GetBG2ScrPtr());
    Deallocate(f);

    f = LoadFile(sMiniGmBgFiles[gMgGroupBase + 4]);
    DecompressLZ16(f, G2S_GetBG3ScrPtr());
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
                    scr = (char *)G2S_GetBG0ScrPtr();
                    dstp = scr + destOff;
                    dstp = destOff ? dstp : dstp; /* register-dest pin; arms identical */
                    valA = (u16)fill;
                    MI_CpuFill16(dstp, valA, nbytes);
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
            DecompressLZ16(f, (char *)G2S_GetBG0CharPtr() + 0x8000);
            Deallocate(f);

            row = m;
            destOff = row;
            do {
                idx = scene->mGfxSlot;
                ch = (char *)G2S_GetBG0CharPtr();
                scr = ch + (idx / 4) * 0x2400 + (idx % 4) * 0xc0 + destOff;
                ch = (char *)G2S_GetBG0CharPtr();
                MI_CpuCopy32(ch + 0x8000 + destOff, scr, copyLen);
                destOff += 0x400;
            } while (++row < 9);

            f = LoadFile(sMiniGmIconPalettes[gMgGroupBase + j]);
            DC_FlushRange(f, flushSize);
            GXS_LoadBGPltt(f, scene->mGfxSlot << 5, plttSize);
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
                    scr = (char *)G2S_GetBG0ScrPtr();
                    dstp = scr + destOff;
                    dstp = idx ? dstp : dstp; /* register-dest pin; arms identical */
                    valB = (u16)(q + (p + ((idx % 4) * 6 + ((idx / 4) * 0x120 + (idx << 12)))));
                    MI_CpuFill16(dstp, valB, nbytes);
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

void dScMiniGm_c::LoadIconPalette(int index) {
    void *file = LoadFile(sMiniGmIconPalettes[index]);
    DC_FlushRange(file, 0x200);
    u32 slot = mGfxSlots[index / 4];
    GXS_LoadBGPltt((u16 *)file + 16, slot << 5, 0x20);
    Deallocate(file);
}

s32 dScMiniGm_c::CleanupResources()
{
    if (gMgLastSelected[0] >= 0) {
        ClearNew(gMgLastSelected[0]);
    }
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

s32 dScMiniGm_c::Behavior()
{
    if ((data_020a0e5a[gActivePlayerSlot << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (gMgSaveDirty == 1) {
        MiniGm_SaveIfDirty((char *)this);
        return 1;
    }
    RandomIntInternal(&data_0209d4b8.value);
    RandomIntInternal(&data_0209e650.value);

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
    UpdateGroupTabs();
    UpdatePageArrows();
    UpdateGameSelect();
    UpdateExitButton();
    UpdatePageScroll();
    return 1;
}

void dScMiniGm_c::DrawRecords() {
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

    if (mPageFlipped == 1)
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
        if (MiniGm_IsUnlocked(this, gMgGroupBase + r8) != 0) {
            type = sMiniGmRows[gMgGroupBase + r8].scoringMode;
            if (type != 0) {
                if (type == 1) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[0], x - 0x10, y, neg1, zero);
                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    MiniGm_DrawNumber(this, x - 4, y, val);
                } else if (type == 2) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[1], x - 0x10, y, neg1, zero);
                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    MiniGm_DrawNumber(this, x - 4, y, val);
                } else if (type == 4) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[2], x - 0x10, y, neg1, zero);
                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    MiniGm_DrawNumber(this, x - 4, y, val);
                } else if (type == 3) {
                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    x = sMiniGmRecordPos[i].x;
                    y = sMiniGmRecordPos[i].y;
                    MiniGm_DrawNumber(this, x - 0x14, y, val);
                } else if (type == 5) {
                    y = sMiniGmRecordPos[i].y;
                    x = sMiniGmRecordPos[i].x;
                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    u = (unsigned int)val;
                    major = u / 60u;
                    MiniGm_DrawNumber(this, x - 0x10, y, (int)major);
                    OAM::RenderSub((OamAttr *)sMiniGmRecordLabels[4], x, y - 8, neg1, zero);

                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u;
                    MiniGm_DrawNumber(this, x + 8, y, (int)(pct / 600u));

                    val = MiniGm_GetRecord(this, gMgGroupBase + r8, zero);
                    u = (unsigned int)val;
                    rem = u % 60u;
                    pct = rem * 100u / 60u;
                    MiniGm_DrawNumber(this, x + 0x10, y, (int)(pct % 10u));
                }
            }
        }
        r8 += 4;
        i += 1;
    } while (i <= end);
}

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

void dScMiniGm_c::DrawNewBadges() {
    int g = (mIconBlinkPhase >= 0x20) ? 1 : 0;
    int start, end;
    if (gMgMenuPage == 1) {
        start = 6;
        end = 8;
    } else {
        start = 0;
        end = 5;
    }
    if (mPageFlipped == 1)
        return;
    int i;
    for (i = start; i <= end; i++) {
        if (MiniGm_IsUnlocked(this, gMgGroupBase + i * 4) == 0)
            continue;
        if (MiniGm_IsNew(this, gMgGroupBase + i * 4) == 0)
            continue;
        OAM::RenderSub((OamAttr *)sMiniGmNewBadge[g],
            sMiniGmBadgePos[i].x, sMiniGmBadgePos[i].y + 0x30, -1, 0);
    }
}

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

    DrawRecords();
    DrawNewBadges();

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

void dScMiniGm_c::OnPendingDestroy()
{
}

void dScMiniGm_c::UpdateGroupTabs() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPageFlipped == 1)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;

    if (mScrollDelay > 0) {
        int *pa0 = &mScrollDelay;
        *pa0 = *pa0 - 1;
        if (mScrollDelay == 0) {
            int f58 = mGroupBase;
            if (f58 != gMgGroupBase) {
                gMgGroupBase = f58;
                MiniGm_LoadGroupIcons();
                mScrollDelay = 0x1e;
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
        if (flag != 0 || (mScrollDelay <= 0 && v != 0)) {
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
                        mGroupBase = e;
                        func_02012790(z);
                        MiniGm_LoadGroupIcons();
                        mScrollDelay = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
            return;
        }

        if (mScrollDelay <= 0)
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
                if (mGroupBase != e) {
                    unsigned char ix = gActivePlayerSlot;
                    unsigned char* p = (unsigned char*)gTouchHeld + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        mGroupBase = e;
                        func_02012790(z);
                        mScrollDelay = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
        }
    }
}

void dScMiniGm_c::UpdatePageArrows() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPageFlipped == 1)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;
    if (MiniGm_HasSecondPage(this) == 0)
        return;

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
            mNextPageTimer = 0x12;
            return;
        } else {
            u8 v = gTouchX[idx*4];
            if (v < 0x38) return;
            if (v > 0x58) return;
            if (gTouchY[idx*4] < 0x40) return;
            if (gTouchY[idx*4] > 0x80) return;
            func_02012790(0);
            mPrevPageTimer = 0x12;
            return;
        }
    }
}

void dScMiniGm_c::UpdateGameSelect()
{
    int sel;
    int ok;
    int row;

    if (gSceneFader->IsAtStart() == 0)
        return;
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
    LoadIconPalette(sel);
    func_02012790(0x1e);
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.SetType(0);
    gMgMenuRow.level = mStartLevel;
    dScene_c::StartSceneFade(sMiniGmRows[gMgLastSelected[0]].id,
                             sMiniGmRows[gMgLastSelected[0]].sceneParam, 0);
    Sound::StopLoadedMusic_Layer1(0x1e);
    mExiting = 1;
    data_0209b2fc = 1;
}

void dScMiniGm_c::UpdateExitButton() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPageFlipped == 1)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;
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
    mExitTimer = 0x1c;
}

void dScMiniGm_c::UpdatePageScroll() {
    if (gSceneFader->IsAtStart() == 0)
        return;
    if (mExiting != 0)
        return;
    if (mPrevPageTimer > 0)
        return;
    if (mNextPageTimer > 0)
        return;
    if (mExitTimer > 0)
        return;
    if (gMgMenuPage == 0) {
        if (mSubBgScrollX >= 0) {
            mSubBgScrollX -= 8;
            if (mSubBgScrollX <= 0) {
                mSubBgScrollX = 0;
                mPageFlipped = 0;
            }
        }
    } else {
        if (mSubBgScrollX <= 0xb0) {
            mSubBgScrollX += 8;
            if (mSubBgScrollX >= 0xb0) {
                mSubBgScrollX = 0xb0;
                mPageFlipped = 0;
            }
        }
    }
    gSubBg0HOffset = (short)mSubBgScrollX;
    gSubBg3HOffset = (short)mSubBgScrollX;
}

int MiniGm_HasSecondPage(void *self) {
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 6; j < 9; j++) {
            if (MiniGm_IsUnlocked(self, j * 4 + i))
                return 1;
        }
    }
    return 0;
}

int MiniGm_IsUnlocked(void *self, int n) {
    if (n < 8) {
        return 1;
    }
    return func_02013638(n - 8);
}

int MiniGm_IsNew(void *self, int n) {
    if (n < 8) {
        return func_020135d0(n + 0x1c);
    } else {
        return func_020135d0(n - 8);
    }
}

void dScMiniGm_c::ClearNew(int n) {
    int seen = MiniGm_IsNew(this, n);
    if (seen == 0)
        return;
    if (n < 8)
        func_02013598(n + 0x1c);
    else
        func_02013598(n - 8);
    gMgSaveDirty = 1;
}

int MiniGm_GetSaveTable(void *a, int i) {
    int offset = i * 0x34;
    return (*(unsigned int *)((char *)sMiniGmSaveTable + offset) >> 8) & 0xff;
}

int MiniGm_GetRecord(void *a, int b, int c) {
    int res = MiniGm_GetSaveTable(a, b);
    return func_02013580(res, c);
}

void dScMiniGm_c::SetRecord(int a1, int r2, int r3) {
    int r5 = r2;
    int r4 = r3;
    int r0 = MiniGm_GetSaveTable(this, a1);
    func_02013568(r0, r5, r4);
}

void MiniGm_SaveIfDirty(char *self) {
    if (gMgSaveDirty != 1) return;
    if (gSceneFader->IsAtStart() == 0)
        return;
    SaveData::SaveMinigames(&data_0209cae4);
    gMgSaveDirty = 0;
    gSoftResetDisabled = 0;
}
