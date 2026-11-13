//cpp
/* dScTitle_c -- the title / level-select screen, ov003.
 *
 * A 0x36-entry level list (sTitleLevelEntries) with a cursor, gTitleCursor,
 * that D-pad up and down move one entry at a time, wrapping (up adds 0x35,
 * which is -1 modulo 0x36), a confirm that either fades to the picked level
 * or hands off to one of the two special entries, and a scrolling window,
 * gTitleListTop, that keeps the cursor on screen. Its startup use is not
 * established by these consumers. 11 functions,
 * .text 0x020ad660..0x020addfc: the seven dScene_c slots every direct child
 * overrides (0, 3, 6, 9, 12, 16, 17 -- the class adds no new virtual), the
 * free scroll helper TitleScrollLevelList, the factory dScTitle_c_classInit,
 * which abuts InitResources at 0x020adc10, and above it the two BG0 text
 * writers only the scroll helper calls. D1/D0 are not written here: the
 * header's inline destructor plus the key function (InitResources) emits
 * them, D1 first, the ROM's order.
 *
 * Source order is the reverse of the ROM's -- mwccarm emits .text in
 * reverse source order. Do not reorder.
 *
 * deslop leftovers:
 * - TitleScrollLevelList is written free here, and that is a reconstruction
 *   choice, not a deduction. The image preserves no original linker symbol
 *   table, so its name, like those of the two BG0 text writers
 *   (TitlePrintLevelRow, TitlePrintString), is coined from what the body
 *   does, and RTTI supplies class identities, not function spellings
 *   (notes/tu-promotion-conventions.md section 1 and
 *   notes/symbol-name-provenance.md). Both call sites hand it this object
 *   and every call to it is inside this class's region: that is evidence to
 *   narrow ownership on later -- call, layout or codegen -- not proof that
 *   the original was free. Its parameter is UNUSED by the body, while both
 *   call sites do load the receiver into r0, so the parameter is kept and
 *   spelled `void *` -- the one spelling neither call site has to cast away
 *   from. Until the ownership is narrowed, the original form stays
 *   uncertain, and the free form is only what this TU reproduces.
 */

#include "dScTitle_c.h"
#include "SpawnInfo.h"
#include "decl_common.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "private/controller_input.h"
#include "dFdBrightness_c.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "OAM.h"
#include "nitro/hw/registers.h"
#include "nitro/mi/memory.h"
#include "nitro/gx/g2.h"
#include "math/CalcLib.h"

/* SetFaders stores the fade controller here; slot 5 waits for the entrance fade. */

/* Stack slot whose address Render passes to OAM::EnableSubOAM. The callee
 * does not read it; the call still has to materialize the pointer in r0. */
struct OamTmp {
    int x;
};

/* ROM symbols this TU references that no header in the tree declares yet,
 * spelt by their exact final names under C linkage the way decl_common.h
 * spells its own. Everything else these
 * functions call comes from decl_common.h or Sound.h and is deliberately
 * NOT repeated here. */

/* The level-list cursor index. RECONCILED: the three legacy sources spelt
 * it `int[]`, `int` and `u32`. `u32` is the one the bytes need, because
 * Behavior's `% 0x36` is an unsigned reduction; the array spelling was only
 * ever read at [0]. */
extern u32 gTitleCursor;
extern char data_0209e674[];
extern dFdBrightness_c *gSceneFader;
extern PadData data_020a0e58[];
extern u8 gGameMode;
/* The scene-wide colour fader, dScene_c's dFdColor_c in
   src/game/stages/d_scene.cpp. Element 6 is its u16 `color` at offset 0xc.
   local extern: spelled as a u16 array because including dFdColor_c.h adds
   named parameters that renumber this TU's compiler-numbered string pool
   (sTitleLevelEntries' 54 rodata relocations move with it). */
extern u16 gSceneColorFader[];
#define SCENE_FADE_COLOR gSceneColorFader[6]
extern char data_020a0d90[];
extern u8 gMainScreenLayers;
extern int data_0208ee44;
extern char data_0209ee90[];
extern "C" extern void Enable3dEngines(void);
/* The print helper reads name; Behavior reads the signed level ID and entrance.
 * IDs -2 and -1 select the versus and minigame scenes. The two trailing bytes
 * are natural alignment padding; no observed consumer reads them. These type
 * and field names describe the consumers, not recovered original spellings. */
struct TitleLevelEntry {
    const char *name;
    s8 levelID;
    u8 entranceID;
};
#ifndef SM64DS_PLATFORM_PC
typedef char TitleLevelEntry_size_must_be_0x08[sizeof(TitleLevelEntry) == 0x08 ? 1 : -1];
#endif

/* Displayed strings retain their exact cartridge spelling. The compiler owns
 * the literal pool; the manifest binds its generated names to address labels. */
extern const TitleLevelEntry sTitleLevelEntries[] = {
    {"Test Map", 0, 0},        {"Main Castle Map", 1, 0}, {"Castle 1F", 2, 0},
    {"Main Garden Map", 3, 0}, {"Castle B1", 4, 0},       {"Castle 2F", 5, 0},
    {"BombHei Map", 6, 0},     {"Battan King Map", 7, 0}, {"Kaizoku Irie", 8, 0},
    {"Kaizoku Ship", 9, 0},    {"Snow Mt", 10, 0},        {"Snow Slider", 11, 0},
    {"Teresa House", 12, 0},   {"Cave", 13, 0},           {"Fire Land", 14, 0},
    {"Fire Mt", 15, 0},        {"Desert Land", 16, 0},    {"Desert Py", 17, 0},
    {"Water Land", 18, 0},     {"Snow Land", 19, 0},      {"Snow Kama", 20, 0},
    {"Water City", 21, 0},     {"Hight Mt", 22, 0},       {"High Slider", 23, 0},
    {"Tibi Deka D", 24, 0},    {"Tibi Deka T", 25, 0},    {"Tibi Deka In", 26, 0},
    {"Clock Tower", 27, 0},    {"Rainbow Cruise", 28, 0}, {"Peach Slider Map", 29, 0},
    {"Suisou", 30, 0},         {"Habatake", 31, 0},       {"Horisoko", 32, 0},
    {"Metal Switch", 33, 0},   {"Rainbow Mario", 34, 0},  {"Koopa1 Map", 35, 0},
    {"Koopa1 Boss", 36, 0},    {"Koopa2 Map", 37, 0},     {"Koopa2 Boss", 38, 0},
    {"Koopa3 Map", 39, 0},     {"Koopa3 Boss", 40, 0},    {"Onimasu Test Map", 41, 0},
    {"Vs Cross", 42, 0},       {"Vs Island", 43, 0},      {"Ex Mario Map", 44, 0},
    {"Ex Mario Boss", 45, 0},  {"Ex Luigi Map", 46, 0},   {"Ex Luigi Boss", 47, 0},
    {"Ex Wario Map", 48, 0},   {"Ex Wario Boss", 49, 0},  {"Play Room", 50, 0},
    {"Vs Main", 51, 0},        {"VS Entry", -2, 0},       {"Mini Game", -1, 0},
};
extern "C" /* `void *` is the tree's plurality spelling for this one (it is declared
 * five different ways across five files); the cast at the one call site is
 * free. */
/* RECONCILED: the two legacy call sites spelt the parameter `int` and
 * `char *`; the body ignores it. `void *` is the spelling neither call site
 * has to cast away from. */
extern void TitleScrollLevelList(void *self);


/* The stylus went down this frame: held, with the held state just changed
 * (see PlayerInput.h). Int-returning, so the ROM materializes the 0/1 it
 * ORs into the confirm test. */
static inline int IsTouchStarted() {
    int started = 0;
    u8 slot = gActivePlayerSlot;
    if (gTouchHeld[slot * 4] != 0)
        started = gTouchEdge[slot * 4] != 0;
    return started;
}

/* [10] 0x020adcbc -- prints one row of the level list into the BG0 screen
 * map at (row, col): the level number in two digits (a blank for a leading
 * zero), a blank, the level's name from sTitleLevelEntries, then blanks out
 * to column 0x1d. Coined name. Called only from TitleScrollLevelList. Each
 * tile goes through MIi_CpuClear16 from a volatile temporary, five of them;
 * that is the spelling the bytes need. */
extern "C" void TitlePrintLevelRow(int level, int row, int col) {
    volatile u16 t0, t1, t2, t3, t4;
    int tens = level / 10;
    int ones = level % 10;
    short *scr = (short *)G2_GetBG0ScrPtr();
    short *d = scr + (col + (row << 5));
    t0 = tens == 0 ? 0xff : tens + 0x30;
    MI_CpuFill16(d, t0, 2);
    t1 = ones + 0x30;
    MI_CpuFill16(d + 1, t1, 2);
    t2 = 0xff;
    short *d2 = d + 2;
    d += 3;
    MI_CpuFill16(d2, t2, 2);
    int cnt = 0x1d - col;
    volatile const signed char *s =
        reinterpret_cast<volatile const signed char *>(sTitleLevelEntries[level].name);
    while (*s != 0) {
        t3 = *s;
        MI_CpuFill16(d, t3, 2);
        s++;
        d++;
        cnt--;
    }
    for (int i = 0; i < cnt; i++) {
        t4 = 0xff;
        MI_CpuFill16(d, t4, 2);
        d++;
    }
}

/* [9] 0x020adc74 -- copies a NUL-terminated string into the BG0 screen map
 * at (row, col), one tile per character. Called only from
 * TitleScrollLevelList, for the caption. Coined name. */
extern "C" void TitlePrintString(signed char *s, int row, int col) {
    short *p = (short *)G2_GetBG0ScrPtr() + (col + (row << 5));
    if (*s == 0)
        return;
    do {
        *p++ = (short)*s++;
    } while (*s != 0);
}

/* Reconstructed source-style name: SM64DS proves dScTitle_c through RTTI,
 * allocation size, vtable identity, and the TITLE registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: func_ov003_020adc10.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020adc10 loads 0x54 -- the class's own size -- into fBase_c::operator
 * new; the inlined ctor runs fBase_c's C2, stores dBase_c then dScene_c's
 * vptrs, ORs pauseFlags 1 and 4 (dScene_c::dScene_c), then stores this
 * class's vptr. The null check is the one `new` itself emits. */
extern "C" dScTitle_c *dScTitle_c_classInit(void) {
    return new dScTitle_c();
}

/* [7] 0x020ada9c -- vtable slot 0, KEY FUNCTION. Brings the 2D engines up
 * for the title screen: banks, graphics mode, BG0 char/palette and the OBJ
 * set, blanks the BG0 screen map, then seeds the scroll window from the
 * cursor and starts the title music. */
s32 dScTitle_c::InitResources() {
    UnloadArchives();
    Enable3dEngines();
    GX::DisableAllBanks();
    GX_SetBankForBG(GX_VRAM_BG_128_D);
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_A);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GXS_DispOn();
    func_020233f4();
    G2_SetBG0Priority(0);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800,
                     GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_BG0Mosaic(0);
    GX_LoadBG0Char(data_0208a178, 0, 0x2000);
    GX_LoadBGPltt(data_0208c178, 0, 0x200);
    {
        u16 *scr = (u16 *)G2_GetBG0ScrPtr();
        int i = 0;
        do {
            *scr++ = 0xff;
            i++;
        } while (i < 0x1800);
    }
    GX_LoadOBJ(data_0208c378, 0, 0x2000);
    GX_LoadOBJPltt(data_0208c178, 0, 0x200);
    gMainScreenLayers = 0x11;
    GX_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ);
    data_0208ee44 = 2;
    TitleScrollLevelList(this);
    G2_SetBG0Offset(0, 0);
    data_0209ee90[0x340] = 0;
    data_0209ee90[0x341] = 0;
    Sound::LoadInitialGroup(2);
    Sound::LoadAndSetMusic_Layer1(0x38);
    return 1;
}

/* [6] 0x020ada80 -- vtable slot 3. The title scene owns only its voice
 * group; releasing that is the whole teardown. */
s32 dScTitle_c::CleanupResources() {
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

/* [5] 0x020ad814 -- vtable slot 6. Level-select cursor update: on confirm
 * (A or START, or a new stylus touch) starts the scene fade / loads the
 * picked level from sTitleLevelEntries; otherwise D-pad up or down moves the
 * cursor one entry (up adds 0x35) with the repeat delay at mRepeatDelay,
 * wrapping the index modulo 0x36. */
s32 dScTitle_c::Behavior() {
    if (gSceneFader->IsAtStart() && ((data_020a0e58[0].pressed & 9) | IsTouchStarted()) != 0) {
        u32 i;
        s8 t;
        func_02012790(1);
        i = gTitleCursor;
        t = sTitleLevelEntries[i].levelID;
        if (t == -1 || IsTouchStarted()) {
            dScene_c::StartSceneFade(5, 0, 0);
            Sound::StopLoadedMusic_Layer1(0x3c);
        } else if (t == -2) {
            dScene_c::StartSceneFade(6, 0, 0);
            Sound::StopLoadedMusic_Layer1(0x3c);
        } else {
            gGameMode = 0;
            LoadLevelNoReturn(t, sTitleLevelEntries[i].entranceID, 1, 0);
            SetPlayerGlobals();
            SetNumPlayers(1);
            dScene_c::StartSceneFade(4, 0, 0);
            SCENE_FADE_COLOR = 0x7fff;
        }
    } else {
        u16 v = data_020a0e58[0].held;
        if (v & 0x40) {
            int h = data_020a0e58[0].pressed & 0x40;
            if (h != 0 || mRepeatDelay == 0) {
                mRepeatDelay = h ? 8 : 2;
                gTitleCursor += 0x35;
                func_02012790(0);
            }
        } else if (v & 0x80) {
            int h = data_020a0e58[0].pressed & 0x80;
            if (h != 0 || mRepeatDelay == 0) {
                mRepeatDelay = h ? 8 : 2;
                gTitleCursor += 1;
                func_02012790(0);
            }
        }
    }

    gTitleCursor = gTitleCursor % 0x36;
    DecIfAbove0_Short(&mRepeatDelay);
    TitleScrollLevelList(this);
    return 1;
}

/* [4] 0x020ad7a4 -- vtable slot 9. Real C++ method over the shared header;
 * this body never touches its own fields. */
s32 dScTitle_c::Render() {
    OamTmp tmp;
    OAM::EnableSubOAM(&tmp);
    {
        int idx = data_0209e664;
        unsigned int v = ((gTitleCursor - gTitleListTop + 2) << 3) & 0xff;
        char *p = data_0209e674 + (idx << 3);
        *(int *)p = v | 0x80000;
        *(short *)(p + 4) = 0;
    }
    return 1;
}

/* [3] 0x020ad7a0 -- vtable slot 12. Nothing to unwind; the slot exists only
 * so dScene_c's teardown has something to call. */
void dScTitle_c::OnPendingDestroy() {}

/* [2] 0x020ad6ec -- scrolls the level-list window so the cursor stays on
 * screen: clamps the first shown entry, gTitleListTop, redraws the visible rows
 * through TitlePrintLevelRow, then repaints the caption. Coined name, written
 * free here as a reconstruction choice; see the file header. The parameter is unused
 * by the body, but both call sites load the receiver into r0.
 *
 * RECONCILED: decl_common.h already declares gTitleListTop `extern int`, so
 * the legacy `int[]` spelling is dropped in favour of the header's scalar
 * and the `[0]` subscripts became scalar accesses. */
extern "C" {
 /* .c-derived member: C linkage for the whole block */
void TitleScrollLevelList(void *) {
    int v = gTitleCursor;
    int nv = v - 0xa;
    gTitleListTop = nv;
    unsigned int t = (unsigned int)(v + 0xb);
    if (t > 0x36) {
        gTitleListTop = nv - (t - 0x36);
    }
    if (gTitleListTop < 0)
        gTitleListTop = 0;
    int s = gTitleListTop;
    int cnt = 0x36 - s;
    int end = cnt + 2;
    if (end > 0x17)
        end = 0x17;
    int i;
    int j = s;
    for (i = 2; i < end; i++) {
        TitlePrintLevelRow(j, i, 3);
        j++;
    }
    TitlePrintString((signed char *)data_020a0d90, 0, 0);
}
}


/* [1] [0] 0x020ad69c (D0) and 0x020ad660 (D1) -- neither is written here.
 * The header's inline destructor is what emits the pair, D1 first, the
 * ROM's order (out of line mwccarm emits D2, D0, D1 plus a D2 the ROM never
 * carried). The key function is InitResources -- the first DECLARED
 * non-inline virtual -- and defining it here emits the vtable whose slots
 * 16/17 odr-use both halves. The empty bodies still reproduce all 0x8c
 * bytes: three inlined vptr stores plus fBase_c::~fBase_c, and D0's
 * inherited operator delete. See include/dScTitle_c.h. */

/* Non-actor registry profile: allocating factory, behavior and render priorities. */
extern "C" BaseSpawnInfo<dScTitle_c> g_profile_TITLE = {dScTitle_c_classInit, 2, 4};
