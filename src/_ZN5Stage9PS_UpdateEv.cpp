//cpp
// NONMATCHING: 16/20 switch cases byte-identical (jump-table-anchored region compare, total region div=690); global aligned div=1239 - the global aligner is anchor-sensitive across the residual -0x9c size skew, the region measure is the honest one. Residuals: case1 star-scroll loop (-11 insn), case8/0xa language-arm lsl hoist + mode-block index rematerialization (-5/-13 insn), case7 d45c/d454 RMW pair coloring (8), case0x11 whole-block coloring (41), and case0xa's mid-case 47-word literal pool which cannot align until all preceding sizes are exact.
/* Stage::PS_Update at 0x0202635c (arm9), size 0x30ac (12,460 bytes, 3115 insns)
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 *
 * Full decompilation of the pause-screen state machine: 6 timer blocks +
 * a 20-case switch on data_0209f248. Byte-identical regions: prologue,
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

extern int data_0208ee44;          /* frame tick */
extern u8 data_0208ee3c;
extern u8 data_0208ee40;
extern s8 data_02092124;
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_0209f1ec;           /* next pause state */
extern u8 data_0209f1fc;
extern u8 data_0209f210;           /* timer: star scroll */
extern u8 data_0209f218;
extern u8 data_0209f22c;           /* timer: whole update pause */
extern u8 data_0209f234;           /* sound mode selection */
extern u8 data_0209f238;
extern u8 data_0209f23c;           /* timer -> options */
extern u8 data_0209f240;
extern u8 data_0209f244;           /* timer: button flash */
extern u8 data_0209f248;           /* current pause state */
extern u8 data_0209f250;
extern u8 data_0209f280;
extern u8 data_0209f29c;
extern u8 data_0209f2a4;
extern u8 data_0209f2b4;
extern u8 data_0209f2b8;           /* save countdown */
extern u8 data_0209f2c4;
extern u8 data_0209f2c8;           /* displayed level */
extern u8 data_0209f2cc;           /* timer -> options */
extern u8 data_0209f2d8;
extern u8 data_0209f2dc;
extern u8 data_0209f2e0;           /* selected button */
extern u8 data_0209f2e4;           /* timer -> options */
extern u8 data_0209f2ec;
extern u8 data_0209f2f0;
extern s8 data_0209f2f8;
extern u16 data_0209f300;
extern u16 data_0209f360[];
extern u8 data_0209f4ae[];
extern int data_0209fc68;
extern int data_0209b454;
extern int data_0209b464;
extern s16 data_020756d0[];
extern u8 data_02111150;
extern volatile u8 data_020a0e40;  /* active input slot */
extern u8 data_020a0de8[]; /* touch: active   [slot*4]   */
extern u8 data_020a0de9[]; /* touch: pressed  [slot*4]   */
extern volatile u8 data_020a0dea[]; /* touch: x        [slot*4]   */
extern volatile u8 data_020a0deb[]; /* touch: y        [slot*4]   */

#define REG16(a) (*(volatile u16 *)(a))
#define REG32(a) (*(volatile u32 *)(a))

struct Stage {
    static void PS_Update();
};

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

    if (data_0209f210 != 0) {
        data_0209f210 = data_0209f210 - data_0208ee44;
        if (data_0209f210 == 0)
            data_0209f238 = 0;
    }
    if (data_0209f2e4 != 0) {
        data_0209f2e4 = data_0209f2e4 - data_0208ee44;
        if (data_0209f2e4 == 0)
            _ZN5Stage20PS_UpdateOptionsMenuEv();
    }
    if (data_0209f244 != 0) {
        data_0209f244 = data_0209f244 - data_0208ee44;
        if (data_0209f244 == 0) {
            u8 st = data_0209f248;
            if (st == 8 || st == 0xa) {
                if (data_0209f240 != 0) {
                    data_0209f240 = 0;
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
    if (data_0209f2cc != 0) {
        data_0209f2cc = data_0209f2cc - data_0208ee44;
        if (data_0209f2cc == 0)
            _ZN5Stage20PS_UpdateOptionsMenuEv();
    }
    if (data_0209f23c != 0) {
        data_0209f23c = data_0209f23c - data_0208ee44;
        if (data_0209f23c == 0)
            _ZN5Stage20PS_UpdateOptionsMenuEv();
    }
    if (data_0209f22c != 0) {
        data_0209f22c = data_0209f22c - data_0208ee44;
        return;
    }

    if (data_0209f248 != data_0209f1ec)
        data_0209f248 = data_0209f1ec;

    switch (data_0209f248) {
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
            data_0209f2c8 = i;
            data_0209f2f0 = 1;
        } else {
            if (SublevelToLevel(data_02092124) >= 0xf) {
                data_0209f2c8 = 0xf;
            } else {
                data_0209f2c8 = SublevelToLevel(data_02092124);
            }
            data_0209f2f0 = 0;
        }
        _ZN7Message7DisplayEj(data_020756d0[data_0209f2c8]);
        data_0209f360[0] = 0x80;
        data_0209f360[1] = 0x120;
        data_0209f360[2] = 0x1c0;
        data_0209f360[3] = 0x260;
        data_0209f2b4 = 4;
        data_0209f2e0 = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        data_0209f240 = 0;
        _ZN5Stage25PS_UpdateOkAndBackButtonsEb(1);
        SetBg1Offset(0, 0);
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d45c |= 0xe;
        data_0209d454 |= 3;
        data_0209f1ec = 1;
        return;
    }
    case 1: {
        if (data_0209f300 != 0)
            return;
        {
            u8 slot = data_020a0e40;
            int touched = 0;
            if (data_020a0de8[slot * 4] != 0 && data_020a0de9[slot * 4] != 0)
                touched = 1;
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
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel1_0;
            goto chk1_1;
        sel1_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            data_0209f22c = data_0208ee44 << 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f2c4 = 2;
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
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel1_1;
            goto chk1_2;
        sel1_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 7;
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
            if (data_0209f2e0 == 2 && IsButtonInputValid() != 0)
                goto sel1_2;
            goto chk1_3;
        sel1_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 9;
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
            if (data_0209f2e0 == 3 && IsButtonInputValid() != 0)
                goto sel1_3;
            goto chk1_arrows;
        sel1_3:
            if (data_0209f2e0 == 3)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 3;
            data_0209f22c = data_0208ee44 << 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f1ec = 0xc;
            func_02012790(0x55);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x28) < 0xb0)
                return;
            return;
        }
    chk1_arrows:
        if (data_0209f2f0 != 0)
            return;
        {
            register u8 *de8_sb;
            var_sl = 0;
            var_fp = var_sl;
            (void)(u32)&data_0209f2c8;
            (void)(u32)&data_020a0de8;
            (void)(u32)&data_0209f238;
            sp4 = var_sl;
            sp8 = var_sl;
            sp0 = var_sl;
            sp10 = 0x52;
            spC = 0x51;
            do {
                s32 de_off;
                var_r0 = sp0;
                de_off = data_020a0e40 * 4;
                if ((data_020a0de8[de_off] != 0) && (data_020a0de8[de_off + 1] != 0)) {
                    var_r0 = 1;
                }
                if ((var_r0 != 0) && ((u32)data_020a0de8[de_off + 2] < 0x38U) && ((u32)data_020a0de8[de_off + 3] < 0x20U)) {
                    data_0209f238 = 1;
                    var_fp = 1;
                    data_0209f2c8 = (u8)(data_0209f2c8 - 1);
                    if (data_0209f2c8 == 0xFF) {
                        data_0209f2c8 = 0xF;
                    }
                    data_0209f210 = (u8)(data_0208ee44 * 3);
                } else {
                    s32 var_r0_2;
                    if ((data_020a0de8[de_off] != 0) && (data_020a0de8[de_off + 1] != 0)) {
                        var_r0_2 = 1;
                    } else {
                        var_r0_2 = sp4;
                    }
                    if (var_r0_2 != 0) {
                        if (((u32)(u8)(data_020a0de8[de_off + 2] - 0xC8) < 0x38U) && ((u32)data_020a0de8[de_off + 3] < 0x20U)) {
                            data_0209f238 = 2;
                            var_fp = 1;
                            data_0209f2c8 = (u8)(data_0209f2c8 + 1);
                            if (data_0209f2c8 == 0x10) {
                                data_0209f2c8 = (u8)sp8;
                            }
                            data_0209f210 = (u8)(data_0208ee44 * 3);
                        }
                    }
                }
                if ((data_0209f2c8 == 0xF) || (CountStarsCollectedInLevelToDisplay(data_0209f2c8) != 0)) {
                    var_sl = 1;
                    if (data_0209f238 == 1) {
                        func_02012790(spC);
                    } else if (data_0209f238 == 2) {
                        func_02012790(sp10);
                    }
                }
            } while (var_sl == 0);
        }
        if (var_fp == 0)
            return;
        _ZN7Message11DisplayTextEt(data_020756d0[data_0209f2c8]);
        return;
    }
    case 2: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000a) = (REG16(0x400000a) & 0x43) | 0x1210;
        data_0209f1ec = 3;
        data_0209f2c8 = SublevelToLevel(data_02092124);
        _ZN7Message16DisplayPauseTextEth(0x277, data_0209f2c8);
        data_0209f360[0] = 0x60;
        data_0209f360[1] = 0x100;
        data_0209f360[2] = 0x1a0;
        data_0209f360[3] = 0x240;
        data_0209f2b4 = 4;
        data_0209f2e0 = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        SetBg3Offset(0, 0);
        data_0209d45c |= 8;
        if (func_02029408() != 0 && data_020756d0[data_0209f2c8] != 0) {
            SetBg2Offset(0, 0);
            SetSubBg0Offset(0, 0);
            SetSubBg1Offset(0, 0);
            data_0209d45c |= 4;
            data_0209d454 |= 3;
            return;
        }
        data_0209f1ec = 0xb;
        return;
    }
    case 3: {
        if (data_0209f300 != 0)
            return;
        {
            u8 slot = data_020a0e40;
            int touched = 0;
            if (data_020a0de8[slot * 4] != 0 && data_020a0de9[slot * 4] != 0)
                touched = 1;
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
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel3_0;
            goto chk3_1;
        sel3_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 2;
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
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel3_1;
            goto chk3_2;
        sel3_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 7;
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
            if (data_0209f2e0 == 2 && IsButtonInputValid() != 0)
                goto sel3_2;
            goto chk3_3;
        sel3_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 9;
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
            if (data_0209f2e0 != 3)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel3_3:
            if (data_0209f2e0 == 3)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 3;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x13;
            func_02012790(0x56);
            return;
        }
    }
    case 4: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        data_0209f1ec = 5;
        _ZN7Message18DisplayPauseTextVSEt(0);
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2e0 = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        if (func_02029408() == 0) {
            data_0209f1ec = 0xb;
            return;
        }
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d45c &= ~8;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d454 |= 3;
        return;
    }
    case 5: {
        if (data_0209f300 != 0)
            return;
        {
            u8 slot = data_020a0e40;
            int touched = 0;
            if (data_020a0de8[slot * 4] != 0 && data_020a0de9[slot * 4] != 0)
                touched = 1;
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
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel5_0;
            goto chk5_1;
        sel5_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f2c4 = 2;
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
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel5_1;
            goto chk5_2;
        sel5_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x13;
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
            if (data_0209f2e0 != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel5_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 6;
            data_02111150 = 1;
            func_02012790(0x55);
            return;
        }
    }
    case 6: {
        _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        data_0209f280 = 1;
        _ZN5Stage10PS_CleanupEv();
        return;
    }
    case 7: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xf00;
        if (data_0209f218 != 0) {
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
        data_0209f360[0] = 0x80;
        data_0209f360[1] = 0x120;
        data_0209f360[2] = 0x1c0;
        data_0209f2b4 = 3;
        data_0209f1ec = 8;
        {
            u8 cur = data_0209f4ae[data_020a0e40 * 0x18];
            data_0209f2dc = cur;
            data_0209f2e0 = cur;
        }
        _ZN5Stage17UpdateMenuButtonsEb(0);
        _ZN7Message25DisplayControllerModeTextEt(0x280);
        REG16(0x400000c) = (((data_0209f2dc + 0xd) << 8) | (REG16(0x400000c) & 0x43)) | 0x10;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        REG32(0x4001010) = 0;
        REG32(0x4001014) = 0;
        data_0209f29c = 0;
        data_0209f240 = 0;
        _ZN5Stage25PS_UpdateOkAndBackButtonsEb(1);
        SetBg2Offset(0, 0);
        data_0209d45c &= ~2;
        data_0209d454 |= 3;
        data_0209d45c |= 4;
        data_0209d45c &= ~8;
        return;
    }
    case 8: {
        u8 slot;
        u8 tx;
        u8 relx;
        u8 firstw;
        u8 ty;
        int idx;
        if (data_0209f300 != 0)
            return;
        if (data_020a0de8[data_020a0e40 * 4] == 0) {
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
        ty = data_020a0deb[slot * 4];
        idx = slot * 4;
        if ((u8)(ty - 0x1e) < 0x24) {
            if ((u8)(ty - 0x20) >= 0x20)
                return;
            if (data_0209f2a4 == 1) {
                int t = 0;
                if (data_020a0de8[slot * 4] != 0) {
                    if (data_020a0de9[idx] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 1;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xd10;
            data_0209f244 = data_0208ee44 << 2;
            {
                int t = 1;
                u8 a = data_020a0de8[slot * 4];
                if (a == 0)
                    goto mode0_change;
                if (!(a != 0 && data_020a0de9[idx] != 0))
                    t = 0;
                if (t == 0)
                    goto mode0_keep;
                if (data_0209f2dc != 0)
                    goto mode0_keep;
            mode0_change:
                data_0209f2dc = 0;
                SetControllerMode(0);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x116);
                data_0209f2c4 = 2;
                goto mode0_done;
            mode0_keep:
                data_0209f2dc = 0;
                data_0209f240 = 0;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                func_02012790(0x5b);
            mode0_done:
                data_0209f2e0 = 0;
                _ZN5Stage17UpdateMenuButtonsEb(0);
                return;
            }
        } else if ((u8)(ty - 0x46) < 0x24) {
            if ((u8)(ty - 0x48) >= 0x20)
                return;
            if (data_0209f2a4 == 2) {
                int t = 0;
                if (data_020a0de8[slot * 4] != 0) {
                    if (data_020a0de9[idx] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 2;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xe10;
            data_0209f244 = data_0208ee44 << 2;
            {
                int t = 1;
                u8 a = data_020a0de8[slot * 4];
                if (a == 0)
                    goto mode1_change;
                if (!(a != 0 && data_020a0de9[idx] != 0))
                    t = 0;
                if (t == 0)
                    goto mode1_keep;
                if (data_0209f2dc != 1)
                    goto mode1_keep;
            mode1_change:
                data_0209f2dc = 1;
                SetControllerMode(1);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x116);
                data_0209f2c4 = 2;
                goto mode1_done;
            mode1_keep:
                data_0209f2dc = 1;
                data_0209f240 = 0;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                func_02012790(0x5b);
            mode1_done:
                data_0209f2e0 = 1;
                _ZN5Stage17UpdateMenuButtonsEb(0);
                return;
            }
        } else if ((u8)(ty - 0x6e) < 0x24) {
            if ((u8)(ty - 0x70) >= 0x20)
                return;
            if (data_0209f2a4 == 3) {
                int t = 0;
                if (data_020a0de8[slot * 4] != 0) {
                    if (data_020a0de9[idx] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 3;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xf10;
            data_0209f244 = data_0208ee44 << 2;
            {
                int t = 1;
                u8 a = data_020a0de8[slot * 4];
                if (a == 0)
                    goto mode2_change;
                if (!(a != 0 && data_020a0de9[idx] != 0))
                    t = 0;
                if (t == 0)
                    goto mode2_keep;
                if (data_0209f2dc != 2)
                    goto mode2_keep;
            mode2_change:
                data_0209f2dc = 2;
                SetControllerMode(2);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x116);
                data_0209f2c4 = 2;
                goto mode2_done;
            mode2_keep:
                data_0209f2dc = 2;
                data_0209f240 = 0;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                func_02012790(0x5b);
            mode2_done:
                data_0209f2e0 = 2;
                _ZN5Stage17UpdateMenuButtonsEb(0);
                return;
            }
        } else {
            u8 a0 = data_020a0de8[slot * 4];
            int t = 0;
            if (a0 != 0) {
                if (data_020a0de9[idx] != 0)
                    t = 1;
            }
            if (t != 0 && relx < firstw && (u8)(ty - 0x98) < 0x20)
                goto okback;
            if (data_0209f218 == 0) {
                int t2;
                if (a0 == 0) {
                    t2 = 0;
                } else if (data_020a0de9[idx] != 0) {
                    t2 = 1;
                } else {
                    t2 = 0;
                }
                if (t2 != 0 &&
                    (u8)(tx - 0xd8) < 0x20 &&
                    (u8)(ty - 0x98) < 0x20)
                    goto okback;
            }
            if (IsButtonInputValid() == 0) {
                data_0209f2a4 = 0;
                return;
            }
        okback:
            if (relx < firstw) {
                u8 slot2 = data_020a0e40;
                if ((u8)(data_020a0deb[slot2 * 4] - 0x98) < 0x20 &&
                    IsButtonInputValid() != 0) {
                    data_0209f29c = 0;
                    goto okback_go;
                }
            }
            {
                u8 slot2 = data_020a0e40;
                if ((u8)(data_020a0dea[slot2 * 4] - 0xd8) < 0x20 &&
                    (u8)(data_020a0deb[slot2 * 4] - 0x98) < 0x20) {
                    data_0209f29c = 1;
                }
            }
        okback_go:
            data_0209f2e0 = 3;
            SetControllerMode(data_0209f2dc);
            data_0209f240 = 1;
            data_0209f244 = data_0208ee44 << 2;
            _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            if (data_0209f29c == 0) {
                data_0209f2c4 = 2;
                func_02012790(0x116);
                return;
            }
            func_02012790(0x5c);
            if (SublevelToLevel(data_0209f2f8) == 0x1d) {
                data_0209f1ec = 0;
            } else {
                data_0209f1ec = 2;
            }
            return;
        }
    }
    case 9: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1000;
        data_0209f1ec = 0xa;
        data_0209f234 = GetSoundMode();
        data_0209f2ec = data_0208ee3c ^ 1;
        data_0209f1fc = data_0208ee40;
        data_0209f238 = 0;
        _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
        data_0209f2b4 = 2;
        data_0209f2e0 = 0;
        _ZN5Stage20PS_UpdateOptionsMenuEv();
        data_0209f29c = 0;
        data_0209f240 = 0;
        _ZN5Stage25PS_UpdateOkAndBackButtonsEb(1);
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            data_0209d45c &= ~2;
        } else {
            data_0209d45c &= ~4;
        }
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        return;
    }
    case 0xa: {
        u8 slot;
        u8 tx;
        u8 relx;
        u8 firstw;
        if (data_020a0de8[data_020a0e40 * 4] == 0) {
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
            u8 ty = data_020a0deb[slot * 4];
            if ((u8)(ty - 0x2e) < 0x14) {
                if ((u8)(tx - 0x5c) >= 0x10)
                    return;
                if ((u8)(ty - 0x30) >= 0x10)
                    return;
                if (data_0209f2a4 == 1) {
                    int t = 0;
                    if (data_020a0de8[slot * 4] != 0) {
                        if (data_020a0de9[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                data_0209f2a4 = 1;
                data_0209f234 = data_0209f234 - 1;
                if (data_0209f234 == 0xff)
                    data_0209f234 = 2;
                data_0209f238 = 2;
                data_0209f210 = data_0208ee44 * 3;
                data_0209f2e0 = 0;
                _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
                data_0209f2e4 = 8;
                _ZN5Stage20PS_UpdateOptionsMenuEv();
                SetSoundMode(data_0209f234);
                func_02012790(0x64);
                if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x5c) < 0x10)
                    return;
                return;
            }
        }
        {
            u8 ty2;
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
            if ((u8)(tx - 6) < 0x3c) {
                ty2 = data_020a0deb[slot * 4];
                if ((u8)(ty2 - 0x26) < 0x24)
                    goto backlight_on;
            }
            goto chk_bl_off;
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
            if (data_0209f2a4 == 2) {
                int t = 0;
                if (data_020a0de8[slot * 4] != 0) {
                    if (data_020a0de9[slot * 4] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 2;
            data_0209f234 = data_0209f234 + 1;
            if (data_0209f234 == 3)
                data_0209f234 = 0;
            data_0209f238 = 1;
            data_0209f210 = data_0208ee44 * 3;
            data_0209f2e0 = 0;
            SetSoundMode(data_0209f234);
            func_02012790(0x64);
            data_0209f2e4 = 8;
            _ZN5Stage20PS_UpdateOptionsMenuEv();
            _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
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
            if ((u8)(ty2 - 0x56) < 0x24) {
                if ((u8)(tx - 0x70) >= 0x38)
                    return;
                if ((u8)(ty2 - 0x58) >= 0x20)
                    return;
                if (data_0209f2a4 == 3) {
                    int t = 0;
                    if (data_020a0de8[slot * 4] != 0) {
                        if (data_020a0de9[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                data_0209f2a4 = 3;
                if (data_0209f2ec != 0) {
                    func_02012790(0x66);
                    TurnBacklightOn();
                } else {
                    data_0209f2cc = data_0208ee44 << 2;
                    func_02012790(0x67);
                }
                data_0209f2ec = 0;
                data_0209f2e0 = 1;
                _ZN5Stage20PS_UpdateOptionsMenuEv();
                if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x70) < 0x38)
                    return;
                return;
            }
            goto chk_bl_off2;
        chk_bl_off:
            if ((u8)(tx - 0xae) < 0x3c) {
                u8 ty3 = data_020a0deb[slot * 4];
                if ((u8)(ty3 - 0x56) < 0x24) {
                    if ((u8)(tx - 0xb0) >= 0x38)
                        return;
                    if ((u8)(ty3 - 0x58) >= 0x20)
                        return;
                    if (data_0209f2a4 == 4) {
                        int t = 0;
                        if (data_020a0de8[slot * 4] != 0) {
                            if (data_020a0de9[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 4;
                    if (data_0209f2ec == 0) {
                        func_02012790(0x66);
                        TurnBacklightOff();
                    } else {
                        data_0209f2cc = data_0208ee44 << 2;
                        func_02012790(0x67);
                    }
                    data_0209f2ec = 1;
                    data_0209f2e0 = 1;
                    _ZN5Stage20PS_UpdateOptionsMenuEv();
                    if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0xb0) < 0x38)
                        return;
                    return;
                }
            }
        chk_bl_off2:
            if (relx < 0x4c) {
                u8 ty4 = data_020a0deb[slot * 4];
                if ((u8)(ty4 - 0x56) < 0x24) {
                    if ((u8)(tx - 8) >= 0x48)
                        return;
                    if ((u8)(ty4 - 0x58) >= 0x20)
                        return;
                    if (data_0209f2a4 == 5) {
                        int t = 0;
                        if (data_020a0de8[slot * 4] != 0) {
                            if (data_020a0de9[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 5;
                    if (data_0209f2e0 != 1) {
                        data_0209f2e0 = 1;
                        _ZN5Stage20PS_UpdateOptionsMenuEv();
                    }
                    data_0209f2ec = data_0209f2ec ^ 1;
                    if (data_0209f2ec == 0) {
                        TurnBacklightOn();
                    } else {
                        TurnBacklightOff();
                    }
                    data_0209f2cc = data_0208ee44 << 2;
                    _ZN5Stage20PS_UpdateOptionsMenuEv();
                    func_02012790(0x66);
                    return;
                }
            }
            {
                int t = 0;
                if (data_020a0de8[slot * 4] != 0) {
                    if (data_020a0de9[slot * 4] != 0)
                        t = 1;
                }
                if (t != 0) {
                    if (relx < firstw &&
                        (u8)(data_020a0deb[slot * 4] - 0x98) < 0x20)
                        goto opt_okback;
                    if ((u8)(tx - 0xd8) < 0x20) {
                        if ((u8)(data_020a0deb[slot * 4] - 0x98) < 0x20)
                            goto opt_okback;
                    }
                }
                if (IsButtonInputValid() == 0)
                    return;
            opt_okback:
                if (relx < firstw) {
                    u8 s4 = data_020a0e40;
                    if ((u8)(data_020a0deb[s4 * 4] - 0x98) < 0x20 &&
                        IsButtonInputValid() != 0) {
                        data_0209f29c = 0;
                        goto opt_go;
                    }
                }
                {
                    u8 s4 = data_020a0e40;
                    if ((u8)(data_020a0dea[s4 * 4] - 0xd8) < 0x20 &&
                        (u8)(data_020a0deb[s4 * 4] - 0x98) < 0x20) {
                        data_0209f29c = 1;
                    }
                }
            opt_go:
                data_0209f2e0 = 2;
                data_0209f22c = data_0208ee44 << 3;
                _ZN5Stage20PS_UpdateOptionsMenuEv();
                data_0209f240 = 1;
                data_0209f244 = data_0208ee44 << 2;
                _ZN5Stage25PS_UpdateOkAndBackButtonsEb(0);
                if (data_0209f29c == 0) {
                    SetSoundMode(data_0209f234);
                    if (data_0209f1fc != 0) {
                        data_0208ee40 = 1;
                    } else {
                        data_0208ee40 = 0;
                    }
                    data_0209f2c4 = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5d);
                if (SublevelToLevel(data_0209f2f8) == 0x1d) {
                    data_0209f1ec = 0;
                } else {
                    data_0209f1ec = 2;
                }
                return;
            }
        }
    }
    case 0xb: {
        if (IsButtonInputValid() == 0)
            return;
        data_0209f244 = data_0208ee44 << 2;
        data_0209f2c4 = 2;
        func_02012790(3);
        return;
    }
    case 0xc: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        _ZN7Message19DisplaySaveMenuTextEt(0x290);
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2e0 = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209f1ec = 0xd;
        return;
    }
    case 0xd: {
        {
            u8 slot = data_020a0e40;
            int touched = 0;
            if (data_020a0de8[slot * 4] != 0 && data_020a0de9[slot * 4] != 0)
                touched = 1;
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
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto seld_0;
            goto chkd_1;
        seld_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f2c4 = 2;
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
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto seld_1;
            goto chkd_2;
        seld_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0xe;
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
            if (data_0209f2e0 != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        seld_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x10;
            func_02012790(0x5f);
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 0xe: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        data_0209f1ec = 0xf;
        _ZN7Message21DisplaySaveStatusTextEt(0x295);
        data_0209d45c &= ~2;
        data_0209f2b8 = 0x78;
        return;
    }
    case 0xf: {
        if (data_0209f2b8 == 0)
            return;
        if (data_0209f2b8 == 0x78)
            _ZN8SaveData15SaveCurrentFileEv();
        if (data_0209f2b8 == 0x3c)
            _ZN7Message21DisplaySaveStatusTextEt(0x296);
        data_0209f2b8 = data_0209f2b8 - data_0208ee44;
        if (data_0209f2b8 != 0)
            return;
        if (data_0209f2e0 != 1) {
            data_0209f2c4 = 2;
            return;
        }
        _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        data_0209f280 = 1;
        _ZN5Stage10PS_CleanupEv();
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
        return;
    }
    case 0x10: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1100;
        data_0209f1ec = 0x11;
        _ZN7Message19DisplayDontSaveTextEt(0x297);
        data_0209f2e0 = 1;
        _ZN5Stage17PS_UpdateSaveMenuEb(1);
        return;
    }
    case 0x11: {
        {
            u8 slot = data_020a0e40;
            int touched = 0;
            if (data_020a0de8[slot * 4] != 0 && data_020a0de9[slot * 4] != 0)
                touched = 1;
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = data_020a0e40;
            u8 tx = data_020a0dea[slot * 4];
            int rel0 = tx - 0x28;
            if ((u8)rel0 < 0x50) {
                int rely = data_020a0deb[slot * 4] - 0x98;
                if ((u8)rely < 0x20) {
                    if (data_0209f2e0 == 0)
                        data_0209f244 = data_0208ee44 << 2;
                    if ((u8)rel0 < 0x50 && (u8)rely < 0x20) {
                        data_0209f22c = data_0208ee44 << 3;
                        data_0209f1ec = 0x12;
                        func_02012790(0x60);
                    } else {
                        func_02012790(0x51);
                    }
                    data_0209f2e0 = 0;
                    _ZN5Stage17PS_UpdateSaveMenuEb(0);
                    return;
                }
            }
            {
                int rel1 = tx - 0x88;
                if ((u8)rel1 >= 0x50)
                    return;
                {
                    int rely2 = data_020a0deb[slot * 4] - 0x98;
                    if ((u8)rely2 >= 0x20)
                        return;
                    if (data_0209f2e0 == 1)
                        data_0209f244 = data_0208ee44 << 2;
                    if ((u8)rel1 < 0x50 && (u8)rely2 < 0x20) {
                        data_0209f22c = data_0208ee44 << 3;
                        data_0209f1ec = 0x12;
                        func_02012790(3);
                    } else {
                        func_02012790(0x52);
                    }
                    data_0209f2e0 = 1;
                    _ZN5Stage17PS_UpdateSaveMenuEb(0);
                    return;
                }
            }
        }
    }
    case 0x12: {
        if (data_0209f2e0 != 0) {
            data_0209f2c4 = 2;
            return;
        }
        _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
        data_0209f280 = 1;
        _ZN5Stage10PS_CleanupEv();
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        return;
    }
    case 0x13: {
        int t = (data_0209f2d8 == 1);
        if (t != false && data_0209fc68 == 0) {
            _ZN5Scene14StartSceneFadeEjjt(6, 1, 0);
            data_0209f280 = 1;
            _ZN5Stage10PS_CleanupEv();
            data_0209b454 |= 0x40000000;
            data_0209b464 = data_0209b454;
            return;
        }
        LoadLevelNoReturn(2, 0, -1, 0);
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        data_0209f280 = 1;
        data_0209f2c4 = 2;
        return;
    }
    default:
        return;
    }
}
}
