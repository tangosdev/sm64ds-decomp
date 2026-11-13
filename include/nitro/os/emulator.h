#ifndef NITRO_OS_EMULATOR_H
#define NITRO_OS_EMULATOR_H
#ifndef __cplusplus
#define OS_GetConsoleType _Z17OS_GetConsoleTypev
#define OS_IsRunOnEmulator _Z18OS_IsRunOnEmulatorv
#endif

/*
 * Console detection, the Nitro SDK's os_emulator.c, under the SDK's own
 * names (ntrtwl/NitroSDK nitro/os/common/emulator.h). The bodies are
 * in src/engine/os/OS_Emulator.cpp.
 *
 * OS_GetConsoleType answers the machine in the high nibble, the device in
 * the GBA slot in bits 24-25 and the main memory size in the low bits.
 * OS_IsRunOnEmulator answers the SDK's BOOL as an int.
 */

#include "types.h"

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

/* OSi_ConsoleTypeCache before the first OS_GetConsoleType. */
#define OSi_CONSOLE_NOT_DETECT 0xffffffff

#define OS_CONSOLE_ENSATA     0x10000000
#define OS_CONSOLE_ISEMULATOR 0x20000000
#define OS_CONSOLE_ISDEBUGGER 0x40000000
#define OS_CONSOLE_NITRO      0x80000000

#define OS_CONSOLE_DEV_CARTRIDGE 0x01000000
#define OS_CONSOLE_DEV_CARD      0x02000000

#define OS_CONSOLE_SIZE_MASK 0x00000003
#define OS_CONSOLE_SIZE_8MB  0x00000002

int OS_IsRunOnEmulator(void);
u32 OS_GetConsoleType(void);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif /* NITRO_OS_EMULATOR_H */
