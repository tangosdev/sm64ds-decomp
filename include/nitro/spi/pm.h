#ifndef NITRO_SPI_PM_H
#define NITRO_SPI_PM_H

/*
 * The ARM9 side of the NitroSDK power-management PXI channel (spi/ARM9/pm.c),
 * defined in src/engine/spi/PM.c. Each function is named by its body
 * against pret's pokediamond and pret's pmd-sky (both carry this file as
 * spi_pm.s); that file's banner gives the evidence. Parameters stay unnamed:
 * a name here takes an @NNN in every C++ includer.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The SPI channel's PXI word layout and the PM command numbers
   (nitro/spi/common/type.h). */
#define SPI_PXI_START_BIT            0x02000000
#define SPI_PXI_END_BIT              0x01000000
#define SPI_PXI_INDEX_SHIFT          16
#define SPI_PXI_RESULT_COMMAND_MASK  0x00007f00
#define SPI_PXI_RESULT_COMMAND_SHIFT 8
#define SPI_PXI_RESULT_DATA_MASK     0x000000ff
#define SPI_PXI_RESULT_DATA_SHIFT    0

#define SPI_PXI_COMMAND_PM_SYNC        0x0060
#define SPI_PXI_COMMAND_PM_SLEEP_START 0x0061
#define SPI_PXI_COMMAND_PM_SLEEP_END   0x0062
#define SPI_PXI_COMMAND_PM_UTILITY     0x0063
#define SPI_PXI_COMMAND_PM_REG_READ    0x0065
#define SPI_PXI_COMMAND_PM_SELF_BLINK  0x0066
#define SPI_PXI_COMMAND_PM_GET_BLINK   0x0067
#define SPI_PXI_COMMAND_PM_REG0VALUE   0x0070
#define SPI_PXI_COMMAND_PM_REG3VALUE   0x0073

/* The power-management IC's registers (nitro/spi/common/pm_common.h). This
   SDK reads four of them; the later SDK's fifth, the backlight control, is
   not here. */
#define REG_PMIC_CTL_ADDR 0
#define PMIC_REG_NUMS     4

#define PMIC_CTL_BKLT1 (1 << 2) /* the bottom screen's backlight */
#define PMIC_CTL_BKLT2 (1 << 3) /* the top screen's backlight */

/* The ARM7's utility commands, sent by PM_SendUtilityCommandAsync. */
enum {
    PM_UTIL_DUMMY = 0,
    PM_UTIL_LED_ON,
    PM_UTIL_LED_BLINK_HIGH_SPEED,
    PM_UTIL_LED_BLINK_LOW_SPEED,
    PM_UTIL_LCD1_BACKLIGHT_ON,
    PM_UTIL_LCD1_BACKLIGHT_OFF,
    PM_UTIL_LCD2_BACKLIGHT_ON,
    PM_UTIL_LCD2_BACKLIGHT_OFF,
    PM_UTIL_LCD12_BACKLIGHT_ON,
    PM_UTIL_LCD12_BACKLIGHT_OFF,
    PM_UTIL_SOUND_POWER_ON,
    PM_UTIL_SOUND_POWER_OFF,
    PM_UTIL_SOUND_VOL_CTRL_ON,
    PM_UTIL_SOUND_VOL_CTRL_OFF,
    PM_UTIL_FORCE_POWER_OFF,
    PM_UTIL_FORCE_POWER_ON
};

/* PM_GoSleepMode's wake-up triggers. */
#define PM_TRIGGER_KEY       (1 << 0)
#define PM_TRIGGER_RTC_ALARM (1 << 1)
#define PM_TRIGGER_COVER_OPEN (1 << 2)
#define PM_TRIGGER_CARD      (1 << 3)
#define PM_TRIGGER_CARTRIDGE (1 << 4)

typedef u32 PMWakeUpTrigger;
typedef u32 PMLogic;

/* What every sender returns, and the results its callback is given. */
#define PM_SUCCESS         0
#define PM_BUSY            1
#define PM_INVALID_COMMAND 0xffff

#define PM_RESULT_SUCCESS 0
#define PM_RESULT_BUSY    1
#define PM_RESULT_ERROR   2

typedef enum { PM_LCD_TOP = 0, PM_LCD_BOTTOM = 1, PM_LCD_ALL = 2 } PMLCDTarget;

typedef enum { PM_BACKLIGHT_OFF = 0, PM_BACKLIGHT_ON = 1 } PMBackLightSwitch;

typedef enum { PM_LCD_POWER_OFF = 0, PM_LCD_POWER_ON = 1 } PMLCDPower;

typedef enum {
    PM_LED_NONE = 0,
    PM_LED_ON = 1,
    PM_LED_BLINK_LOW = 2,
    PM_LED_BLINK_HIGH = 3
} PMLEDStatus;

/* The LED patterns PMi_SendLEDPatternCommand gives the ARM7. */
typedef enum {
    PM_LED_PATTERN_NONE = 0,
    PM_LED_PATTERN_ON = 1,
    PM_LED_PATTERN_BLINK_LOW = 2,
    PM_LED_PATTERN_BLINK_HIGH = 3,
    PM_LED_PATTERN_BLINK1 = 4,
    PM_LED_PATTERN_BLINK2 = 5,
    PM_LED_PATTERN_BLINK3 = 6,
    PM_LED_PATTERN_BLINK4 = 7,
    PM_LED_PATTERN_BLINK5 = 8,
    PM_LED_PATTERN_BLINK6 = 9,
    PM_LED_PATTERN_BLINK8 = 10,
    PM_LED_PATTERN_BLINK10 = 11,
    PM_LED_PATTERN_PATTERN1 = 12,
    PM_LED_PATTERN_PATTERN2 = 13,
    PM_LED_PATTERN_PATTERN3 = 14,
    PM_LED_PATTERN_WIRELESS = 15
} PMLEDPattern;

typedef void (*PMCallback)(u32 result, void *arg);

/* NitroSDK pm.c's PMiWork, the channel state (PMi_Work). Every sender takes
   `lock`, stores its callback and argument, and sends; PMi_CommonCallback
   finishes through PMi_CallCallbackAndUnlock, which drops `lock` and calls
   callback(result, callbackArg). A SPI_PXI_COMMAND_PM_GET_BLINK reply is
   stored through `work`. */
typedef struct {
    int lock;
    PMCallback callback;
    void *callbackArg;
    void *work;
} PMiWork;
extern PMiWork PMi_Work;

/* NitroSDK's PMData16 (nitro/spi/ARM9/pm.h), one register-read slot of
   PMi_RegisterBuffer: a reply-arrived flag and the caller's destination for
   the reply. PM_Init clears every `flag`; PMi_ReadRegisterAsync clears one
   and stores the caller's buffer in `buffer`; PMi_CommonCallback stores the
   halfword reply to SPI_PXI_COMMAND_PM_REG0VALUE..REG3VALUE through `buffer`
   and sets `flag`. This SDK has four slots, not the later five.

   `buffer` also has its own symbol row, data_020a813c, because both of its
   accesses reach it at that address. As this struct's second field, mwcc
   folds the +4 into the pool literal and names PMi_RegisterBuffer with a RELA
   addend of 4, which resolves to the same 0x020a813c relocs.txt records. The
   row stays: dsd needs a symbol at every relocs.txt target. */
typedef struct {
    u16 flag;
    u16 padding;
    u16 *buffer;
} PMData16;
extern PMData16 PMi_RegisterBuffer[PMIC_REG_NUMS];

void PM_Init(void);
u32 PM_SendUtilityCommandAsync(u32, PMCallback, void *);
u32 PMi_ReadRegisterAsync(u16, u16 *, PMCallback, void *);
u32 PMi_ReadRegister(u16, u16 *);
u32 PMi_SetLEDAsync(PMLEDStatus, PMCallback, void *);
u32 PMi_SetLED(PMLEDStatus);
u32 PM_SetBackLightAsync(PMLCDTarget, PMBackLightSwitch, PMCallback, void *);
u32 PM_SetBackLight(PMLCDTarget, PMBackLightSwitch);
u32 PM_ForceToPowerOffAsync(PMCallback, void *);
u32 PM_ForceToPowerOff(void);
u32 PM_GetBackLight(PMBackLightSwitch *, PMBackLightSwitch *);
void PM_GoSleepMode(PMWakeUpTrigger, PMLogic, u16);
int PMi_SetLCDPower(PMLCDPower, PMLEDStatus, int, int);
int PM_SetLCDPower(PMLCDPower);
PMLCDPower PM_GetLCDPower(void);
u32 PMi_SendLEDPatternCommandAsync(PMLEDPattern, PMCallback, void *);
u32 PMi_SendLEDPatternCommand(PMLEDPattern);

#ifdef __cplusplus
}
#endif

#endif
