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
 * Names and spellings: ntrtwl/NitroSDK has the C source (spi/ARM9/src/pm.c),
 * and pret's pokediamond (SPI_pm.s) and pmd-sky (spi_pm.s) carry it as
 * assembly under the same function names. Each function here is matched to
 * one of theirs by its body: the PXI command it sends, the utility command
 * numbers, and the registers it reads and writes. The statics take pm.c's
 * names. mwccarm emits the bodies in reverse, so PMi_Lock, the first in
 * source, is the highest in the ROM.
 *
 * This SDK is older than all three. There are no sleep callback lists
 * (PM_GoSleepMode saves neither the backlights nor the display registers),
 * no amplifier or sound control (no PMi_SetAmp), and no register write or
 * LED-pattern read, so the LED-pattern sender is the lowest function.
 * PM_ForceToPowerOffAsync sends the command without turning the LCD on
 * first, and PMi_WaitBusy spins on the lock without polling the PXI receive
 * FIFO. The register-read slots are four (REG0VALUE..REG3VALUE), not five.
 * PM_GoSleepMode waits out the wake-up before it checks for a pulled card,
 * not after.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/interrupt_vblank.h"
#include "nitro/os/mutex.h"
#include "nitro/hw/registers.h"
#include "nitro/spi/pm.h"
#include "nitro/ctrdg/ctrdg_common.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/mb/mb.h"
#include "nitro/pxi/fifo.h"
#include "nitro/pxi/init.h"

/* The SDK's nitro/types.h spellings; include/types.h has no BOOL. */
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

/* pm.c's packet builders: a start/end bit pair, the packet's sequence number,
 * then a command and one data byte, or a halfword of data. */
inline u32 PMi_MakeData1(u32 bit, u32 seq, u32 command, u32 data) {
    return (bit) | ((seq) << SPI_PXI_INDEX_SHIFT) | ((command) << 8) | ((data) & 0xff);
}

inline u32 PMi_MakeData2(u32 bit, u32 seq, u32 data) {
    return (bit) | ((seq) << SPI_PXI_INDEX_SHIFT) | ((data) & 0xffff);
}

#define PMi_SetCallback(callback, arg)      do {                                        PMi_Work.callback = (callback);         PMi_Work.callbackArg = (arg);       } while (0)

#define PMi_WAIT_FRAME 7

/* The SDK's inline IME helpers (nitro/os/common/interrupt.h): each returns
 * the previous IME. Local because an inline body in nitro/os/interrupt.h
 * would renumber the @NNN symbols of all its includers. */
static inline BOOL OS_EnableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 1;
    return prep;
}

static inline BOOL OS_DisableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 0;
    return prep;
}

static inline BOOL OS_RestoreIrq(BOOL enable) {
    u16 prep = REG_IME;
    REG_IME = enable;
    return prep;
}

/* The display's power bit (nitro/gx/gx.h's GXi_PowerLCD). */
#define REG_GX_POWCNT_LCD_MASK 0x0001

static inline void GXi_PowerLCD(BOOL enable) {
    if (enable) {
        REG_POWCNT1 |= REG_GX_POWCNT_LCD_MASK;
    } else {
        REG_POWCNT1 &= ~REG_GX_POWCNT_LCD_MASK;
    }
}

extern u16 PMi_IsInit;
extern volatile int PMi_SyncFlag; /* SPI_PXI_COMMAND_PM_SYNC (0x60) answered */
extern int PMi_SleepEndFlag;      /* SPI_PXI_COMMAND_PM_SLEEP_END (0x62) answered */
extern u32 PMi_LCDCount;          /* the VBlank count when the LCD last went off */
extern OSMutex PMi_Mutex;

void PMi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err);
u32 PMi_SendSleepStart(u16 trigger, u16 keyPattern);
void PMi_SendPxiData(u32 data);

int PMi_Lock(void) {
    OSIntrMode enabled = OS_DisableInterrupts();

    if (PMi_Work.lock) {
        OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    PMi_Work.lock = TRUE;
    OS_RestoreInterrupts(enabled);

    return TRUE;
}

void PMi_WaitBusy(void) {
    volatile int *p = &PMi_Work.lock;

    while (*p) {
    }
}

void PMi_DummyCallback(u32 result, void *arg) {
    *(u32 *)arg = result;
}

void PMi_CallCallbackAndUnlock(u32 result) {
    PMCallback callback;
    void *arg;

    callback = PMi_Work.callback;
    arg = PMi_Work.callbackArg;

    if (PMi_Work.lock) {
        PMi_Work.lock = FALSE;
    }

    if (callback) {
        PMi_Work.callback = NULL;
        (callback)(result, arg);
    }
}

void PM_Init(void) {
    int i;

    if (PMi_IsInit) {
        return;
    }

    PMi_IsInit = TRUE;
    PMi_Work.lock = FALSE;
    PMi_Work.callback = NULL;

    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_PM, PXI_PROC_ARM7)) {
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_PM, PMi_CommonCallback);

    for (i = 0; i < PMIC_REG_NUMS; i++) {
        PMi_RegisterBuffer[i].flag = FALSE;
    }

    OS_InitMutex(&PMi_Mutex);

    PMi_LCDCount = OS_GetVBlankCount();
}

void PMi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err) {
    u16 command;
    u16 pxiResult;

    if (err) {
        PMi_CallCallbackAndUnlock(PM_RESULT_ERROR);
        return;
    }

    command = ((data & SPI_PXI_RESULT_COMMAND_MASK) >> SPI_PXI_RESULT_COMMAND_SHIFT);
    pxiResult = ((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);

    if (SPI_PXI_COMMAND_PM_REG0VALUE <= command && command <= SPI_PXI_COMMAND_PM_REG3VALUE) {
        int num = (command - SPI_PXI_COMMAND_PM_REG0VALUE);
        u16 value = (pxiResult & 0xff);
        u16 *buffer = PMi_RegisterBuffer[num].buffer;

        if (buffer) {
            *buffer = value;
        }
        PMi_RegisterBuffer[num].flag = TRUE;
        pxiResult = PM_RESULT_SUCCESS;
    } else if (command == SPI_PXI_COMMAND_PM_SYNC) {
        PMi_SyncFlag = TRUE;
    } else if (command == SPI_PXI_COMMAND_PM_SLEEP_END) {
        PMi_SleepEndFlag = TRUE;
    } else if (command == SPI_PXI_COMMAND_PM_GET_BLINK) {
        if (PMi_Work.work) {
            *(u32 *)PMi_Work.work = pxiResult;
        }
        pxiResult = PM_RESULT_SUCCESS;
    }

    PMi_CallCallbackAndUnlock(pxiResult);
}

u32 PMi_SendSleepStart(u16 trigger, u16 keyIntrData) {
    u32 pxi_send_data;
    volatile int *p;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SyncFlag = 0;
    pxi_send_data =
        PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0, SPI_PXI_COMMAND_PM_SYNC, 0);
    PMi_SendPxiData(pxi_send_data);

    /* The later SDK spins on PMi_SyncFlag by name; this one waits through a
     * pointer, as PMi_WaitBusy does. By name, mwcc schedules the clear after
     * PMi_SetLCDPower's argument set-up instead of before it. */
    p = &PMi_SyncFlag;
    while (*p == 0) {
    }

    *p = 0;
    PMi_SleepEndFlag = 0;

    PMi_SetLCDPower(PM_LCD_POWER_OFF, PM_LED_BLINK_LOW, FALSE, FALSE);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_SLEEP_START, trigger);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, keyIntrData);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

u32 PM_SendUtilityCommandAsync(u32 number, PMCallback callback, void *arg) {
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT, 0, SPI_PXI_COMMAND_PM_UTILITY, number >> 16);
    PMi_SendPxiData(pxi_send_data);

    pxi_send_data = PMi_MakeData2(SPI_PXI_END_BIT, 1, number);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

u32 PMi_ReadRegisterAsync(u16 registerAddr, u16 *buffer, PMCallback callback, void *arg) {
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_RegisterBuffer[registerAddr].flag = FALSE;
    PMi_SetCallback(callback, arg);
    PMi_RegisterBuffer[registerAddr].buffer = buffer;

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0,
                                  SPI_PXI_COMMAND_PM_REG_READ, registerAddr);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

u32 PMi_ReadRegister(u16 registerAddr, u16 *buffer) {
    u32 commandResult;
    u32 sendResult = PMi_ReadRegisterAsync(registerAddr, buffer, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PMi_SetLEDAsync(PMLEDStatus status, PMCallback callback, void *arg) {
    u32 command;

    switch (status) {
    case PM_LED_ON:
        command = PM_UTIL_LED_ON;
        break;
    case PM_LED_BLINK_HIGH:
        command = PM_UTIL_LED_BLINK_HIGH_SPEED;
        break;
    case PM_LED_BLINK_LOW:
        command = PM_UTIL_LED_BLINK_LOW_SPEED;
        break;
    default:
        command = 0;
    }

    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
}

u32 PMi_SetLED(PMLEDStatus status) {
    u32 commandResult;
    u32 sendResult = PMi_SetLEDAsync(status, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PM_SetBackLightAsync(PMLCDTarget target, PMBackLightSwitch sw, PMCallback callback, void *arg) {
    u32 command = 0;

    if (target == PM_LCD_TOP) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD2_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD2_BACKLIGHT_OFF;
        }
    } else if (target == PM_LCD_BOTTOM) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD1_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD1_BACKLIGHT_OFF;
        }
    } else if (target == PM_LCD_ALL) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD12_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD12_BACKLIGHT_OFF;
        }
    }

    return (command) ? PM_SendUtilityCommandAsync(command, callback, arg) : PM_INVALID_COMMAND;
}

u32 PM_SetBackLight(PMLCDTarget target, PMBackLightSwitch sw) {
    u32 commandResult;
    u32 sendResult = PM_SetBackLightAsync(target, sw, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PM_ForceToPowerOffAsync(PMCallback callback, void *arg) {
    return PM_SendUtilityCommandAsync(PM_UTIL_FORCE_POWER_OFF, callback, arg);
}

u32 PM_ForceToPowerOff(void) {
    u32 commandResult;
    u32 sendResult = PM_ForceToPowerOffAsync(PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}

u32 PM_GetBackLight(PMBackLightSwitch *top, PMBackLightSwitch *bottom) {
    u16 reg;
    u32 result;

    if ((result = PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)) == PM_SUCCESS) {
        if (top) {
            *top = (reg & PMIC_CTL_BKLT2) ? PM_BACKLIGHT_ON : PM_BACKLIGHT_OFF;
        }
        if (bottom) {
            *bottom = (reg & PMIC_CTL_BKLT1) ? PM_BACKLIGHT_ON : PM_BACKLIGHT_OFF;
        }
    }

    return result;
}

void PMi_SendPxiData(u32 data) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_PM, data, FALSE) != PXI_FIFO_SUCCESS) {
    }
}

void PM_GoSleepMode(PMWakeUpTrigger trigger, PMLogic logic, u16 keyPattern) {
    BOOL prepIrq;
    OSIntrMode prepIntrMode;
    OSIrqMask prepIntrMask;
    BOOL powerOffFlag = FALSE;

    prepIrq = OS_DisableIrq();
    prepIntrMode = OS_DisableInterrupts();
    prepIntrMask = OS_DisableIrqMask((1 << OS_IRQ_TABLE_MAX) - 1);
    OS_SetIrqMask(OS_IE_SPFIFO_RECV);
    OS_RestoreInterrupts(prepIntrMode);
    OS_EnableIrq();

    if (trigger & PM_TRIGGER_CARD) {
        if (MB_IsMultiBootChild()) {
            trigger &= ~PM_TRIGGER_CARD;
        }
    }

    if (trigger & PM_TRIGGER_CARTRIDGE) {
        if (!CTRDG_IsExisting()) {
            trigger &= ~PM_TRIGGER_CARTRIDGE;
        }
    }

    while (PMi_SendSleepStart(trigger, logic | keyPattern) != PM_SUCCESS) {
    }

    OS_Halt();

    /* 100 ms at the 33.5 MHz system clock. */
    OS_SpinWait(0x332340);

    if (trigger & PM_TRIGGER_CARD) {
        if (REG_IF & OS_IE_CARD_IREQ) {
            powerOffFlag = TRUE;
        }
    }

    if (!powerOffFlag) {
        PMi_SetLCDPower(PM_LCD_POWER_ON, PM_LED_ON, TRUE, FALSE);
    }

    OS_DisableInterrupts();
    OS_SetIrqMask(prepIntrMask);
    OS_RestoreInterrupts(prepIntrMode);
    OS_RestoreIrq(prepIrq);

    if (powerOffFlag) {
        PM_ForceToPowerOff();
    }
}

int PMi_SetLCDPower(PMLCDPower sw, PMLEDStatus led, int skip, int isSync) {
    switch (sw) {
    case PM_LCD_POWER_ON:
        if (!skip && OS_GetVBlankCount() - PMi_LCDCount <= PMi_WAIT_FRAME) {
            return FALSE;
        }

        if (led != PM_LED_NONE) {
            if (isSync) {
                PMi_SetLED(led);
            } else {
                PMi_SetLEDAsync(led, NULL, NULL);
            }
        }

        GXi_PowerLCD(TRUE);
        break;
    case PM_LCD_POWER_OFF:
        GXi_PowerLCD(FALSE);

        PMi_LCDCount = OS_GetVBlankCount();

        if (led != PM_LED_NONE) {
            if (isSync) {
                PMi_SetLED(led);
            } else {
                PMi_SetLEDAsync(led, NULL, NULL);
            }
        }
        break;
    }

    return TRUE;
}

int PM_SetLCDPower(PMLCDPower sw) {
    if (sw != PM_LCD_POWER_ON) {
        sw = PM_LCD_POWER_OFF;
    }

    return PMi_SetLCDPower(sw, PM_LED_NONE, FALSE, TRUE);
}

PMLCDPower PM_GetLCDPower(void) {
    return (REG_POWCNT1 & REG_GX_POWCNT_LCD_MASK) ? PM_LCD_POWER_ON : PM_LCD_POWER_OFF;
}

u32 PMi_SendLEDPatternCommandAsync(PMLEDPattern pattern, PMCallback callback, void *arg) {
    u32 pxi_send_data;

    if (!PMi_Lock()) {
        return PM_BUSY;
    }

    PMi_SetCallback(callback, arg);

    pxi_send_data = PMi_MakeData1(SPI_PXI_START_BIT | SPI_PXI_END_BIT, 0,
                                  SPI_PXI_COMMAND_PM_SELF_BLINK, pattern);
    PMi_SendPxiData(pxi_send_data);

    return PM_SUCCESS;
}

u32 PMi_SendLEDPatternCommand(PMLEDPattern pattern) {
    u32 commandResult;
    u32 sendResult = PMi_SendLEDPatternCommandAsync(pattern, PMi_DummyCallback, &commandResult);

    if (sendResult == PM_SUCCESS) {
        PMi_WaitBusy();
        return commandResult;
    }

    return sendResult;
}
