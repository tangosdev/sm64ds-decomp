#ifndef NITRO_OS_TICK_H
#define NITRO_OS_TICK_H
#ifndef __cplusplus
#define OS_GetTick _Z10OS_GetTickv
#define OS_InitTick _Z11OS_InitTickv
#endif

/*
 * The system tick of the Nitro SDK's os_tick.c, under the SDK's own names
 * (ntrtwl/NitroSDK nitro/os/common/tick.h). The bodies are in
 * src/engine/os/OS_Tick.cpp. A tick is 64 system-clock cycles: timer 0 runs
 * at prescaler 64 and its overflow interrupt counts the high bits.
 *
 * The SDK returns the count as its OSTick, a u64. The definition and
 * decl_common.h spell it s64, so this declaration does too; a second
 * spelling would not compile beside theirs.
 */

#include "types.h"


void OS_InitTick(void);
s64 OS_GetTick(void);


#endif /* NITRO_OS_TICK_H */
