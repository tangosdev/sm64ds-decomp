//cpp
/* Console detection (the Nitro SDK's os_emulator.c), arm9 .text
 * 0x0205865c..0x020587e4.
 *
 * OS_GetConsoleType works out once what the game runs on, caches it in
 * OSi_ConsoleTypeCache (data_0209a040) and returns it. The high nibble says
 * which machine it is, the device bits say whether a development cartridge
 * is in the GBA slot, and the low bits give the main memory size.
 *
 * pret's pmd-sky os_emulator.c defines OS_IsRunOnEmulator,
 * OS_GetConsoleType, OSi_GetDeviceType and OSi_IsRunOnDebugger in this
 * order. mwccarm emits the bodies in reverse, so the ROM runs from
 * OSi_IsRunOnDebugger at 0x0205865c up to OS_IsRunOnEmulator at 0x020587dc.
 * Here OS_IsRunOnEmulator always returns FALSE, the same as pret's
 * pokediamond, while OS_GetConsoleType still does the full detection.
 */
#include "IRQ.h"
#include "nitro/os/spinLock.h"

#define OSi_CONSOLE_NOT_DETECT 0xffffffff

#define OS_CONSOLE_ENSATA 0x10000000
#define OS_CONSOLE_ISEMULATOR 0x20000000
#define OS_CONSOLE_ISDEBUGGER 0x40000000
#define OS_CONSOLE_NITRO 0x80000000

#define OS_CONSOLE_DEV_CARTRIDGE 0x01000000
#define OS_CONSOLE_DEV_CARD 0x02000000

#define HW_CTRDG_ROM 0x08000000
#define HW_CHECK_DEBUGGER_BUF1 0x027ff814
#define HW_CHECK_DEBUGGER_SW 0x027ffc10
#define HW_CHECK_DEBUGGER_BUF2 0x027ffc14
#define HW_MMEMCHECKER_SUB 0x027ffffa

extern "C" {

extern int data_0209a040; /* OSi_ConsoleTypeCache, initialised to -1 */

int OSi_GetDeviceType(void);
int OSi_IsRunOnDebugger(void);

int OS_IsRunOnEmulator(void) {
    return 0;
}

int OS_GetConsoleType(void) {
    if (data_0209a040 == (int)OSi_CONSOLE_NOT_DETECT) {
        int type = OSi_GetDeviceType();

        if (OS_IsRunOnEmulator())
            type |= OS_CONSOLE_ENSATA;
        else if (OSi_IsRunOnDebugger())
            type |= OS_CONSOLE_ISDEBUGGER;
        else if (type & OS_CONSOLE_DEV_CARTRIDGE)
            type |= OS_CONSOLE_ISEMULATOR;
        else
            type |= OS_CONSOLE_NITRO;

        data_0209a040 = type | *(u16 *)HW_MMEMCHECKER_SUB;
    }
    return data_0209a040;
}

int OSi_GetDeviceType(void) {
    int result;
    int checked = 0;
    u16 lockId;
    s32 ret;
    u32 enabled;

    lockId = (u16)OS_GetLockID();
    result = checked;

    do {
        ret = -1;
        enabled = IRQ::Disable();

        if ((OS_ReadOwnerOfLockCartridge() & OS_MAINP_LOCKED_FLAG) != 0 ||
            (ret = OS_TryLockCartridge(lockId)) == 0) {
            if (((u32 *)HW_CTRDG_ROM)[0] == 'TNIN' && ((u32 *)HW_CTRDG_ROM)[1] == 'ODNE')
                result = OS_CONSOLE_DEV_CARTRIDGE;
            else
                result = OS_CONSOLE_DEV_CARD;

            if (ret == 0) {
                OS_UnlockCartridge(lockId);
                checked = 1;
            }
        }

        IRQ::Restore(enabled);
    } while (checked == 0);

    return result;
}

int OSi_IsRunOnDebugger(void) {
    u32 checkAddress;
    if (*(volatile u16 *)HW_CHECK_DEBUGGER_SW == 0)
        checkAddress = HW_CHECK_DEBUGGER_BUF1;
    else
        checkAddress = HW_CHECK_DEBUGGER_BUF2;
    return *(volatile u16 *)checkAddress == 1;
}
}
