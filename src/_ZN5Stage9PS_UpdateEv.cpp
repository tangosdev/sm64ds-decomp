//cpp
// NONMATCHING: 17/20 switch cases byte-identical (jump-table-anchored region compare, region div=180; global aligned div=756, was 916). KEY UNLOCK: #pragma opt_common_subs off switches mwccarm to EBB-local CSE = the ROM's per-block slot*4 rematerialization (case 8 now BYTE-EXACT incl. language arms; mid-case 47-word pools in cases 1 and 0xa now placed exactly as ROM). Named locals = manual long-lived CSE under the pragma (int idxa = slot*4 for cross-block reuse); inline exprs remat per EBB. Shared reused int temp (tmpv/rel/rely - ONE variable reassigned per range check, m2c shows them as separate rel0/rel1) reproduces the ROM's stale-temp reads (case 0xa bl_toggle entry tests (u8)tmpv NOT relx - ROM reads the stale temp; opt_okback in 0xa is || not &&). Residuals: case 1 (45: dead-cmp-no-branch cell at loop arm A - branch removal keeps and+cmp, every guarded-dead-code spelling either keeps the bhs or DCEs the cmp too; -2 insns), case 0xa (111: pure reg rotation slot r6/tx r3/ty2 r2 vs mine r2/r1/r0, size EXACT), case 0x11 (24: same rotation class, size exact). The two rotations survive decl-order + register-kw permutation and may be coupled to case 1's residual via function-wide allocation.
/* Stage::PS_Update at 0x0202635c (arm9), size 0x30ac (12,460 bytes, 3115 insns)
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 *
 * Full decompilation of the pause-screen state machine: 6 timer blocks +
 * a 20-case switch on PAUSE_MENU_ID. Byte-identical regions: prologue,
 * timers, jump table, cases 0,2,3,4,5,6,9,0xb,0xc,0xd,0xe,0xf,0x10,0x12,
 * 0x13 and the shared tail.
 *
 * Working notes for the refine tier:
 * - Dead compare tails after each button-select come from
 *   `if ((u8)(dea[...]-K) < N) return; return;` (tail-merge keeps the cmp;
 *   bare expressions and empty ifs are DCE'd; identical-arms if/else inside
 *   a loop merges WITHOUT the cmp - that is case 1's remaining dead-cmp).
 * - data_020a0e40/dea/deb behave volatile; de8/de9 do not (ROM reuses their
 *   loads across blocks). Mixed extern decls below reflect that.
 * - Case 8/0xa wall: the ROM's language arms are 6 insns (no lsl); slot*4
 *   is materialized once at the join and REMATERIALIZED per mode block
 *   (r0 at the join, fresh lsl inside each block after calls clobber it).
 *   Every spelling tried (idx var before/after ty, no idx var, int slot,
 *   per-arm slot locals+merge) hoists one lsl into each language arm and
 *   keeps a long-lived idx register instead. 3 extra insns x 2 cases, and
 *   it perturbs the whole-case register rotation.
 * - Case 8 mode blocks decoded: `if (a==0) goto change; t=1;
 *   if (!(a && de9[idx])) t=0; if (t==0) goto keep;
 *   if (f2dc != MODE) goto keep; change: ... keep: ...` - this shape now
 *   matches the ROM's control flow exactly (cmp/beq/cmp/beq chain).
 * - Frame is 0x14: five named constant locals (0,0,0,0x51,0x52) in case 1
 *   spill to [sp..sp+0x10]; 0xf/1/2 live in r7/r8/r6. Restructuring case-1
 *   temps can silently drop spill slots and shift every add sp - check
 *   `sub sp,#0x14` first after any case-1 edit. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;
typedef short s16;
typedef signed int s32;
typedef unsigned int u32;

extern "C" {
int IsButtonInputValid(void);
int func_02029408(void);
void _ZN5Stage20PS_UpdateOptionsMenuEv(void);
void _ZN5Stage25PS_UpdateOkAndBackButtonsEb(int b);
void _ZN5Stage17UpdateMenuButtonsEb(int b);
void _ZN5Stage17PS_UpdateSaveMenuEb(int b);
void _ZN5Stage10PS_CleanupEv(void);
void _ZN7Message7DisplayEj(int m);
void _ZN7Message11DisplayTextEt(int m);
void _ZN7Message16DisplayPauseTextEth(int m, u8 a);
void _ZN7Message18DisplayPauseTextVSEt(int m);
void _ZN7Message22DisplayOptionsMenuTextEt(int m);
void _ZN7Message19DisplaySaveMenuTextEt(int m);
void _ZN7Message21DisplaySaveStatusTextEt(int m);
void _ZN7Message19DisplayDontSaveTextEt(int m);
void _ZN7Message25DisplayControllerModeTextEt(int m);
void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, int c);
int GetOwnerLanguage(void);
u16 *_ZN3G2S12GetBG1ScrPtrEv(void);
void SetControllerMode(int m);
void LoadLevelNoReturn(int a, int b, int c, int d);
int SublevelToLevel(int lv);
int CountStarsCollectedInLevelToDisplay(int lv);
void SetBg1Offset(int x, int y);
void SetBg2Offset(int x, int y);
void SetBg3Offset(int x, int y);
void SetSubBg0Offset(int x, int y);
void SetSubBg1Offset(int x, int y);
void func_02012790(int snd);
void LoadCompressedFileAt(int id, void *dst);
int GetSoundMode(void);
void SetSoundMode(int m);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
void _ZN8SaveData15SaveCurrentFileEv(void);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 a);

extern int GAME_SPEED_RELATED;          /* frame tick */
extern u8 BACKLIGHT_ENABLED;
extern u8 data_0208ee40;
extern s8 LEVEL_OF_LAST_COLLECTED_STAR;
extern u8 BOTTOM_SCREEN_RELATED;
extern u8 TOP_SCREEN_RELATED;
extern u8 NEXT_PAUSE_MENU_ID;           /* next pause state */
extern u8 OPTIONS_MENU_OPENED;
extern u8 ARROW_TIMER;           /* timer: star scroll */
extern u8 PAUSED_WITH_SELECT;
extern u8 PAUSE_MENU_TIMER;           /* timer: whole update pause */
extern u8 SOUND_OPTION;           /* sound mode selection */
extern u8 SELECTED_ARROW;
extern u8 TIMER_0209f23c;           /* timer -> options */
extern u8 BACK_BUTTON_PRESSED_2;
extern u8 MENU_CHANGE_TIMER;           /* timer: button flash */
extern u8 PAUSE_MENU_ID;           /* current pause state */
extern u8 CURR_PLAYER_ID;
extern u8 STOP_MUSIC_DURING_FADE;
extern u8 BACK_BUTTON_PRESSED;
extern u8 SELECTED_BUTTON_SMALL;
extern u8 NUM_BIG_BUTTONS;
extern u8 SAVE_MENU_TIMER;           /* save countdown */
extern u8 GAME_PAUSED;
extern u8 MENU_DISPLAYED_COURSE;           /* displayed level */
extern u8 BACKLIGHT_OPTION_TIMER;           /* timer -> options */
extern u8 CURRENT_GAMEMODE;
extern u8 SELECTED_CONTROLLER_MODE;
extern u8 SELECTED_BUTTON;           /* selected button */
extern u8 SOUND_MODE_TIMER;           /* timer -> options */
extern u8 BACKLIGHT_OFF;
extern u8 COURSE_ARROW_BUTTONS_INVISIBLE;
extern s8 LEVEL_ID;
extern u16 STAGE_TIMER;
extern u16 BG1_BUTTON_TILE_OFFSETS[];
extern u8 data_0209f4ae[];
extern int DP_STATE;
extern int NEXT_ACTOR_UPDATE_FLAGS;
extern int ACTOR_UPDATE_FLAGS;
extern s16 data_020756d0[];
extern u8 data_02111150;
extern volatile u8 data_020a0e40;  /* active input slot */
extern u8 TOUCH_INPUT_ARR[]; /* touch: active   [slot*4]   */
extern u8 data_020a0de9[]; /* touch: pressed  [slot*4]   */
extern volatile u8 data_020a0dea[]; /* touch: x        [slot*4]   */
extern volatile u8 data_020a0deb[]; /* touch: y        [slot*4]   */

#define DE8P(off) ((u8 *)((long long)(int)(TOUCH_INPUT_ARR + (off)) & 0xFFFFFFFFFFFFFFFFLL))
#define REG16(a) (*(volatile u16 *)(a))
#define REG32(a) (*(volatile u32 *)(a))

struct Stage {
    static void PS_Update();
};

#pragma opt_common_subs off
void Stage::PS_Update()
{
    register u8 var_sl;
    register u8 var_fp;
    s32 sp0;
    s32 sp4;
    s32 sp8;
    s32 spC;
    s32 sp10;
    s32 var_r0;

    if (ARROW_TIMER != 0) {
        ARROW_TIMER = ARROW_TIMER - GAME_SPEED_RELATED;
        if (ARROW_TIMER == 0)
            SELECTED_ARROW = 0;
    }
    if (SOUND_MODE_TIMER != 0) {
        SOUND_MODE_TIMER = SOUND_MODE_TIMER - GAME_SPEED_RELATED;
        if (SOUND_MODE_TIMER == 0)
            _ZN5Stage20PS_UpdateOptionsMenuEv();
    }
    if (MENU_CHANGE_TIMER != 0) {
        MENU_CHANGE_TIMER = MENU_CHANGE_TIMER - GAME_SPEED_RELATED;
        if (MENU_CHANGE_TIMER == 0) {
            u8 st = PAUSE_MENU_ID;
            if (st == 8 || st == 0xa) {
                if (BACK_BUTTON_PRESSED_2 != 0) {
                    BACK_BUTTON_PRESSED_2 = 0;
                    _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                } else if (st == 8) {
                    _ZN5Stage17UpdateMenuButtonsEb(0);
                }
            } else if ((u8)(st + 0xef) <= 1) {
                _ZN5Stage17PS_UpdateSaveMenuEb(0);
            } else {
                _ZN5Stage17UpdateMenuButtonsEb(0);
            }
        }
    }
    if (BACKLIGHT_OPTION_TIMER != 0) {
        BACKLIGHT_OPTION_TIMER = BACKLIGHT_OPTION_TIMER - GAME_SPEED_RELATED;
        if (BACKLIGHT_OPTION_TIMER == 0)
            _ZN5Stage20PS_UpdateOptionsMenuEv();
    }
    if (TIMER_0209f23c != 0) {
        TIMER_0209f23c = TIMER_0209f23c - GAME_SPEED_RELATED;
        if (TIMER_0209f23c == 0)
            _ZN5Stage20PS_UpdateOptionsMenuEv();
    }
    if (PAUSE_MENU_TIMER != 0) {
        PAUSE_MENU_TIMER = PAUSE_MENU_TIMER - GAME_SPEED_RELATED;
        return;
    }

    if (PAUSE_MENU_ID != NEXT_PAUSE_MENU_ID)
        PAUSE_MENU_ID = NEXT_PAUSE_MENU_ID;

    switch (PAUSE_MENU_ID) {
    case 0: {
        u8 i;
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000a) = (REG16(0x400000a) & 0x43) | 0x1210;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        i = 0;
        do {
            if (CountStarsCollectedInLevelToDisplay(i) != 0)
                break;
            i = (u8)(i + 1);
        } while (i < 0xf);
        if (i == 0xf) {
            MENU_DISPLAYED_COURSE = i;
            COURSE_ARROW_BUTTONS_INVISIBLE = 1;
        } else {
            if (SublevelToLevel(LEVEL_OF_LAST_COLLECTED_STAR) >= 0xf) {
                MENU_DISPLAYED_COURSE = 0xf;
            } else {
                MENU_DISPLAYED_COURSE = SublevelToLevel(LEVEL_OF_LAST_COLLECTED_STAR);
            }
            COURSE_ARROW_BUTTONS_INVISIBLE = 0;
        }
        _ZN7Message7DisplayEj(data_020756d0[MENU_DISPLAYED_COURSE]);
        BG1_BUTTON_TILE_OFFSETS[0] = 0x80;
        BG1_BUTTON_TILE_OFFSETS[1] = 0x120;
        BG1_BUTTON_TILE_OFFSETS[2] = 0x1c0;
        BG1_BUTTON_TILE_OFFSETS[3] = 0x260;
        NUM_BIG_BUTTONS = 4;
        SELECTED_BUTTON = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        BACK_BUTTON_PRESSED_2 = 0;
        _ZN5Stage25PS_UpdateOkAndBackButtonsEb(1);
        SetBg1Offset(0, 0);
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        TOP_SCREEN_RELATED |= 0xe;
        BOTTOM_SCREEN_RELATED |= 3;
        NEXT_PAUSE_MENU_ID = 1;
        return;
    }
    case 1: {
        if (STAGE_TIMER != 0)
            return;
        {
            int touched = 0;
            u8 slot = data_020a0e40;
            if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                if (data_020a0de9[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x20) < 0x20)
                goto sel1_0;
            if (SELECTED_BUTTON == 0 && IsButtonInputValid() != 0)
                goto sel1_0;
            goto chk1_1;
        sel1_0:
            if (SELECTED_BUTTON == 0)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 0;
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            GAME_PAUSED = 2;
            func_02012790(3);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x28) < 0xb0)
                return;
            return;
        }
    chk1_1:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x48) < 0x20)
                goto sel1_1;
            if (SELECTED_BUTTON == 1 && IsButtonInputValid() != 0)
                goto sel1_1;
            goto chk1_2;
        sel1_1:
            if (SELECTED_BUTTON == 1)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 7;
            func_02012790(0x53);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk1_2:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x70) < 0x20)
                goto sel1_2;
            if (SELECTED_BUTTON == 2 && IsButtonInputValid() != 0)
                goto sel1_2;
            goto chk1_3;
        sel1_2:
            if (SELECTED_BUTTON == 2)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 9;
            func_02012790(0x54);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk1_3:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x98) < 0x20)
                goto sel1_3;
            if (SELECTED_BUTTON == 3 && IsButtonInputValid() != 0)
                goto sel1_3;
            goto chk1_arrows;
        sel1_3:
            if (SELECTED_BUTTON == 3)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 3;
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            NEXT_PAUSE_MENU_ID = 0xc;
            func_02012790(0x55);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x28) < 0xb0)
                return;
            return;
        }
    chk1_arrows:
        if (COURSE_ARROW_BUTTONS_INVISIBLE != 0)
            return;
        {
            register u8 *de8_sb;
            var_sl = 0;
            var_fp = var_sl;
            (void)(u32)&SELECTED_ARROW;
            (void)(u32)&TOUCH_INPUT_ARR;
            (void)(u32)&MENU_DISPLAYED_COURSE;
            sp4 = var_sl;
            sp8 = var_sl;
            sp0 = var_sl;
            sp10 = 0x52;
            spC = 0x51;
            do {
                s32 de_off;
                u8 sl2;
                u8 a;
                u8 vx;
                var_r0 = sp0;
                sl2 = data_020a0e40;
                de_off = sl2 * 4;
                a = TOUCH_INPUT_ARR[sl2 * 4];
                if ((a != 0) && (DE8P(de_off)[1] != 0)) {
                    var_r0 = 1;
                }
                if ((var_r0 != 0) && ((u32)(vx = DE8P(sl2 * 4)[2]) < 0x38U) && ((u32)DE8P(sl2 * 4)[3] < 0x20U)) {
                    MENU_DISPLAYED_COURSE = (u8)(MENU_DISPLAYED_COURSE - 1);
                    SELECTED_ARROW = 1;
                    var_fp = 1;
                    if (((u32)vx & 0xffU) < 0x38U) {
                        if (MENU_DISPLAYED_COURSE == 0xFF) {
                            MENU_DISPLAYED_COURSE = 0xF;
                        }
                    }
                    ARROW_TIMER = (u8)(GAME_SPEED_RELATED * 3);
                } else {
                    s32 var_r0_2;
                    if ((a != 0) && (DE8P(sl2 * 4)[1] != 0)) {
                        var_r0_2 = 1;
                    } else {
                        var_r0_2 = sp4;
                    }
                    if (var_r0_2 != 0) {
                        if (((u32)(u8)(DE8P(sl2 * 4)[2] - 0xC8) < 0x38U) && ((u32)DE8P(sl2 * 4)[3] < 0x20U)) {
                            MENU_DISPLAYED_COURSE = (u8)(MENU_DISPLAYED_COURSE + 1);
                            SELECTED_ARROW = 2;
                            var_fp = 1;
                            if (MENU_DISPLAYED_COURSE == 0x10) {
                                MENU_DISPLAYED_COURSE = (u8)sp8;
                            }
                            ARROW_TIMER = (u8)(GAME_SPEED_RELATED * 3);
                        }
                    }
                }
                if ((MENU_DISPLAYED_COURSE == 0xF) || (CountStarsCollectedInLevelToDisplay(MENU_DISPLAYED_COURSE) != 0)) {
                    var_sl = 1;
                    if (SELECTED_ARROW == 1) {
                        func_02012790(spC);
                    } else if (SELECTED_ARROW == 2) {
                        func_02012790(sp10);
                    }
                }
            } while (var_sl == 0);
        }
        if (var_fp == 0)
            return;
        _ZN7Message11DisplayTextEt(data_020756d0[MENU_DISPLAYED_COURSE]);
        return;
    }
    case 2: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000a) = (REG16(0x400000a) & 0x43) | 0x1210;
        NEXT_PAUSE_MENU_ID = 3;
        MENU_DISPLAYED_COURSE = SublevelToLevel(LEVEL_OF_LAST_COLLECTED_STAR);
        _ZN7Message16DisplayPauseTextEth(0x277, MENU_DISPLAYED_COURSE);
        BG1_BUTTON_TILE_OFFSETS[0] = 0x60;
        BG1_BUTTON_TILE_OFFSETS[1] = 0x100;
        BG1_BUTTON_TILE_OFFSETS[2] = 0x1a0;
        BG1_BUTTON_TILE_OFFSETS[3] = 0x240;
        NUM_BIG_BUTTONS = 4;
        SELECTED_BUTTON = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        SetBg3Offset(0, 0);
        TOP_SCREEN_RELATED |= 8;
        if (func_02029408() != 0 && data_020756d0[MENU_DISPLAYED_COURSE] != 0) {
            SetBg2Offset(0, 0);
            SetSubBg0Offset(0, 0);
            SetSubBg1Offset(0, 0);
            TOP_SCREEN_RELATED |= 4;
            BOTTOM_SCREEN_RELATED |= 3;
            return;
        }
        NEXT_PAUSE_MENU_ID = 0xb;
        return;
    }
    case 3: {
        if (STAGE_TIMER != 0)
            return;
        {
            int touched = 0;
            u8 slot = data_020a0e40;
            if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                if (data_020a0de9[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x18) < 0x20)
                goto sel3_0;
            if (SELECTED_BUTTON == 0 && IsButtonInputValid() != 0)
                goto sel3_0;
            goto chk3_1;
        sel3_0:
            if (SELECTED_BUTTON == 0)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 0;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 2;
            func_02012790(3);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_1:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x40) < 0x20)
                goto sel3_1;
            if (SELECTED_BUTTON == 1 && IsButtonInputValid() != 0)
                goto sel3_1;
            goto chk3_2;
        sel3_1:
            if (SELECTED_BUTTON == 1)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 7;
            func_02012790(0x53);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_2:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x68) < 0x20)
                goto sel3_2;
            if (SELECTED_BUTTON == 2 && IsButtonInputValid() != 0)
                goto sel3_2;
            goto chk3_3;
        sel3_2:
            if (SELECTED_BUTTON == 2)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 9;
            func_02012790(0x54);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_3:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x90) < 0x20)
                goto sel3_3;
            if (SELECTED_BUTTON != 3)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel3_3:
            if (SELECTED_BUTTON == 3)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 0x13;
            func_02012790(0x56);
            return;
        }
    }
    case 4: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        NEXT_PAUSE_MENU_ID = 5;
        _ZN7Message18DisplayPauseTextVSEt(0);
        BG1_BUTTON_TILE_OFFSETS[0] = 0xa0;
        BG1_BUTTON_TILE_OFFSETS[1] = 0x140;
        BG1_BUTTON_TILE_OFFSETS[2] = 0x1e0;
        NUM_BIG_BUTTONS = 3;
        SELECTED_BUTTON = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        if (func_02029408() == 0) {
            NEXT_PAUSE_MENU_ID = 0xb;
            return;
        }
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        TOP_SCREEN_RELATED |= 4;
        TOP_SCREEN_RELATED &= ~8;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        BOTTOM_SCREEN_RELATED |= 3;
        return;
    }
    case 5: {
        if (STAGE_TIMER != 0)
            return;
        {
            int touched = 0;
            u8 slot = data_020a0e40;
            if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                if (data_020a0de9[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x28) < 0x20)
                goto sel5_0;
            if (SELECTED_BUTTON == 0 && IsButtonInputValid() != 0)
                goto sel5_0;
            goto chk5_1;
        sel5_0:
            if (SELECTED_BUTTON == 0)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 0;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            GAME_PAUSED = 2;
            func_02012790(3);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk5_1:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x50) < 0x20)
                goto sel5_1;
            if (SELECTED_BUTTON == 1 && IsButtonInputValid() != 0)
                goto sel5_1;
            goto chk5_2;
        sel5_1:
            if (SELECTED_BUTTON == 1)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 0x13;
            data_02111150 = 1;
            func_02012790(0x56);
            return;
        }
    chk5_2:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x78) < 0x20)
                goto sel5_2;
            if (SELECTED_BUTTON != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel5_2:
            if (SELECTED_BUTTON == 2)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 6;
            data_02111150 = 1;
            func_02012790(0x55);
            return;
        }
    }
    case 6: {
        _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        NEXT_ACTOR_UPDATE_FLAGS |= 0x40000000;
        ACTOR_UPDATE_FLAGS = NEXT_ACTOR_UPDATE_FLAGS;
        STOP_MUSIC_DURING_FADE = 1;
        _ZN5Stage10PS_CleanupEv();
        return;
    }
    case 7: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xf00;
        if (PAUSED_WITH_SELECT != 0) {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb001, _ZN3G2S12GetBG1ScrPtrEv());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac01, _ZN3G2S12GetBG1ScrPtrEv());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa801, _ZN3G2S12GetBG1ScrPtrEv());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa401, _ZN3G2S12GetBG1ScrPtrEv());
            } else {
                LoadCompressedFileAt(0xa001, _ZN3G2S12GetBG1ScrPtrEv());
            }
        } else {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb009, _ZN3G2S12GetBG1ScrPtrEv());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac09, _ZN3G2S12GetBG1ScrPtrEv());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa809, _ZN3G2S12GetBG1ScrPtrEv());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa409, _ZN3G2S12GetBG1ScrPtrEv());
            } else {
                LoadCompressedFileAt(0xa009, _ZN3G2S12GetBG1ScrPtrEv());
            }
        }
        BG1_BUTTON_TILE_OFFSETS[0] = 0x80;
        BG1_BUTTON_TILE_OFFSETS[1] = 0x120;
        BG1_BUTTON_TILE_OFFSETS[2] = 0x1c0;
        NUM_BIG_BUTTONS = 3;
        NEXT_PAUSE_MENU_ID = 8;
        {
            u8 cur = data_0209f4ae[data_020a0e40 * 0x18];
            SELECTED_CONTROLLER_MODE = cur;
            SELECTED_BUTTON = cur;
        }
        _ZN5Stage17UpdateMenuButtonsEb(0);
        _ZN7Message25DisplayControllerModeTextEt(0x280);
        REG16(0x400000c) = (((SELECTED_CONTROLLER_MODE + 0xd) << 8) | (REG16(0x400000c) & 0x43)) | 0x10;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        REG32(0x4001010) = 0;
        REG32(0x4001014) = 0;
        BACK_BUTTON_PRESSED = 0;
        BACK_BUTTON_PRESSED_2 = 0;
        _ZN5Stage25PS_UpdateOkAndBackButtonsEb(1);
        SetBg2Offset(0, 0);
        TOP_SCREEN_RELATED &= ~2;
        TOP_SCREEN_RELATED |= 4;
        TOP_SCREEN_RELATED &= ~8;
        BOTTOM_SCREEN_RELATED |= 3;
        return;
    }
    case 8: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        int idxa;
        if (STAGE_TIMER != 0)
            return;
        if (TOUCH_INPUT_ARR[data_020a0e40 * 4] == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        if (GetOwnerLanguage() == 5) {
            slot = data_020a0e40;
            firstw = 0x64;
            tx = data_020a0dea[slot * 4];
            relx = (u8)(tx - 0x44);
        } else if (GetOwnerLanguage() == 3) {
            slot = data_020a0e40;
            firstw = 0x74;
            tx = data_020a0dea[slot * 4];
            relx = (u8)(tx - 0x3c);
        } else {
            slot = data_020a0e40;
            firstw = 0x54;
            tx = data_020a0dea[slot * 4];
            relx = (u8)(tx - 0x4c);
        }
        idxa = slot * 4;
        ty = data_020a0deb[slot * 4];
        if ((u8)(ty - 0x1e) < 0x24) {
            if ((u8)(ty - 0x20) >= 0x20)
                return;
            if (SELECTED_BUTTON_SMALL == 1) {
                int t = 0;
                if (TOUCH_INPUT_ARR[idxa] != 0) {
                    if (data_020a0de9[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            SELECTED_BUTTON_SMALL = 1;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xd10;
            MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            {
                int t = 1;
                u8 a = TOUCH_INPUT_ARR[slot * 4];
                if (a == 0)
                    goto mode0_change;
                if (!(a != 0 && data_020a0de9[slot * 4] != 0))
                    t = 0;
                if (t == 0)
                    goto mode0_keep;
                if (SELECTED_CONTROLLER_MODE != 0)
                    goto mode0_keep;
            mode0_change:
                SELECTED_CONTROLLER_MODE = 0;
                SetControllerMode(0);
                PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
                func_02012790(0x116);
                GAME_PAUSED = 2;
                goto mode0_done;
            mode0_keep:
                SELECTED_CONTROLLER_MODE = 0;
                BACK_BUTTON_PRESSED_2 = 0;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                func_02012790(0x5b);
            mode0_done:
                SELECTED_BUTTON = 0;
                _ZN5Stage17UpdateMenuButtonsEb(0);
                return;
            }
        } else if ((u8)(ty - 0x46) < 0x24) {
            if ((u8)(ty - 0x48) >= 0x20)
                return;
            if (SELECTED_BUTTON_SMALL == 2) {
                int t = 0;
                if (TOUCH_INPUT_ARR[idxa] != 0) {
                    if (data_020a0de9[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            SELECTED_BUTTON_SMALL = 2;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xe10;
            MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            {
                int t;
                u8 a = TOUCH_INPUT_ARR[slot * 4];
                if (a == 0)
                    goto mode1_change;
                t = (a != 0 && data_020a0de9[slot * 4] != 0);
                if (t == 0)
                    goto mode1_keep;
                if (SELECTED_CONTROLLER_MODE != 1)
                    goto mode1_keep;
            mode1_change:
                SELECTED_CONTROLLER_MODE = 1;
                SetControllerMode(1);
                PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
                func_02012790(0x116);
                GAME_PAUSED = 2;
                goto mode1_done;
            mode1_keep:
                SELECTED_CONTROLLER_MODE = 1;
                BACK_BUTTON_PRESSED_2 = 0;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                func_02012790(0x5b);
            mode1_done:
                SELECTED_BUTTON = 1;
                _ZN5Stage17UpdateMenuButtonsEb(0);
                return;
            }
        } else if ((u8)(ty - 0x6e) < 0x24) {
            if ((u8)(ty - 0x70) >= 0x20)
                return;
            if (SELECTED_BUTTON_SMALL == 3) {
                int t = 0;
                if (TOUCH_INPUT_ARR[idxa] != 0) {
                    if (data_020a0de9[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            SELECTED_BUTTON_SMALL = 3;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xf10;
            MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            {
                int t;
                u8 a = TOUCH_INPUT_ARR[slot * 4];
                if (a == 0)
                    goto mode2_change;
                t = (a != 0 && data_020a0de9[slot * 4] != 0);
                if (t == 0)
                    goto mode2_keep;
                if (SELECTED_CONTROLLER_MODE != 2)
                    goto mode2_keep;
            mode2_change:
                SELECTED_CONTROLLER_MODE = 2;
                SetControllerMode(2);
                PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
                func_02012790(0x116);
                GAME_PAUSED = 2;
                goto mode2_done;
            mode2_keep:
                SELECTED_CONTROLLER_MODE = 2;
                BACK_BUTTON_PRESSED_2 = 0;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                func_02012790(0x5b);
            mode2_done:
                SELECTED_BUTTON = 2;
                _ZN5Stage17UpdateMenuButtonsEb(0);
                return;
            }
        } else {
            int t = 0;
            u8 a0 = TOUCH_INPUT_ARR[idxa];
            if (a0 != 0) {
                if (data_020a0de9[idxa] != 0)
                    t = 1;
            }
            if (t == 0)
                goto chk218;
            if (relx >= firstw)
                goto chk218;
            if ((u8)(ty - 0x98) < 0x20)
                goto okback;
        chk218:
            if (PAUSED_WITH_SELECT == 0) {
                int t2;
                t2 = (a0 != 0 && data_020a0de9[slot * 4] != 0);
                if (t2 == 0)
                    goto biv8;
                if ((u8)(tx - 0xd8) >= 0x20)
                    goto biv8;
                if ((u8)(ty - 0x98) < 0x20)
                    goto okback;
            }
        biv8:
            if (IsButtonInputValid() == 0)
                goto fail8;
        okback:
            if (relx < firstw) {
                u8 slot2 = data_020a0e40;
                if ((u8)(data_020a0deb[slot2 * 4] - 0x98) < 0x20 &&
                    IsButtonInputValid() != 0) {
                    BACK_BUTTON_PRESSED = 0;
                    goto okback_go;
                }
            }
            {
                u8 slot2 = data_020a0e40;
                if ((u8)(data_020a0dea[slot2 * 4] - 0xd8) < 0x20 &&
                    (u8)(data_020a0deb[slot2 * 4] - 0x98) < 0x20) {
                    BACK_BUTTON_PRESSED = 1;
                }
            }
        okback_go:
            SELECTED_BUTTON = 3;
            SetControllerMode(SELECTED_CONTROLLER_MODE);
            BACK_BUTTON_PRESSED_2 = 1;
            MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            if (BACK_BUTTON_PRESSED == 0) {
                GAME_PAUSED = 2;
                func_02012790(0x116);
                return;
            }
            func_02012790(0x5c);
            if (SublevelToLevel(LEVEL_ID) == 0x1d) {
                NEXT_PAUSE_MENU_ID = 0;
            } else {
                NEXT_PAUSE_MENU_ID = 2;
            }
            return;
        fail8:
            SELECTED_BUTTON_SMALL = 0;
            return;
        }
    }
    case 9: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1000;
        NEXT_PAUSE_MENU_ID = 0xa;
        SOUND_OPTION = GetSoundMode();
        BACKLIGHT_OFF = BACKLIGHT_ENABLED ^ 1;
        OPTIONS_MENU_OPENED = data_0208ee40;
        SELECTED_ARROW = 0;
        _ZN7Message22DisplayOptionsMenuTextEt((s16)(SOUND_OPTION + 0x284));
        NUM_BIG_BUTTONS = 2;
        SELECTED_BUTTON = 0;
        _ZN5Stage20PS_UpdateOptionsMenuEv();
        BACK_BUTTON_PRESSED = 0;
        BACK_BUTTON_PRESSED_2 = 0;
        _ZN5Stage25PS_UpdateOkAndBackButtonsEb(1);
        if (SublevelToLevel(LEVEL_ID) == 0x1d) {
            TOP_SCREEN_RELATED &= ~2;
        } else {
            TOP_SCREEN_RELATED &= ~4;
        }
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        return;
    }
    case 0xa: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        if (TOUCH_INPUT_ARR[data_020a0e40 * 4] == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        if (GetOwnerLanguage() == 5) {
            slot = data_020a0e40;
            firstw = 0x70;
            tx = data_020a0dea[slot * 4];
            relx = (u8)(tx - 0x40);
        } else if (GetOwnerLanguage() == 3) {
            slot = data_020a0e40;
            firstw = 0x60;
            tx = data_020a0dea[slot * 4];
            relx = (u8)(tx - 0x48);
        } else {
            slot = data_020a0e40;
            firstw = 0x50;
            tx = data_020a0dea[slot * 4];
            relx = (u8)(tx - 0x50);
        }
        if ((u8)(tx - 0x5a) < 0x14) {
            ty = data_020a0deb[slot * 4];
            if ((u8)(ty - 0x2e) < 0x14) {
                if ((u8)(tx - 0x5c) >= 0x10)
                    return;
                if ((u8)(ty - 0x30) >= 0x10)
                    return;
                if (SELECTED_BUTTON_SMALL == 1) {
                    int t = 0;
                    if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                        if (data_020a0de9[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                SELECTED_BUTTON_SMALL = 1;
                SOUND_OPTION = SOUND_OPTION - 1;
                if (SOUND_OPTION == 0xff)
                    SOUND_OPTION = 2;
                SELECTED_ARROW = 2;
                ARROW_TIMER = GAME_SPEED_RELATED * 3;
                SELECTED_BUTTON = 0;
                _ZN7Message22DisplayOptionsMenuTextEt((s16)(SOUND_OPTION + 0x284));
                SOUND_MODE_TIMER = 8;
                _ZN5Stage20PS_UpdateOptionsMenuEv();
                SetSoundMode(SOUND_OPTION);
                func_02012790(0x64);
                if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x5c) < 0x10)
                    return;
                return;
            }
        }
        {
            u8 ty2;
            int t6;
            int tmpv;
            if ((u8)(tx - 0xea) < 0x14) {
                ty2 = data_020a0deb[slot * 4];
                if ((u8)(ty2 - 0x2e) < 0x14)
                    goto snd_next;
            }
            if ((u8)(tx - 0x6e) < 0x7c) {
                ty2 = data_020a0deb[slot * 4];
                if ((u8)(ty2 - 0x26) < 0x24)
                    goto snd_next;
            }
            t6 = tx - 6;
            if ((u8)t6 < 0x3c) {
                ty2 = data_020a0deb[slot * 4];
                if ((u8)(ty2 - 0x26) < 0x24)
                    goto snd_next;
            }
            goto backlight_on;
        snd_next:
            if ((u8)(tx - 0xec) < 0x10) {
                if ((u8)(ty2 - 0x30) < 0x10)
                    goto snd_next_go;
            }
            if ((u8)(tx - 0x70) < 0x78) {
                if ((u8)(ty2 - 0x28) < 0x20)
                    goto snd_next_go;
            }
            if ((u8)(tx - 8) >= 0x38)
                return;
            if ((u8)(ty2 - 0x28) >= 0x20)
                return;
        snd_next_go:
            if (SELECTED_BUTTON_SMALL == 2) {
                int t = 0;
                if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                    if (data_020a0de9[slot * 4] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            SELECTED_BUTTON_SMALL = 2;
            SOUND_OPTION = SOUND_OPTION + 1;
            if (SOUND_OPTION == 3)
                SOUND_OPTION = 0;
            SELECTED_ARROW = 1;
            ARROW_TIMER = GAME_SPEED_RELATED * 3;
            SELECTED_BUTTON = 0;
            SetSoundMode(SOUND_OPTION);
            func_02012790(0x64);
            SOUND_MODE_TIMER = 8;
            _ZN5Stage20PS_UpdateOptionsMenuEv();
            _ZN7Message22DisplayOptionsMenuTextEt((s16)(SOUND_OPTION + 0x284));
            {
                u8 s3 = data_020a0e40;
                u8 x3 = data_020a0dea[s3 * 4];
                if ((u8)(x3 - 0xec) < 0x10) {
                    if ((u8)(data_020a0deb[s3 * 4] - 0x28) < 0x10)
                        return;
                }
                if ((u8)(x3 - 0x70) < 0x78) {
                    if ((u8)(data_020a0deb[s3 * 4] - 0x20) < 0x20)
                        return;
                }
                if ((u8)(x3 - 8) < 0x38)
                    return;
                return;
            }
        backlight_on:
            if ((u8)t6 < 0x3c) {
                u8 ty2b = data_020a0deb[slot * 4];
                if ((u8)(ty2b - 0x56) < 0x24) {
                tmpv = tx - 0x70;
                if ((u8)tmpv >= 0x38)
                    return;
                tmpv = ty2b - 0x58;
                if ((u8)tmpv >= 0x20)
                    return;
                if (SELECTED_BUTTON_SMALL == 3) {
                    int t = 0;
                    if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                        if (data_020a0de9[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                SELECTED_BUTTON_SMALL = 3;
                if (BACKLIGHT_OFF != 0) {
                    func_02012790(0x66);
                    TurnBacklightOn();
                } else {
                    BACKLIGHT_OPTION_TIMER = GAME_SPEED_RELATED << 2;
                    func_02012790(0x67);
                }
                BACKLIGHT_OFF = 0;
                SELECTED_BUTTON = 1;
                _ZN5Stage20PS_UpdateOptionsMenuEv();
                if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x70) < 0x38)
                    return;
                return;
                }
            }
        chk_bl_off:
            if ((u8)(tx - 0xae) < 0x3c) {
                u8 ty3 = data_020a0deb[slot * 4];
                if ((u8)(ty3 - 0x56) < 0x24) {
                    tmpv = tx - 0xb0;
                    if ((u8)tmpv >= 0x38)
                        return;
                    tmpv = ty3 - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (SELECTED_BUTTON_SMALL == 4) {
                        int t = 0;
                        if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                            if (data_020a0de9[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    SELECTED_BUTTON_SMALL = 4;
                    if (BACKLIGHT_OFF == 0) {
                        func_02012790(0x66);
                        TurnBacklightOff();
                    } else {
                        BACKLIGHT_OPTION_TIMER = GAME_SPEED_RELATED << 2;
                        func_02012790(0x67);
                    }
                    BACKLIGHT_OFF = 1;
                    SELECTED_BUTTON = 1;
                    _ZN5Stage20PS_UpdateOptionsMenuEv();
                    if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0xb0) < 0x38)
                        return;
                    return;
                }
            }
        chk_bl_off2:
            if ((u8)tmpv < 0x4c) {
                u8 ty4 = data_020a0deb[slot * 4];
                if ((u8)(ty4 - 0x56) < 0x24) {
                    tmpv = tx - 8;
                    if ((u8)tmpv >= 0x48)
                        return;
                    tmpv = ty4 - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (SELECTED_BUTTON_SMALL == 5) {
                        int t = 0;
                        if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                            if (data_020a0de9[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    SELECTED_BUTTON_SMALL = 5;
                    if (SELECTED_BUTTON != 1) {
                        SELECTED_BUTTON = 1;
                        _ZN5Stage20PS_UpdateOptionsMenuEv();
                    }
                    BACKLIGHT_OFF = BACKLIGHT_OFF ^ 1;
                    if (BACKLIGHT_OFF == 0) {
                        TurnBacklightOn();
                    } else {
                        TurnBacklightOff();
                    }
                    BACKLIGHT_OPTION_TIMER = GAME_SPEED_RELATED << 2;
                    _ZN5Stage20PS_UpdateOptionsMenuEv();
                    func_02012790(0x66);
                    return;
                }
            }
            {
                int t = 0;
                if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                    if (data_020a0de9[slot * 4] != 0)
                        t = 1;
                }
                if (t == 0)
                    goto biv_a;
                if (relx >= firstw)
                    goto chk_dx;
                if ((u8)(data_020a0deb[slot * 4] - 0x98) < 0x20)
                    goto opt_okback;
            chk_dx:
                if ((u8)(tx - 0xd8) < 0x20) {
                    if ((u8)(data_020a0deb[slot * 4] - 0x98) < 0x20)
                        goto opt_okback;
                }
            biv_a:
                if (IsButtonInputValid() == 0)
                    return;
            opt_okback:
                if (relx < firstw) {
                    u8 s4 = data_020a0e40;
                    if ((u8)(data_020a0deb[s4 * 4] - 0x98) < 0x20)
                        goto set29c;
                }
                if (IsButtonInputValid() != 0) {
                set29c:
                    BACK_BUTTON_PRESSED = 0;
                    goto opt_go;
                }
                {
                    u8 s4 = data_020a0e40;
                    if ((u8)(data_020a0dea[s4 * 4] - 0xd8) < 0x20 &&
                        (u8)(data_020a0deb[s4 * 4] - 0x98) < 0x20) {
                        BACK_BUTTON_PRESSED = 1;
                    }
                }
            opt_go:
                SELECTED_BUTTON = 2;
                PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
                _ZN5Stage20PS_UpdateOptionsMenuEv();
                BACK_BUTTON_PRESSED_2 = 1;
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                if (BACK_BUTTON_PRESSED == 0) {
                    SetSoundMode(SOUND_OPTION);
                    if (OPTIONS_MENU_OPENED != 0) {
                        data_0208ee40 = 1;
                    } else {
                        data_0208ee40 = 0;
                    }
                    GAME_PAUSED = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5d);
                if (SublevelToLevel(LEVEL_ID) == 0x1d) {
                    NEXT_PAUSE_MENU_ID = 0;
                } else {
                    NEXT_PAUSE_MENU_ID = 2;
                }
                return;
            }
        }
    }
    case 0xb: {
        if (IsButtonInputValid() == 0)
            return;
        MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
        GAME_PAUSED = 2;
        func_02012790(3);
        return;
    }
    case 0xc: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        _ZN7Message19DisplaySaveMenuTextEt(0x290);
        BG1_BUTTON_TILE_OFFSETS[0] = 0xa0;
        BG1_BUTTON_TILE_OFFSETS[1] = 0x140;
        BG1_BUTTON_TILE_OFFSETS[2] = 0x1e0;
        NUM_BIG_BUTTONS = 3;
        SELECTED_BUTTON = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        NEXT_PAUSE_MENU_ID = 0xd;
        return;
    }
    case 0xd: {
        {
            int touched = 0;
            u8 slot = data_020a0e40;
            if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                if (data_020a0de9[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x28) < 0x20)
                goto seld_0;
            if (SELECTED_BUTTON == 0 && IsButtonInputValid() != 0)
                goto seld_0;
            goto chkd_1;
        seld_0:
            if (SELECTED_BUTTON == 0)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 0;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            GAME_PAUSED = 2;
            func_02012790(3);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chkd_1:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x50) < 0x20)
                goto seld_1;
            if (SELECTED_BUTTON == 1 && IsButtonInputValid() != 0)
                goto seld_1;
            goto chkd_2;
        seld_1:
            if (SELECTED_BUTTON == 1)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 0xe;
            func_02012790(0x5e);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chkd_2:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x78) < 0x20)
                goto seld_2;
            if (SELECTED_BUTTON != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        seld_2:
            if (SELECTED_BUTTON == 2)
                MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
            SELECTED_BUTTON = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
            NEXT_PAUSE_MENU_ID = 0x10;
            func_02012790(0x5f);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 0xe: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        NEXT_PAUSE_MENU_ID = 0xf;
        _ZN7Message21DisplaySaveStatusTextEt(0x295);
        TOP_SCREEN_RELATED &= ~2;
        SAVE_MENU_TIMER = 0x78;
        return;
    }
    case 0xf: {
        if (SAVE_MENU_TIMER == 0)
            return;
        if (SAVE_MENU_TIMER == 0x78)
            _ZN8SaveData15SaveCurrentFileEv();
        if (SAVE_MENU_TIMER == 0x3c)
            _ZN7Message21DisplaySaveStatusTextEt(0x296);
        SAVE_MENU_TIMER = SAVE_MENU_TIMER - GAME_SPEED_RELATED;
        if (SAVE_MENU_TIMER != 0)
            return;
        if (SELECTED_BUTTON != 1) {
            GAME_PAUSED = 2;
            return;
        }
        _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        STOP_MUSIC_DURING_FADE = 1;
        _ZN5Stage10PS_CleanupEv();
        NEXT_ACTOR_UPDATE_FLAGS |= 0x40000000;
        ACTOR_UPDATE_FLAGS = NEXT_ACTOR_UPDATE_FLAGS;
        _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
        return;
    }
    case 0x10: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1100;
        NEXT_PAUSE_MENU_ID = 0x11;
        _ZN7Message19DisplayDontSaveTextEt(0x297);
        SELECTED_BUTTON = 1;
        _ZN5Stage17PS_UpdateSaveMenuEb(1);
        return;
    }
    case 0x11: {
        {
            int touched = 0;
            u8 slot = data_020a0e40;
            if (TOUCH_INPUT_ARR[slot * 4] != 0) {
                if (data_020a0de9[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            int rely;
            int rel;
            u8 slot = data_020a0e40;
            u8 tx = data_020a0dea[slot * 4];
            rel = tx - 0x28;
            if ((u8)rel < 0x50) {
                rely = data_020a0deb[slot * 4] - 0x98;
                if ((u8)rely < 0x20) {
                    if (SELECTED_BUTTON == 0)
                        MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
                        NEXT_PAUSE_MENU_ID = 0x12;
                        func_02012790(0x60);
                    } else {
                        func_02012790(0x51);
                    }
                    SELECTED_BUTTON = 0;
                    _ZN5Stage17PS_UpdateSaveMenuEb(0);
                    return;
                }
            }
            {
                rel = tx - 0x88;
                if ((u8)rel >= 0x50)
                    return;
                {
                    rely = data_020a0deb[slot * 4] - 0x98;
                    if ((u8)rely >= 0x20)
                        return;
                    if (SELECTED_BUTTON == 1)
                        MENU_CHANGE_TIMER = GAME_SPEED_RELATED << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        PAUSE_MENU_TIMER = GAME_SPEED_RELATED << 3;
                        NEXT_PAUSE_MENU_ID = 0x12;
                        func_02012790(3);
                    } else {
                        func_02012790(0x52);
                    }
                    SELECTED_BUTTON = 1;
                    _ZN5Stage17PS_UpdateSaveMenuEb(0);
                    return;
                }
            }
        }
    }
    case 0x12: {
        if (SELECTED_BUTTON != 0) {
            GAME_PAUSED = 2;
            return;
        }
        _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        STOP_MUSIC_DURING_FADE = 1;
        _ZN5Stage10PS_CleanupEv();
        NEXT_ACTOR_UPDATE_FLAGS |= 0x40000000;
        ACTOR_UPDATE_FLAGS = NEXT_ACTOR_UPDATE_FLAGS;
        return;
    }
    case 0x13: {
        int t = (CURRENT_GAMEMODE == 1);
        if (t != false && DP_STATE == 0) {
            _ZN5Scene14StartSceneFadeEjjt(6, 1, 0);
            STOP_MUSIC_DURING_FADE = 1;
            _ZN5Stage10PS_CleanupEv();
            NEXT_ACTOR_UPDATE_FLAGS |= 0x40000000;
            ACTOR_UPDATE_FLAGS = NEXT_ACTOR_UPDATE_FLAGS;
            return;
        }
        LoadLevelNoReturn(2, 0, -1, 0);
        NEXT_ACTOR_UPDATE_FLAGS |= 0x40000000;
        ACTOR_UPDATE_FLAGS = NEXT_ACTOR_UPDATE_FLAGS;
        STOP_MUSIC_DURING_FADE = 1;
        GAME_PAUSED = 2;
        return;
    }
    default:
        return;
    }
}
}
