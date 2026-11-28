//cpp
#include "types.h"
#include "Sound.h"
#include "LinkSettings.h"
#include "Link.h"
#include "OAM.h"
#include "OamAttr.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g2.h"

extern "C" {

/* The link session's state block at 0x0209fc50: present slots (fc5c, fc5d
   interior), re-ranked slots (fc64), player count (fc50), disconnect bits
   (fc58), a second state word (fc60) and the session state machine
   (fc68) the per-frame dispatcher func_02030e4c calls through
   data_020927ec. */
u8 data_0209fc50;
u8 data_0209fc54;
u8 data_0209fc58;
u8 data_0209fc5c[4];
int data_0209fc60;
s8 data_0209fc64[4];
int data_0209fc68;

extern void func_0203da2c(int v);
extern u8 gSoftResetDisabled;
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern void *data_020927b0[];
extern void *data_020927c4[];
extern void *data_020927d8[];
extern u8 data_0209440c;
extern u8 data_02092778;
extern int data_0209277c;
extern u8 data_0209e64c;
extern u8 gSoundInitialized;
extern u8 data_0208f274;
extern struct OamAttr *data_020927a0[];
extern void (*data_020927ec[])(void);

extern void Disable3dEngines(void);
extern void DecompressLZ16(const void *src, void *dst);
extern void SetBg1Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void func_02019a58(void);
extern void func_020132d8(void);
extern void func_0201ffcc(void);
extern void func_020199a4(void);
extern int func_0203d81c(void);
extern void func_0203d854(void);
extern void func_0203d868(void);
extern void func_0203d8d4(void);
extern void func_0203d8e8(void);
extern int func_0203d9b4(void);
extern int func_0203d950(int i);
extern int func_0203d890(void);
extern s32 func_0203d9f4(void);
extern void func_02030790(void);
extern void func_02030838(void);

/* The per-frame session check func_020197b8 runs. In states 1..4 a link
   error (gLinkStatusFlags bits 0x9d, GetLinkStatus) ends the session
   (func_02030790). It sets disconnect bit 0x10 for error 4, 0x20 for
   error 0x80, else all four player bits, plus 0x20 before state 4. In
   states 3 and 4, losing the parent (slot 0) or the data_0209fc54 flag
   d_s_entry_methods.cpp raises ends it too: player bit 0, 0x20 before
   state 4, 0x40 for the flag. Then the current state's handler from
   data_020927ec runs. Always 1. */
int func_02030e4c(void) {
    void (*fn)(void);
    if (data_0209fc68 != 0 && data_0209fc68 < 5 && GetLinkStatus(0x9d)) {
        if (GetLinkStatus(4)) {
            data_0209fc58 |= 0x10;
        } else if (GetLinkStatus(0x80)) {
            data_0209fc58 |= 0x20;
        } else {
            data_0209fc58 |= 15;
            if (data_0209fc68 < 4)
                data_0209fc58 |= 0x20;
        }
        func_02030790();
    } else if (data_0209fc68 >= 3 && data_0209fc68 < 5) {
        if (func_0203d950(0) == 0 || data_0209fc54 != 0) {
            data_0209fc58 |= 1;
            if (data_0209fc68 < 4)
                data_0209fc58 |= 0x20;
            if (data_0209fc54 != 0)
                data_0209fc58 |= 0x40;
            func_02030790();
        }
    }
    fn = data_020927ec[data_0209fc68];
    if (fn != 0)
        fn();
    return 1;
}

/* Session state 1's per-frame handler (data_020927ec[1]): seat the
   players the parent posts (func_0203d890) in order, as func_020308d0
   does for a local game. */
void func_02030dac(void) {
    int n = func_0203d890();
    int i;
    int j;
    for (i = 0; i < n; i++) {
        data_0209fc5c[i] = 1;
        data_0209fc64[i] = i;
    }
    for (j = n; j < 4; j++) {
        data_0209fc5c[j] = 0;
        data_0209fc64[j] = -1;
    }
    data_0209fc50 = n;
}

/* Session state 3's per-frame handler (data_020927ec[3]): mark the first
   data_0208f274 slots present if the link reports them (func_0203d950).
   An absent one clears slot 1, not its own slot, as the ROM does. Then
   rank the present slots, and once their count matches the player count
   the parent posts (func_0203d890) move to state 4 and post their mask
   (SetSessionPlayerMask). */
void func_02030ca0(void) {
    int count;
    int i;
    signed char cnt;
    int k;
    unsigned char mask;

    GetLocalPlayerId();
    count = data_0208f274;
    for (i = 0; i < count; i++) {
        if (func_0203d950(i))
            data_0209fc5c[i] = 1;
        else
            data_0209fc5c[1] = 0;
    }

    cnt = 0;
    mask = 0;
    for (k = 0; k < 4; k++) {
        if (data_0209fc5c[k] != 0) {
            data_0209fc64[k] = cnt;
            cnt = cnt + 1;
            mask |= (1 << k);
        } else {
            data_0209fc64[k] = -1;
        }
    }

    data_0209fc50 = cnt;
    if (data_0209fc50 != func_0203d890())
        return;
    if (data_0209fc68 != 0)
        data_0209fc68 = 4;
    SetSessionPlayerMask(mask);
}

/* Session state 4's per-frame handler (data_020927ec[4]): a peer slot
   (1..3) that is seated but no longer present on the link (func_0203d950)
   gets its disconnect bit in data_0209fc58 and ends the session
   (func_02030790). */
void func_02030c38(void) {
    int i;
    unsigned char *p = data_0209fc5c + 1;
    for (i = 1; i < 4; i++) {
        if (*p != 0 && func_0203d950(i) == 0) {
            data_0209fc58 |= 1 << i;
            func_02030790();
        }
        p++;
    }
}

/* Session state 5's per-frame handler (data_020927ec[5]). On the host
   (func_0203d9b4), post a slot mask of the players present on the link
   (func_0203d950) through SetSessionPlayerMask. A present slot whose bit the
   posted mask (IsPlayerInSession) no longer has is dropped and gets its
   disconnect bit in data_0209fc58. If slot 0 is still present but this
   console's own slot (GetLocalPlayerId) is not, flag all four and end the
   session (func_02030790). */
void func_02030b58(void) {
    if (func_0203d9b4() != 0) {
        u8 flags = 0;
        int j;
        for (j = 0; j < 4; j++) {
            if (func_0203d950(j) != 0)
                flags = (u8)(flags | (1 << j));
        }
        SetSessionPlayerMask(flags);
    }
    int i;
    for (i = 0; i < 4; i++) {
        if (data_0209fc5c[i] != 0) {
            if (IsPlayerInSession(i) == 0) {
                data_0209fc58 = (u8)(data_0209fc58 | (1 << i));
                data_0209fc5c[i] = 0;
            }
        }
    }
    if (data_0209fc5c[0] == 0)
        return;
    if (data_0209fc5c[GetLocalPlayerId()] != 0)
        return;
    data_0209fc58 |= 15;
    func_02030790();
}

/* Start a session in link mode arg (func_0203da2c; d_s_entry_methods.cpp
   passes 0, 1 and 3, d_s_mb.cpp 2) and clear the link error bits
   (ClearLinkErrors). No slot is present yet and no disconnect bit is set.
   The session state data_0209fc68 starts at 1 for mode 3 and at 3
   otherwise; data_020927ec holds the per-frame handler for each state,
   func_02030e4c calls it. The skip flag gSoftResetDisabled is raised. */
void func_02030aa4(int arg) {
    int i;
    func_0203da2c(arg);
    ClearLinkErrors();
    for (i = 0; i < 4; i++) {
        data_0209fc5c[i] = 0;
        data_0209fc64[i] = -1;
    }
    data_0209fc50 = 0;
    data_0209fc58 = 0;
    if (arg == 3)
        data_0209fc68 = 1;
    else
        data_0209fc68 = 3;
    data_0209fc60 = 0;
    data_0209fc54 = 0;
    gSoftResetDisabled = 1;
}

/* Leave the session: local link mode (func_0203da2c(0)) and session state
   0. dScEntry_c and dScStage_c call it once a disconnect is handled. */
void func_02030a78(void) {
    func_0203da2c(0);
    data_0209fc68 = 0;
}

/* func_02030a6c @ 0x2030a6c (arm9) -- tail-call veneer to func_0203d8e8 (0x203d8e8).
 * ldr ip, [pc]; bx ip; .word 0x203d8e8
 */
void func_02030a6c(void) {
    func_0203d8e8();
}

/* Ask the mode-3 link step to wind down (func_0203d8d4 sets
   data_020a0f40, which func_0203e20c reads), then
   clear the session state data_0209fc68 and the skip flag gSoftResetDisabled. */
void func_02030a38(void) {
    func_0203d8d4();
    data_0209fc68 = 0;
    gSoftResetDisabled = 0;
}

/* Re-rank the present slots once a session is running: each present slot
   (data_0209fc5c) gets the next rank in data_0209fc64, an absent one -1.
   The count goes to data_0209fc50, the disconnect bits data_0209fc58 are
   cleared, and the session state data_0209fc68 moves to 4. dScEntry_c
   calls it when the players are settled. */
void func_02030994(void) {
    signed char acc;
    int i;
    unsigned char *src;
    signed char *dst;
    if (data_0209fc68 == 0)
        return;
    acc = 0;
    src = data_0209fc5c;
    dst = data_0209fc64;
    i = 0;
    do {
        unsigned char b = *src;
        i++;
        src++;
        if (b != 0) {
            *dst = acc;
            acc = (acc + 1);
        } else {
            *dst = -1;
        }
        dst++;
    } while (i < 4);
    data_0209fc50 = acc;
    data_0209fc58 = 0;
    data_0209fc68 = 4;
}

/* The slot whose rank (data_0209fc64) is val, or -1. dScEntry_c uses it to
   map a player to its seat for the palette and score tables. */
int func_02030958(int val) {
    signed char i = 0;
    signed char *p = data_0209fc64;
    do {
        if (val == *p)
            return i;
        i = i + 1;
        p = p + 1;
    } while (i < 4);
    return -1;
}

/* Seat n players for a local game (SetNumPlayers): slots 0..n-1 are
   present (data_0209fc5c) and ranked in order (data_0209fc64), the rest
   absent with rank -1, and data_0209fc50 holds the count. */
void func_020308d0(int n) {
    int i;
    data_0209fc50 = n;
    for (i = 0; i < n; i++) {
        data_0209fc64[i] = i;
        data_0209fc5c[i] = 1;
    }
    for (i = n; i < 4; i++) {
        data_0209fc64[i] = -1;
        data_0209fc5c[i] = 0;
    }
}

/* Arm the link handshake: clear the passed latch data_020a0f30
   (func_0203d854), then raise flag 0x1000 in this console's input record
   (func_0203d868), which every console sees in its copy of the four
   records. func_020308a8 polls it. */
void func_020308b4(void) {
    func_0203d854();
    func_0203d868();
}

/* Poll the link handshake func_020308b4 armed (func_0203d81c): 1 once
   func_0203ea5c has seen flag 0x1000 in every present player's record. */
int func_020308a8(void) {
    return func_0203d81c();
}

/* If data_0209fc68 is non-null, looks up an OamAttr* in a table indexed by
   func_0203d9f4(), then calls OAM::Render(false, oam, 8, 0xb8, ...).
   Always calls empty stub func_02030838 at end. */
void func_0203083c(void) {
    if (data_0209fc68 != 0) {
        struct OamAttr *oam = data_020927a0[func_0203d9f4()];
        OAM::Render(false, oam, 8, 184, -1, -1, 0x1000, 0x1000, 0, -1);
    }
    func_02030838();
}

/* Empty. func_0203083c calls it after drawing the link icon, and
   func_020199a4 calls it too. */
void func_02030838(void) {}
}

extern s32 GetLanguageIndexVS(void);
void func_02030500(void);

/* End the VS session on an error: session state data_0209fc68 becomes 6,
   which dScEntry_c::HandlePlayerDisconnect reads as a connection error. If
   a session was running (data_02092778, which d_s_entry_methods.cpp sets),
   drop to local link mode (func_0203da2c(0)), put the BG1 picture up on
   both screens (func_02030500), and with the link started (data_0209e64c)
   and gSoundInitialized set, play sound 0x123 and stop the loaded music.
   func_0201ffcc then runs one more link step with the watchdog held, and
   func_020199a4 follows. The state handlers func_02030b58
   and func_02030c38 and the error checks in func_02030e4c call it after
   setting their disconnect bits in data_0209fc58. */
extern "C" void func_02030790(void) {
    data_0209fc68 = 6;
    if (data_02092778 == 0)
        return;
    func_0203da2c(0);
    func_02030500();
    data_0209277c = 0;
    func_02019a58();
    if (data_0209e64c != 0 && gSoundInitialized != 0) {
        func_02012790(0x123);
        Sound::StopLoadedMusic_Layer1(4);
        func_020132d8();
    }
    func_0201ffcc();
    func_020199a4();
}

/* Put up the same language-indexed BG1 picture on both screens: hide every
   plane, map the VRAM banks, set engine A to graphics mode 1, load the
   characters, palette and one of two screen maps (data_0209fc58 bits 4-5
   choose) on both engines, then show BG1 alone. */
void func_02030500(void) {
    void *p;

    gMainScreenLayers = 0;
    gSubScreenLayers = 0;
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);

    Disable3dEngines();
    GX::DisableAllBanks();
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);

    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    G2_BlendNone();
    G2S_BlendNone();
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                     GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2S_BG1Mosaic(0);

    DecompressLZ16(data_020927b0[GetLanguageIndexVS()], G2_GetBG1CharPtr());
    DecompressLZ16(data_020927b0[GetLanguageIndexVS()], G2S_GetBG1CharPtr());

    GX_LoadBGPltt(&data_0209440c, 0, 0x40);
    GXS_LoadBGPltt(&data_0209440c, 0, 0x40);

    if (data_0209fc58 & 0x30) {
        p = data_020927d8[GetLanguageIndexVS()];
    } else {
        p = data_020927c4[GetLanguageIndexVS()];
    }
    DecompressLZ16(p, G2_GetBG1ScrPtr());
    DecompressLZ16(p, G2S_GetBG1ScrPtr());

    G2_SetBG1Priority(0);
    G2S_SetBG1Priority(0);
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    gMainScreenLayers = DISPLAY_LAYER_BG1;
    gSubScreenLayers = DISPLAY_LAYER_BG1;
    GX_SetVisiblePlane(DISPLAY_LAYER_BG1);
    GXS_SetVisiblePlane(DISPLAY_LAYER_BG1);
}
