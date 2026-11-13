//cpp
/* dScGameOver_c -- the game-over screen, ov003.
 *
 * Mario is out of lives: eight "GAME OVER" glyphs drop in from off screen,
 * then a yes/no continue prompt (stylus or button; left is yes). 10
 * functions, .text 0x020b0580..0x020b117c: the seven dScene_c slots every
 * direct child overrides (0, 3, 6, 9, 12, 16, 17 -- the class adds no new
 * virtual), two non-virtual helpers that share the TU, and the factory
 * dScGameOver_c_classInit, which abuts InitResources at 0x020b1118 and ends
 * at ov003's .text end. D1/D0 are not
 * written here: the header's inline destructor plus the key function
 * (InitResources) emits them, D1 first, the ROM's order.
 *
 * Source order is the reverse of the ROM's -- mwccarm emits .text in
 * reverse source order. Do not reorder.
 *
 * deslop leftovers:
 * - UpdateIntro (0x020b060c) and UpdateButtonPalettes (0x020b0730) are
 *   non-virtual methods under coined names.
 * - OAM::Render's Fix12 overload stays mangled: OAM.h excludes the by-value
 *   Fix12<int> overloads (codegen wall, pinned in
 *   notes/experiments/gameover-2711-oam-render-fix12.md). G2S_GetBG1ScrPtr
 *   comes from nitro/gx/gx_bgcnt.h as `void *` and UpdateButtonPalettes casts it to u16* for its pointer
 *   arithmetic (typing ergonomics, not a codegen wall -- see
 *   notes/experiments/gameover-2711-bg1scrptr-u16.md).
 * - gSceneFader, the dFdBrightness_c dScene_c::SetFaders installed, gates
 *   Behavior through its IsAtStart virtual (slot 5). The TU-local shadow
 *   vtable this file used to spell out to that slot was measured inert and
 *   dropped for the plain call, so it is no longer a retained substitution
 *   (notes/experiments/gameover-2711-scenegate-virtual-call.md).
 * - mGlyphUnused[8]: zeroed by InitResources, never read back.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScGameOver_c_AssetId {
    ASSET_D_2D_BTM_GAME_OVER_BG_NSC_BIN = 0x0228,
    ASSET_D_2D_GAME_OVER_BG_NCL_BIN = 0x0231,
    ASSET_D_2D_GAME_OVER_BG_NSC_BIN = 0x0232,
    ASSET_D_2D_GAME_OVER_NCG_BIN = 0x0233,
    ASSET_D_2D_GAME_OVER_NCL_BIN = 0x0234,
    ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN = 0x025c
};

#include "dScGameOver_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "dScStage_c.h"
#include "OamAttr.h"
#include "OAM.h"
#include "SpawnInfo.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "dFdBrightness_c.h"
#include "Sound.h"

/* SetFaders stores the fade controller here; slot 5 waits for the entrance fade. */

/* Native scene data: prompt selection and the animated GAME OVER sprites. */
/* Screen-map offsets for the two prompt rows, and the glyph animation's
 * off-screen starts and final x positions. The initial-position symbol retains
 * its existing shared declaration in decl_common.h. */
u16 sGameOverPromptTileOffsets[2] = {224, 416};
s16 data_ov003_020b1764[8] = {-96, -69, -42, -16, 274, 297, 325, 352};
s16 sGameOverGlyphTargetX[8] = {32, 59, 86, 112, 146, 169, 197, 224};

/* Reconstructed names number the eight display slots; the ROM does not preserve
 * their original identifiers. attr3 terminates each single-sprite template. */
OamAttr sGameOverGlyph0 = {0x00f0, 0x81f0, 0x0000, 0xffff};
OamAttr sGameOverGlyph7 = {0x00f0, 0x81f0, 0x0018, 0xffff};
OamAttr sGameOverGlyph6 = {0x00f0, 0x81f0, 0x000c, 0xffff};
OamAttr sGameOverGlyph5 = {0x00f0, 0x81f0, 0x0014, 0xffff};
OamAttr sGameOverGlyph4 = {0x00f0, 0x81f0, 0x0010, 0xffff};
OamAttr sGameOverGlyph3 = {0x00f0, 0x81f0, 0x000c, 0xffff};
OamAttr sGameOverGlyph2 = {0x00f0, 0x81f0, 0x0008, 0xffff};
OamAttr sGameOverGlyph1 = {0x00f0, 0x81f0, 0x0004, 0xffff};

/* The ninth pointer addresses this terminated composite template. Render uses
 * only the first eight pointers. The historical extent beyond its terminator
 * is unknown; the following 24 zero bytes remain outside this TU's range. */
OamAttr sGameOverCompositeTemplate[8] = {
    {0x00e8, 0x8190, 0x0000, 0}, {0x00e8, 0x81ab, 0x0004, 0},      {0x00e8, 0x81c6, 0x0008, 0},
    {0x00e8, 0x81e0, 0x000c, 0}, {0x00e8, 0x8002, 0x0010, 0},      {0x00e8, 0x8019, 0x0014, 0},
    {0x00e8, 0x8035, 0x000c, 0}, {0x00e8, 0x8050, 0x0018, 0xffff},
};
OamAttr *sGameOverGlyphTemplates[9] = {
    &sGameOverGlyph0, &sGameOverGlyph1, &sGameOverGlyph2,
    &sGameOverGlyph3, &sGameOverGlyph4, &sGameOverGlyph5,
    &sGameOverGlyph6, &sGameOverGlyph7, sGameOverCompositeTemplate,
};

/* External scene services not declared by the included headers. */
extern dFdBrightness_c *gSceneFader;
extern int data_0208ee44;
/* Scalars, not tables: the two BG-enable bit registers, main and sub --
 * the spelling the rest of the tree uses for this pair. */
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern u8 data_0209f204;
/* u16* spelling for UpdateButtonPalettes' `p += ...` and 16-bit stores, no cast needed.
 * Ergonomics, not a constraint: the void* form compiles to identical bytes
 * at both consumers (see notes/experiments/gameover-2711-bg1scrptr-u16.md).
 * Init passes the result to DecompressLZ16's void* param either way. */

extern "C" void OamRender10_d_s_game_over(s32 sub, OamAttr *data, s32 x, s32 y, s32 palette,
                                          s32 priority, Fix12i scaleX, Fix12i scaleY, s32 rotation,
                                          s32 mode);
extern "C" void *LoadFile(int handle);
extern "C" void DecompressLZ16(void *src, void *dst);

/* The stylus went down this frame: held, with the held state just changed
 * (see PlayerInput.h). Int-returning, so the ROM materializes the 0/1 it
 * tests. */
static inline int IsTouchStarted() {
    int started = 0;
    unsigned char slot = gActivePlayerSlot;
    if (gTouchHeld[slot * 4] != 0)
        started = gTouchEdge[slot * 4] != 0;
    return started;
}

/* Reconstructed source-style name: SM64DS proves dScGameOver_c through RTTI,
 * allocation size, vtable identity, and the GAME_OVER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov003_020b1118.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020b1118 loads 0x98 -- the class's own size -- into fBase_c::operator
 * new; the inlined ctor runs fBase_c's C2, stores dBase_c then dScene_c's
 * vptrs, ORs pauseFlags 1 and 4 (dScene_c::dScene_c), then stores this
 * class's vptr. The null check is the one `new` itself emits. */
dScGameOver_c *dScGameOver_c_classInit(void) {
    return new dScGameOver_c();
}

/* [8] 0x020b0b3c -- vtable slot 0, KEY FUNCTION */
/* Brings both engines up for the game-over screen, loads the
 * language-specific "GAME OVER" art (five language variants on both
 * screens), then zeroes the cursor FSM and lays the eight glyphs out along
 * mGlyphX/mGlyphY with a fixed scale and no rotation. */
s32 dScGameOver_c::InitResources() {
    void *f;
    int i;

    Disable3dEngines();
    GX::DisableAllBanks();
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForOBJ(GX_VRAM_OBJ_16_G);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GXS_SetGraphicsMode(GX_BGMODE_3);

    GX_SetBGScrOffset(1);
    GX_SetPower(GX_POWER_ALL);
    GX_DispOn();

    GXS_DispOn();

    G2_SetBG0Priority(0);
    G2_BG0Mosaic(0);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x7000,
                     GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
    SetBg0Offset(0, 0);

    G2_SetBG1Priority(0);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800,
                     GX_BG_CHARBASE_0x1c000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);
    SetBg1Offset(0, 0);

    LoadDebugFont();

    G2_SetBG2Priority(0);
    G2_BG2Mosaic(0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6800,
                         GX_BG_CHARBASE_0x10000);
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
    DecompressLZ16(f, G2_GetBG2CharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_GAME_OVER_BG_NCL_BIN);
    GX_LoadBGPltt(f, 0, 0x1a0);
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_GAME_OVER_BG_NSC_BIN);
    DecompressLZ16(f, G2_GetBG2ScrPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN);
    DecompressLZ16(f, G2_GetBG0ScrPtr());
    Deallocate(f);

    G2_SetBG3Priority(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                         GX_BG_CHARBASE_0x00000);
    G2_BG3Mosaic(0);
    LoadFont(0);

    f = LoadFile(ASSET_D_2D_GAME_OVER_NCG_BIN);
    DecompressLZ16(f, G2_GetOBJCharPtr());
    Deallocate(f);

    f = LoadFile(ASSET_D_2D_GAME_OVER_NCL_BIN);
    GX_LoadOBJPltt(f, 0, 0x200);
    Deallocate(f);

    gMainScreenLayers = DISPLAY_LAYER_BG2 | DISPLAY_LAYER_OBJ;

    G2S_SetBG0Priority(0);
    G2S_BG0Mosaic(0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    SetSubBg0Offset(0, 0);

    G2S_SetBG1Priority(0);
    G2S_BG1Mosaic(0);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
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
    DecompressLZ16(f, (void *)G2S_GetBG1CharPtr());
    Deallocate(f);

    f = LoadFile(0x9807);
    GXS_LoadBGPltt(f, 0, 0x200);
    Deallocate(f);

    f = LoadFile(0x9801);
    DecompressLZ16(f, G2S_GetBG1ScrPtr());
    Deallocate(f);

    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6800,
                          GX_BG_CHARBASE_0x00000);
    SetSubBg2Offset(0, 0);

    f = LoadFile(ASSET_D_2D_BTM_GAME_OVER_BG_NSC_BIN);
    DecompressLZ16(f, G2S_GetBG2ScrPtr());
    Deallocate(f);

    func_0201cebc(0x27d);
    UpdateButtonPalettes(1);

    mCursorState = 0;
    mSelection = 0;
    mCommitTimer = 0;
    mFlashTimer = 0;
    mIntroPhase = 0;
    for (i = 0; i < 8; i++) {
        mGlyphX[i] = data_ov003_020b1764[i];
        mGlyphY[i] = 0x60;
        mGlyphDelay[i] = 0x30;
        mGlyphUnused[i] = 0;
    }

    gSubScreenLayers = 0x14;
    GX_SetVisiblePlane(gMainScreenLayers);
    GXS_SetVisiblePlane(DISPLAY_LAYER_BG2 | DISPLAY_LAYER_OBJ);
    data_0209f204 = 1;
    Sound::LoadInitialGroup(2);
    Sound::LoadAndSetMusic_Layer1(0x15);
    data_0208ee44 = 1;
    return 1;
}

/* [7] 0x020b0b34 -- vtable slot 3. Owns no allocation: everything drawn is
 * OAM data belonging to the overlay, so there is nothing to release. */
s32 dScGameOver_c::CleanupResources() {
    return 1;
}

/* [6] 0x020b0894 -- vtable slot 6. A two-state cursor FSM over the yes/no
 * prompt: state 0 reads the stylus box and the button, state 1 runs the two
 * countdowns down and commits -- continue (func_0202ae74) on "yes", or a
 * scene fade back out on "no". mIntroPhase gates all of it until the intro
 * is in. */
s32 dScGameOver_c::Behavior() {
    if (gSceneFader->IsAtStart() != 0) {
        UpdateIntro();

        if (mIntroPhase < 2)
            return 1;

        switch (mCursorState) {
        case 0: {
            unsigned char idx;
            if (!IsTouchStarted() && !IsButtonInputValid())
                break;
            idx = gActivePlayerSlot;
            if (((unsigned char)(gTouchX[idx * 4] - 8) < 0xf0 &&
                 (unsigned char)(gTouchY[idx * 4] - 0x38) < 0x20) ||
                (mSelection == 0 && IsButtonInputValid())) {
                if (mSelection == 0)
                    mFlashTimer = (unsigned char)(data_0208ee44 << 3);
                mSelection = 0;
                UpdateButtonPalettes(0);
                mCommitTimer = (unsigned char)(data_0208ee44 << 4);
                func_02012790(0x9a);
                mCursorState = 1;
                break;
            }
            idx = gActivePlayerSlot;
            if (((unsigned char)(gTouchX[idx * 4] - 8) < 0xf0 &&
                 (unsigned char)(gTouchY[idx * 4] - 0x68) < 0x20) ||
                (mSelection == 1 && IsButtonInputValid())) {
                if (mSelection == 1)
                    mFlashTimer = (unsigned char)(data_0208ee44 << 3);
                mSelection = 1;
                UpdateButtonPalettes(0);
                mCommitTimer = (unsigned char)(data_0208ee44 << 4);
                func_02012790(0x9b);
                mCursorState = 1;
            }
            break;
        }
        case 1:
            if (mFlashTimer != 0) {
                mFlashTimer -= data_0208ee44;
                if (mFlashTimer == 0)
                    UpdateButtonPalettes(0);
            }
            if (mCommitTimer != 0) {
                mCommitTimer -= data_0208ee44;
                if (mCommitTimer == 0) {
                    if (mSelection == 0)
                        func_0202ae74();
                    else
                        dScene_c::StartSceneFade(1, 0, 0);
                    Sound::StopLoadedMusic_Layer1(0x3c);
                }
            }
            break;
        }
    }
    return 1;
}

/* [5] 0x020b0814 -- vtable slot 9. Draws the eight "GAME OVER" glyph
 * sprites from the per-glyph x/y arrays InitResources laid out, then lets
 * dScStage_c draw the yes/no bouncing arrows once the cursor is live. */
s32 dScGameOver_c::Render() {
    int i;
    for (i = 0; i < 8; i++) {
        OAM::Render(0, sGameOverGlyphTemplates[i], mGlyphX[i], mGlyphY[i], -1, -1, 0x1000, 0x1000,
                    0, -1);
    }
    if (mIntroPhase >= 2)
        dScStage_c::RenderBouncingArrows();
    return 1;
}

/* [4] 0x020b0810 -- vtable slot 12. Nothing to unwind; the slot exists only
 * so dScene_c's teardown has something to call. */
void dScGameOver_c::OnPendingDestroy() {}

/* [3] 0x020b0730 -- repaints the sub-screen BG1 palette bank over the two
 * yes/no boxes: two 0x20-by-4 rectangles whose screen-map origins come from
 * sGameOverPromptTileOffsets. `unselectAll` non-zero forces both back to the unselected
 * bank; zero picks the highlighted bank for whichever box mSelection names,
 * and only while the mFlashTimer has run out. */
void dScGameOver_c::UpdateButtonPalettes(int unselectAll) {
    int i;
    for (i = 0; i < 2; i++) {
        unsigned int v;
        int j;
        unsigned short *p = (unsigned short *)G2S_GetBG1ScrPtr();
        p += sGameOverPromptTileOffsets[i];
        if (unselectAll) {
            v = 0x1000;
        } else {
            unsigned int w;
            if ((mSelection == i) && (mFlashTimer == 0))
                w = 0x2000;
            else
                w = 0x1000;
            v = (unsigned short)w;
        }
        for (j = 0; j < 0x20; j++) {
            p[0] = (p[0] & 0x3ff) | v;
            p[0x20] = (p[0x20] & 0x3ff) | v;
            p[0x40] = (p[0x40] & 0x3ff) | v;
            p[0x60] = (p[0x60] & 0x3ff) | v;
            p += 1;
        }
    }
}

/* [2] 0x020b060c -- the glyph drop-in step, one frame per call. */
/* The glyph drop-in animation step, one frame per call, driven off
 * mIntroPhase: phase 0 walks the eight glyphs, counting each one's
 * mGlyphDelay down and then sliding its mGlyphX toward the target in
 * sGameOverGlyphTargetX -- the first four from the left, the last four from
 * the right -- and flips to phase 1 when the last one lands; phase 1 waits
 * out one more mGlyphDelay tick, then reaches phase 2, which is what unblocks
 * Behavior's cursor FSM and Render's bouncing arrows, and enables the two
 * BG layers the prompt lives on. */
void dScGameOver_c::UpdateIntro() {
    unsigned char s = this->mIntroPhase;
    int i;
    int ip;
    switch (s) {
    case 0:
        i = 0;
        ip = 0xa;
        do {
            if (mGlyphDelay[i] != 0) {
                mGlyphDelay[i]--;
            } else {
                short tgt = sGameOverGlyphTargetX[i];
                short x = mGlyphX[i];
                if (x != tgt) {
                    if (i < 4) {
                        mGlyphX[i] += 0xc;
                        if (mGlyphX[i] >= tgt) {
                            mGlyphX[i] = tgt;
                            mGlyphDelay[i] = ip;
                        }
                    } else {
                        mGlyphX[i] -= 0xc;
                        if (mGlyphX[i] <= tgt) {
                            mGlyphX[i] = tgt;
                            mGlyphDelay[i] = ip;
                            mIntroPhase = 1;
                        }
                    }
                }
            }
            i++;
            ip += 4;
        } while (i < 8);
        break;

    case 1:
        if (mGlyphDelay[0] != 0) {
            mGlyphDelay[0]--;
            return;
        }
        this->mIntroPhase = 2;
        gMainScreenLayers |= DISPLAY_LAYER_BG0;
        gSubScreenLayers |= 3;
        break;
    }
}

/* [1] [0] 0x020b05bc (D0) and 0x020b0580 (D1) -- neither is written here.
 * The header's inline destructor is what emits the pair, D1 first, the ROM's
 * order (out of line mwccarm emits D2, D0, D1 plus a D2 the ROM never
 * carried). The key function is InitResources -- the first DECLARED
 * non-inline virtual -- and defining it here emits the vtable whose slots
 * 16/17 odr-use both halves. The empty bodies still reproduce all 0x8c
 * bytes: three inlined vptr stores plus fBase_c::~fBase_c, and D0's
 * inherited operator delete. See include/dScGameOver_c.h. */

/* Scene registry priorities use the base profile, without an actor tail. */
BaseSpawnInfo<dScGameOver_c> g_profile_GAME_OVER = {dScGameOver_c_classInit, 8, 11};
