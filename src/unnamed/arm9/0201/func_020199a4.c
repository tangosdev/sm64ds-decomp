#include "types.h"
#include "nitro/pad/pad.h"
#include "nitro/spi/pm.h"
#include "private/watchdog_alarm.h"

enum { false, true };

extern int _Z13func_0203da3cv(void);
extern void func_02019e60(void);
extern int _Z13func_0203d974v(void);
extern unsigned int _Z13func_02012790j(unsigned int n);
extern void func_02019dfc(void);
extern void _Z13func_02011dc0v(void);
extern void func_02019ed0(void);
extern void _Z13func_02011db4v(void);
extern unsigned char gSoftResetDisabled;
extern int data_0209fc68;
extern int data_0208ee44;
extern unsigned char data_0208ee48;
extern unsigned char data_0209d4ec;
extern unsigned char gSoundInitialized;
extern unsigned char gSoundMuted;
extern short data_0209d504;
extern unsigned char data_0209d4f4;
extern void _Z13func_0201a4d0v(void);
extern void func_02030838(void);
extern void _Z13func_0201a4bcv(void);
extern void _Z13func_020132d8v(void);
extern u8 data_0209d4f0;
extern int data_0209d514;
extern u8 gGameMode;

static inline int IsSkipRequested(void) {
    return (_Z13func_0203da3cv() != 0 || gSoftResetDisabled != 0) ? 1 : 0;
}

void func_02019ac4(void) {
    int skip;
    int delta;
    int closed;
    unsigned int t;

    skip = IsSkipRequested();
    closed = PAD_DetectFold();
    t = data_0209fc68;
    delta = data_0208ee44;
    /* materialized compare kept: if (t == 6) or an inline predicate drops 3 insns */
    t = t == 6;
    if (t != false)
        data_0208ee48 = 0;
    if (data_0209d4ec == 0) {
        if (closed != 0) {
            if (gSoundInitialized != 0) {
                gSoundMuted = 1;
                func_02019e60();
                data_0209d4ec = 1;
            }
        }
    }
    if (data_0209d4ec == 1) {
        short tv = data_0209d504;
        if (tv <= 0) {
            if (data_0208ee48 != 0) {
                int arg;
                if (_Z13func_0203d974v() != 0) {
                    data_0209d504 = 0x3c;
                    arg = 0x44;
                } else {
                    data_0209d504 = 0x78;
                    arg = 0x10;
                }
                _Z13func_02012790j(arg);
            } else {
                data_0209d504 = 0;
            }
            data_0209d4ec = 2;
        } else {
            data_0209d504 = tv - delta;
        }
    }
    if (data_0209d4ec == 2) {
        if (closed == 0) {
            func_02019dfc();
            data_0209d4ec = 6;
        } else {
            short tv = data_0209d504;
            if (tv <= 0) {
                if (data_0209d4f4 == 0) {
                    _Z13func_02011dc0v();
                    data_0209d4f4 = 1;
                }
                if (skip != 0)
                    PM_SetLCDPower(PM_LCD_POWER_OFF);
                data_0209d4ec = 3;
            } else {
                data_0209d504 = tv - delta;
            }
        }
    }
    if (data_0209d4ec == 3) {
        if (skip == 0) {
            func_02019ed0();
            data_0209d4ec = 4;
        } else {
            if (closed == 0)
                data_0209d4ec = 4;
        }
    }
    if (data_0209d4ec == 4) {
        if (PM_GetLCDPower() == PM_LCD_POWER_ON || PM_SetLCDPower(PM_LCD_POWER_ON) == true) {
            if (data_0209d4f4 != 0) {
                _Z13func_02011db4v();
                data_0209d4f4 = 0;
            }
            func_02019dfc();
            data_0209d4ec = 5;
            data_0209d504 = 1;
        }
    }
    if (data_0209d4ec == 5) {
        short tv = data_0209d504;
        if (tv <= 0) {
            if (data_0208ee48 != 0) {
                int arg;
                if (_Z13func_0203d974v() != 0) {
                    data_0209d504 = 0x3c;
                    arg = 0x45;
                } else {
                    data_0209d504 = 0xb4;
                    arg = 0x11;
                }
                _Z13func_02012790j(arg);
            } else {
                data_0209d504 = 0;
            }
            data_0209d4ec = 6;
        } else {
            data_0209d504 = tv - delta;
        }
    }
    if (data_0209d4ec == 6) {
        short tv = data_0209d504;
        if (tv <= 0) {
            data_0209d4ec = 0;
            gSoundMuted = 0;
        } else {
            data_0209d504 = tv - delta;
        }
    }
}

void func_02019a58(void) {
    while (PM_GetLCDPower() != PM_LCD_POWER_ON && PM_SetLCDPower(PM_LCD_POWER_ON) != true) {
        _Z13func_0201a4d0v();
    }

    if (data_0209d4f4 != 0) {
        _Z13func_02011db4v();
        data_0209d4f4 = 0;
        _Z13func_0201a4d0v();
    }
    data_0209d4ec = 7;
}

void func_020199a4(void) {
    int sl = 0xb4;
    if (data_0209d4ec == 7)
        data_0209d4ec = 0;
    func_0201a9ec(&data_0209d574);
    for (;;) {
        int b = (int)(gGameMode == 1) ? 0 : 1;
        if (b == 0)
            func_02030838();
        {
            int dt = data_0208ee44;
            if (sl > 0)
                sl -= dt;
            else
                func_02019ac4();
        }
        data_0209d4f0 = 1;
        _Z13func_0201a4bcv();
        data_0209d4f0 = 0;
        data_0209d514 = 0;
        if (gSoundInitialized != 0)
            _Z13func_020132d8v();
    }
}
