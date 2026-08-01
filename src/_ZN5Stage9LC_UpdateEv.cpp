//cpp
#include "types.h"
extern "C" {
int IsButtonInputValid(void);
void _ZN5Stage17UpdateMenuButtonsEb(int b);
int SublevelToLevel(int lv);
void _ZN7Message21DisplayLevelClearTextEta(int m, u8 lvl);
void SetBg2Offset(int x, int y);
void SetBg3Offset(int x, int y);
void SetSubBg0Offset(int x, int y);
void SetSubBg1Offset(int x, int y);
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int fix, int e);
void GiveLives(int n);
void _ZN5Sound12PlayBank3_2DEj(u32 a);
u32 _ZN8SaveData13GetCoinRecordEj(int lv);
void _ZN8SaveData21SetCoinRecordIfHigherEah(int a, int b);
void func_02012790(int snd);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, int a, int b);
void _ZN7Message21DisplaySaveStatusTextEt(int m);
void _ZN8SaveData15SaveCurrentFileEv(void);
void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, int c);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 a);

extern int data_0208ee44;
extern s8 data_02092124;
extern int data_0209b454;
extern int data_0209b464;
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_0209f20c;
extern u8 data_0209f22c;
extern u8 data_0209f244;
extern u8 data_0209f260;
extern u8 data_0209f280;
extern u8 data_0209f2ac;
extern u8 data_0209f2b0;
extern u8 data_0209f2b4;
extern u8 data_0209f2b8;
extern u8 data_0209f2d4;
extern u8 data_0209f2e0;
extern s8 data_0209f2f8;
extern s16 data_0209f358;
extern u16 data_0209f360[];
extern volatile u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern volatile u8 data_020a0dea[];
extern volatile u8 data_020a0deb[];
extern u8 data_ov002_02111178;
}

#define REG16(a) (*(volatile u16 *)(a))

struct Stage {
    static void LC_Update();
};

void Stage::LC_Update()
{
    if (data_0209f244 != 0) {
        data_0209f244 = data_0209f244 - data_0208ee44;
        if (data_0209f244 == 0)
            _ZN5Stage17UpdateMenuButtonsEb(0);
    }
    if (data_0209f22c != 0) {
        data_0209f22c = data_0209f22c - data_0208ee44;
        return;
    }

    switch (data_0209f2d4) {
    case 0:
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        _ZN7Message21DisplayLevelClearTextEta(0x27a, SublevelToLevel(data_02092124));
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2b0 = 0;
        data_0209f2e0 = 0;
        _ZN5Stage17UpdateMenuButtonsEb(1);
        SetBg3Offset(0, 0);
        data_0209d45c |= 8;
        data_0209f260 = 0;
        data_0209f2d4 = 1;
        data_0209f280 = 0;
        if (data_0209f358 != 0)
            return;
        if (data_0209f2ac != 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        data_0209f22c = 0x1e;
        return;
    case 1: {
        int level = SublevelToLevel(data_02092124);
        if (data_0209f358 > data_0209f260) {
            data_0209f260 += 1;
            if (data_0209f260 % 50 == 0) {
                GiveLives(1);
                _ZN5Sound12PlayBank3_2DEj(0x6e);
            }
            if (data_0209f358 == data_0209f260) {
                if (data_0209f2ac != 0) {
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
                    data_0209f280 = 1;
                }
                data_0209f22c = 0x1e;
                if (level < 0xf) {
                    u8 coins = data_0209f260;
                    if (_ZN8SaveData13GetCoinRecordEj(level) >= coins)
                        return;
                    _ZN8SaveData21SetCoinRecordIfHigherEah(level, coins);
                    data_0209f2b0 = 1;
                    func_02012790(0x22);
                    return;
                }
                if (level < 0xf)
                    return;
                if (level >= 0x15)
                    return;
                func_02012790(0x22);
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
                data_0209f280 = 1;
                return;
            }
            func_02012790(0x15);
            return;
        }
        if (data_0209f260 == 0 && level >= 0xf && level < 0x15) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        data_0209f22c = 0x3c;
        data_0209f358 = 0;
        data_0209f2d4 = 2;
        return;
    }
    case 2: {
        int t6;
        int t5;
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1210;
        } else {
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        }
        data_0209f358 = 0;
        t6 = data_0209d45c ^ 0x18;
        t5 = data_0209d454 ^ 0x10;
        SetBg2Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d454 |= 3;
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4000050, t6 | 0x20, -7);
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4001050, t5 | 0x20, -7);
        data_0209f2d4 = 3;
        return;
    }
    case 3: {
        {
            int touched = 0;
            u8 slot = data_020a0e40;
            if (data_020a0de8[slot * 4] != 0) {
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
            func_02012790(0x57);
            data_0209f2d4 = 4;
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_1:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x50) < 0x20)
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
            func_02012790(0x58);
            data_0209f2d4 = 4;
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_2:
        {
            u8 slot = data_020a0e40;
            if ((u8)(data_020a0dea[slot * 4] - 8) < 0xf0 &&
                (u8)(data_020a0deb[slot * 4] - 0x78) < 0x20)
                goto sel3_2;
            if (data_0209f2e0 != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel3_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            _ZN5Stage17UpdateMenuButtonsEb(0);
            data_0209f22c = data_0208ee44 << 3;
            func_02012790(0x59);
            data_0209f2d4 = 6;
            if ((u8)(data_020a0dea[data_020a0e40 * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 4:
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        data_0209f2d4 = 5;
        data_0209d45c &= ~4;
        _ZN7Message21DisplaySaveStatusTextEt(0x295);
        data_0209f2b8 = 0x78;
        return;
    case 5:
        if (data_0209f2b8 == 0)
            return;
        if (data_0209f2b8 == 0x78) {
            _ZN8SaveData15SaveCurrentFileEv();
        } else if (data_0209f2b8 == 0x3c) {
            _ZN7Message21DisplaySaveStatusTextEt(0x296);
        }
        data_0209f2b8 = data_0209f2b8 - data_0208ee44;
        if (data_0209f2b8 == 0)
            data_0209f2d4 = 6;
        return;
    case 6:
        if (data_0209f280 != 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x7f, 0, 0x15666, 0);
            data_0209f280 = 0;
        }
        data_0209d45c &= ~0xc;
        data_0209d454 &= ~3;
        REG16(0x4000050) = 0;
        if (data_0209f2e0 != 1) {
            data_ov002_02111178 = 5;
        } else {
            _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
            data_ov002_02111178 = 0;
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
            {
                int t = data_0209b454 | 0x40000000;
                data_0209b454 = t;
                data_0209b464 = t;
            }
        }
        data_0209f2e0 = 0;
        data_0209f2d4 = 0;
        data_0209f2b0 = 0;
        data_0209f20c = 0;
        return;
    default:
        return;
    }
}
