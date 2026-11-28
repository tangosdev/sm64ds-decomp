//cpp
#include "types.h"
#include "Heap.h"
#include "Memory.h"
#include "PlayerInput.h"
#include "nitro/gx/g2.h"
#include "nitro/os/init.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/system.h"
#include "nitro/pad/pad.h"
#include "nitro/spi/pm.h"
#include "dScene_c.h"
#include "nitro/hw/registers.h"
#include "private/controller_input.h"
#include "Link.h"
#include "private/watchdog_alarm.h"
#include "fManager_c.h"

extern int data_0209d50c;
extern unsigned char data_0209d4e8;
extern PadData data_020a0e58[];
extern unsigned short data_0209d534[];
extern int data_0209d51c;
extern int gGlobalTimer;
extern unsigned char data_0209d4f0;
extern char data_0209ee90[];
extern int data_0209d514;
extern unsigned char gSoundInitialized;
extern unsigned char data_0209e64c;
extern unsigned char gSoftResetDisabled;
extern int data_0209fc68;
extern int gFrameStep;
extern unsigned char data_0208ee48;
extern unsigned char data_0209d4ec;
extern unsigned char gSoundMuted;
extern short data_0209d504;
extern unsigned char data_0209d4f4;
extern u8 gGameMode;
extern u8 data_0209d4e4;
extern unsigned char data_0209d4d4[];
extern PMBackLightSwitch data_0209d508;
extern PMBackLightSwitch data_0209d510;

void func_02019390(void);
void func_02023498(void);
void func_02019404(void);
extern "C" {
void func_02019ac4(void);
void func_0203bb60(void);
void func_0203bc7c(void);
void func_020190b8(void);
void func_0201ffcc(void);
int func_02030e4c(void);
void func_020132d8(void);
int func_0203da3c(void);
}
extern "C" void func_02019e60(void);
extern "C" int func_0203d974(void);
extern "C" unsigned int func_02012790(unsigned int n);
extern "C" void func_02019dfc(void);
extern "C" void func_02011dc0(void);
extern "C" void func_02019ed0(void);
extern "C" void func_02011db4(void);
extern "C" void func_0201a4d0(void);
extern "C" void func_02030838(void);
extern "C" void func_0201a4bc(void);
extern "C" void func_02061128(int (*callback)(void));
extern "C" int func_02019f10(void);

extern "C" {

/* Sleep with the lid closed. data_0209d4e4 is up while it sleeps (the
   pull-out thread func_02019f64 reads it). It wakes on the lid opening or
   the card being pulled. The lid check func_02019ac4 calls it. */
void func_02019ed0(void) {
    data_0209d4e4 = 1;
    PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, 0, 0);
    data_0209d4e4 = 0;
}

/* Register func_02019f10 as the card pull-out callback (func_02061128),
   during boot (NitroStartUp). func_02019f10 starts the thread that shuts
   the game down. */
void func_02019ebc(void) {
    func_02061128(func_02019f10);
}

/* Turn both backlights off for a closed lid, once: keep their states
   (PM_GetBackLight into data_0209d508 and data_0209d510), then
   PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF), as
   TurnBacklightOff does. data_0209d4d4 records it. The lid check
   func_02019ac4 calls it when the lid closes. */
void func_02019e60(void) {
    if (data_0209d4d4[0])
        return;
    PM_GetBackLight(&data_0209d508, &data_0209d510);
    PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);
    data_0209d4d4[0] = 1;
}

/* Put the backlights back as func_02019e60 kept them: the top from
   data_0209d508, the bottom from data_0209d510. The lid check
   func_02019ac4 calls it when the lid opens again. */
void func_02019dfc(void) {
    if (data_0209d4d4[0] == 0)
        return;
    PM_SetBackLight(PM_LCD_TOP, data_0209d508);
    PM_SetBackLight(PM_LCD_BOTTOM, data_0209d510);
    data_0209d4d4[0] = 0;
}

static inline int IsSkipRequested(void) {
    return (func_0203da3c() != 0 || gSoftResetDisabled != 0) ? 1 : 0;
}

void func_02019ac4(void) {
    int skip;
    int delta;
    int closed;
    unsigned int t;

    skip = IsSkipRequested();
    closed = PAD_DetectFold();
    t = data_0209fc68;
    delta = gFrameStep;
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
                if (func_0203d974() != 0) {
                    data_0209d504 = 60;
                    arg = 0x44;
                } else {
                    data_0209d504 = 120;
                    arg = 0x10;
                }
                func_02012790(arg);
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
                    func_02011dc0();
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
                func_02011db4();
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
                if (func_0203d974() != 0) {
                    data_0209d504 = 60;
                    arg = 0x45;
                } else {
                    data_0209d504 = 180;
                    arg = 0x11;
                }
                func_02012790(arg);
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
        func_0201a4d0();
    }

    if (data_0209d4f4 != 0) {
        func_02011db4();
        data_0209d4f4 = 0;
        func_0201a4d0();
    }
    data_0209d4ec = 7;
}

void func_020199a4(void) {
    int sl = 0xb4;
    if (data_0209d4ec == 7)
        data_0209d4ec = 0;
    func_0201a9ec(&gWatchdog);
    for (;;) {
        int b = !(gGameMode == 1);
        if (b == 0)
            func_02030838();
        {
            int dt = gFrameStep;
            if (sl > 0)
                sl -= dt;
            else
                func_02019ac4();
        }
        data_0209d4f0 = 1;
        func_0201a4bc();
        data_0209d4f0 = 0;
        data_0209d514 = 0;
        if (gSoundInitialized != 0)
            func_020132d8();
    }
}
}

/* The main loop (main.cpp), never returning. data_0209d50c records the
   step it is in. */
void func_020197b8(void) {
    G2_SetBlendBrightness(0x3f, 0x1f);
    G2S_SetBlendBrightness(0x3f, 0x1f);
    do {
        unsigned short v;
        data_0209d50c = 0xb;
        func_02019ac4();
        data_0209d50c = 2;
        func_02019390();
        data_0209d50c = 0x16;
        func_0203bb60();
        data_0209d50c = 0x17;
        func_0203bc7c();
        v = data_020a0e58[gActivePlayerSlot].held;
        data_0209d534[data_0209d4e8] = v;
        data_0209d51c = v;
        data_0209d4e8 = (data_0209d4e8 + 1) & 0x1f;
        func_02023498();
        data_0209d50c = 3;
        dScene_c::SpawnIfNecessary();
        data_0209d50c = 4;
        fManager_c::mainLoop();
        data_0209d50c = 5;
        func_02019404();
        data_0209d50c = 6;
        gGlobalTimer = gGlobalTimer + 1;
        OS_DisableIrqMask(OS_IE_V_BLANK);
        func_020190b8();
        data_0209d4f0 = 1;
        OS_EnableIrqMask(OS_IE_V_BLANK);
        data_0209d50c = 7;
        func_0201a4bc();
        data_0209d4f0 = 0;
        data_0209d50c = 0x15;
        func_0201ffcc();
        if ((*(int *)(data_0209ee90 + 0x108) & 0x10) == 0)
            data_0209d514 = 0;
        data_0209d50c = 9;
        if (gSoundInitialized != 0)
            func_020132d8();
        if (data_0209e64c != 0 || GetLinkStatus(0x9d) != 0)
            func_02030e4c();
    } while (1);
}

/* Boot, lid, IRQ and main-loop state. Interior labels keep their configured
   addresses inside this one .bss run. */
unsigned char data_0209d4d4[0x54];
