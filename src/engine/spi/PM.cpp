//cpp
/* The power-management PXI channel (the Nitro SDK's spi/ARM9/pm.c),
 * arm9 .text 0x0205f5c0..0x0205ff70.
 *
 * Each command sender takes the channel lock (PMi_Lock) and sends one to
 * three words to the ARM7 on PXI tag 8 (PMi_SendPxiData); all but
 * PMi_SendSleepStart first store a completion callback and its argument.
 * PMi_CommonCallback, the tag's receive callback, stores any reply and
 * finishes through PMi_CallCallbackAndUnlock. Each
 * synchronous form passes PMi_DummyCallback, which stores the result in a
 * stack word, and spins in PMi_WaitBusy until the lock drops.
 *
 * Names: pret's pokediamond (SPI_pm.s) and pret's pmd-sky (spi_pm.s) keep
 * this file as assembly under the same function names; pokediamond writes
 * LED and LCD in capitals, pmd-sky Led and Lcd, and this file follows
 * pokediamond. Each function here is matched to one of theirs by its body:
 * the PXI command it sends (0x60 sleep start, 0x63 utility, 0x65 register
 * read, 0x66 LED pattern), the utility command numbers (LED 1..3, backlight
 * 4..9, power off 0xe), and the registers it reads and writes (POWCNT1
 * 0x04000304, IME 0x04000208, the VBlank count 0x027ffc3c). mwccarm emits the
 * bodies in reverse, so PMi_Lock, the first in source, is the highest in the
 * ROM.
 *
 * This SDK is older than either pret's. The channel state is separate
 * variables, not one PMi_Work struct. There are no sleep callback lists
 * (PM_GoSleepMode saves neither the backlights nor the display registers),
 * no amplifier control (no PMi_SetAmp), and no register write or LED-pattern
 * read, so the LED-pattern sender is the lowest function.
 * PM_ForceToPowerOffAsync sends the command without turning the LCD off
 * first. PMi_WaitBusy spins on the lock without polling the PXI receive FIFO.
 * PMi_CommonCallback stores register replies for commands 0x70..0x73, not
 * 0x70..0x74.
 */
#include "types.h"
#include "IRQ.h"
#include "CP15.h"
#include "nitro/spi/pm.h"
#include "nitro/ctrdg/ctrdg_common.h"

#define PXI_FIFO_TAG_PM 8
#define PXI_PROC_ARM7 1

#define REG_IME (*(volatile u16 *)0x04000208)
#define REG_IF (*(volatile u32 *)0x04000214)
#define REG_POWCNT1 (*(volatile u16 *)0x04000304)
#define HW_VBLANK_COUNT (*(u32 *)0x027ffc3c) /* the SDK's HW_VBLANK_COUNT_BUF */

#define PM_TRIGGER_CARD 0x08
#define PM_TRIGGER_CARTRIDGE 0x10

#define PM_LCD_POWER_OFF 0
#define PM_LCD_POWER_ON 1

/* The SDK's inline IME helpers: each returns the previous IME. */
static inline u16 OS_DisableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 0;
    return prep;
}

static inline u16 OS_EnableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 1;
    return prep;
}

static inline u16 OS_RestoreIrq(u16 enable) {
    u16 prep = REG_IME;
    REG_IME = enable;
    return prep;
}

extern "C" {

extern u16 data_020a8104;          /* set once PM_Init has run */
extern volatile int data_020a8108; /* command 0x60 answered */
extern int data_020a810c;          /* command 0x62 answered */
extern u32 data_020a8110;          /* HW_VBLANK_COUNT when the LCD last went off */
extern char data_020a8124;         /* an OSMutex */

void OS_SpinWait(u32 cycles);
void OS_InitMutex(void *mutex);
void PXI_Init(void);
int PXI_IsCallbackReady(int tag, int proc);
void PXI_SetFifoRecvCallback(int tag, void *callback);
int PXI_SendWordByFifo(unsigned int tag, unsigned int data, unsigned int reply);

void PMi_CommonCallback(int tag, u32 data, int err);
u32 PMi_SendSleepStart(u16 trigger, u16 keyPattern);
void PMi_SendPxiData(u32 data);

int PMi_Lock(void) {
    u32 saved = IRQ::Disable();
    if (data_020a8114.lock != 0) {
        IRQ::Restore(saved);
        return 0;
    }
    data_020a8114.lock = 1;
    IRQ::Restore(saved);
    return 1;
}

void PMi_WaitBusy(void) {
    volatile int *lock = &data_020a8114.lock;
    while (*lock != 0) {
    }
}

void PMi_DummyCallback(u32 result, void *arg) {
    *(u32 *)arg = result;
}

void PMi_CallCallbackAndUnlock(u32 result) {
    int lock = data_020a8114.lock;
    PMCallback callback = data_020a8114.callback;
    void *arg = data_020a8114.callbackArg;
    if (lock != 0) {
        data_020a8114.lock = 0;
    }
    if (callback == 0) {
        return;
    }
    data_020a8114.callback = 0;
    callback(result, arg);
}

void PM_Init(void) {
    int i;
    if (data_020a8104 != 0)
        return;
    data_020a8104 = 1;
    data_020a8114.lock = 0;
    data_020a8114.callback = 0;
    PXI_Init();
    while (PXI_IsCallbackReady(PXI_FIFO_TAG_PM, PXI_PROC_ARM7) == 0)
        ;
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_PM, (void *)PMi_CommonCallback);
    for (i = 0; i < 4; i++)
        data_020a8138[i].flag = 0;
    OS_InitMutex(&data_020a8124);
    data_020a8110 = HW_VBLANK_COUNT;
}

void PMi_CommonCallback(int tag, u32 data, int err) {
    u16 command;
    u16 result;

    if (err != 0) {
        PMi_CallCallbackAndUnlock(2);
        return;
    }

    command = (data & 0x7f00) >> 8;
    result = data & 0xff;

    if (command >= 0x70 && command <= 0x73) {
        int index = command - 0x70;
        u16 value = result & 0xff;
        u16 *reply = data_020a8138[index].reply;
        if (reply != 0) {
            *reply = value;
        }
        data_020a8138[index].flag = 1;
        result = 0;
    } else if (command == 0x60) {
        data_020a8108 = 1;
    } else if (command == 0x62) {
        data_020a810c = 1;
    } else if (command == 0x67) {
        int *reply = data_020a8114.replyDst;
        if (reply != 0) {
            *reply = result;
        }
        result = 0;
    }

    PMi_CallCallbackAndUnlock(result);
}

u32 PMi_SendSleepStart(u16 trigger, u16 keyPattern) {
    volatile int *ready;

    if (!PMi_Lock())
        return 1;
    data_020a8108 = 0;
    PMi_SendPxiData(0x3006000);
    ready = &data_020a8108;
    while (!*ready) {
    }
    *ready = 0;
    data_020a810c = 0;
    PMi_SetLCDPower(PM_LCD_POWER_OFF, 2, 0, 0);
    PMi_SendPxiData((trigger & 0xff) | 0x2006100);
    PMi_SendPxiData((keyPattern & 0xffff) | 0x1010000);
    return 0;
}

u32 PM_SendUtilityCommandAsync(u32 number, PMCallback callback, void *arg) {
    if (!PMi_Lock())
        return 1;
    data_020a8114.callback = callback;
    data_020a8114.callbackArg = arg;
    PMi_SendPxiData(((number >> 16) & 0xff) | 0x2006300);
    PMi_SendPxiData((number & 0xffff) | 0x1010000);
    return 0;
}

u32 PMi_ReadRegisterAsync(u32 registerAddr, u16 *buffer, PMCallback callback, void *arg) {
    if (!PMi_Lock())
        return 1;
    data_020a8138[registerAddr].flag = 0;
    data_020a8114.callback = callback;
    data_020a8114.callbackArg = arg;
    data_020a8138[registerAddr].reply = buffer;
    PMi_SendPxiData((registerAddr & 0xff) | 0x3006500);
    return 0;
}

u32 PMi_ReadRegister(u32 registerAddr, u16 *buffer) {
    u32 result;
    u32 sent = PMi_ReadRegisterAsync(registerAddr, buffer, PMi_DummyCallback, &result);
    if (sent != 0)
        return sent;
    PMi_WaitBusy();
    return result;
}

u32 PMi_SetLEDAsync(u32 status, PMCallback callback, void *arg) {
    u32 command;
    switch (status) {
    case 1:
        command = 1;
        break;
    case 3:
        command = 2;
        break;
    case 2:
        command = 3;
        break;
    default:
        command = 0;
        break;
    }
    if (command == 0)
        return 0xffff;
    return PM_SendUtilityCommandAsync(command, callback, arg);
}

u32 PMi_SetLED(u32 status) {
    u32 result;
    u32 sent = PMi_SetLEDAsync(status, PMi_DummyCallback, &result);
    if (sent != 0)
        return sent;
    PMi_WaitBusy();
    return result;
}

u32 PM_SetBackLightAsync(u32 target, u32 status, PMCallback callback, void *arg) {
    u32 command = 0;
    if (target == 0) {
        if (status == 1)
            command = 6;
        if (status == 0)
            command = 7;
    } else if (target == 1) {
        if (status == 1)
            command = 4;
        if (status == 0)
            command = 5;
    } else if (target == 2) {
        if (status == 1)
            command = 8;
        if (status == 0)
            command = 9;
    }
    if (command == 0)
        return 0xffff;
    return PM_SendUtilityCommandAsync(command, callback, arg);
}

u32 PM_SetBackLight(u32 target, u32 status) {
    u32 result;
    u32 sent = PM_SetBackLightAsync(target, status, PMi_DummyCallback, &result);
    if (sent != 0)
        return sent;
    PMi_WaitBusy();
    return result;
}

u32 PM_ForceToPowerOffAsync(PMCallback callback, void *arg) {
    return PM_SendUtilityCommandAsync(0xe, callback, arg);
}

u32 PM_ForceToPowerOff(void) {
    u32 result;
    u32 sent = PM_ForceToPowerOffAsync(PMi_DummyCallback, &result);
    if (sent != 0)
        return sent;
    PMi_WaitBusy();
    return result;
}

u32 PM_GetBackLight(int *top, int *bottom) {
    u16 flags;
    u32 result = PMi_ReadRegister(0, &flags);
    if (result)
        return result;
    if (top)
        *top = (flags & 8) ? 1 : 0;
    if (bottom)
        *bottom = (flags & 4) ? 1 : 0;
    return result;
}

void PMi_SendPxiData(u32 data) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_PM, data, 0) != 0)
        ;
}

void PM_GoSleepMode(u32 trigger, u32 logic, u32 keyPattern) {
    u32 saved_mode;
    u32 saved_mask;
    u32 card_pulled = 0;
    u16 saved_ime;

    saved_ime = OS_DisableIrq();
    saved_mode = IRQ::Disable();
    saved_mask = IRQ::DisableIRQs(0x3fffff);
    IRQ::SetIRQs(0x40000);
    IRQ::Restore(saved_mode);

    (void)OS_EnableIrq();
    if (trigger & PM_TRIGGER_CARD) {
        /* pret's pmd-sky makes the same test of this word */
        if (*(volatile u16 *)0x027ffc40 == 2)
            trigger &= ~PM_TRIGGER_CARD;
    }
    if (trigger & PM_TRIGGER_CARTRIDGE) {
        if (CTRDG_IsExisting() == 0)
            trigger &= ~PM_TRIGGER_CARTRIDGE;
    }
    while (PMi_SendSleepStart(trigger, logic | keyPattern) != 0)
        ;

    CP15::WaitForInterrupt();
    OS_SpinWait(0x332340);
    if (trigger & PM_TRIGGER_CARD) {
        if (REG_IF & 0x100000)
            card_pulled = 1;
    }
    if (card_pulled == 0) {
        PMi_SetLCDPower(PM_LCD_POWER_ON, 1, 1, 0);
    }
    IRQ::Disable();
    IRQ::SetIRQs(saved_mask);
    IRQ::Restore(saved_mode);

    (void)OS_RestoreIrq(saved_ime);
    if (card_pulled != 0) {
        PM_ForceToPowerOff();
    }
}

int PMi_SetLCDPower(u32 sw, u32 led, int skip, int isSync) {
    switch (sw) {
    case PM_LCD_POWER_ON:
        if (skip == 0) {
            if (HW_VBLANK_COUNT - data_020a8110 <= 7)
                return 0;
        }
        if (led != 0) {
            if (isSync != 0)
                PMi_SetLED(led);
            else
                PMi_SetLEDAsync(led, 0, 0);
        }
        REG_POWCNT1 |= 1;
        break;
    case PM_LCD_POWER_OFF:
        REG_POWCNT1 &= ~1;
        data_020a8110 = HW_VBLANK_COUNT;
        if (led != 0) {
            if (isSync != 0)
                PMi_SetLED(led);
            else
                PMi_SetLEDAsync(led, 0, 0);
        }
        break;
    }
    return 1;
}

int PM_SetLCDPower(u32 sw) {
    u32 mode = sw;
    if (sw != PM_LCD_POWER_ON)
        mode = PM_LCD_POWER_OFF;
    return PMi_SetLCDPower(mode, 0, 0, 1);
}

int PM_GetLCDPower(void) {
    return (REG_POWCNT1 & 1) != 0;
}

u32 PMi_SendLEDPatternCommandAsync(u32 pattern, PMCallback callback, void *arg) {
    if (!PMi_Lock())
        return 1;
    data_020a8114.callback = callback;
    data_020a8114.callbackArg = arg;
    PMi_SendPxiData((pattern & 0xff) | 0x3006600);
    return 0;
}

u32 PMi_SendLEDPatternCommand(u32 pattern) {
    u32 result;
    u32 sent = PMi_SendLEDPatternCommandAsync(pattern, PMi_DummyCallback, &result);
    if (sent != 0)
        return sent;
    PMi_WaitBusy();
    return result;
}
}
