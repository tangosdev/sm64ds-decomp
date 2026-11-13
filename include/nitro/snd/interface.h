#ifndef NITRO_SND_INTERFACE_H
#define NITRO_SND_INTERFACE_H
#ifndef __cplusplus
#define SNDi_SetSurroundDecay _Z21SNDi_SetSurroundDecayi
#endif

/*
 * The ARM9 sound command calls of the Nitro SDK's SND_interface.c, under the
 * SDK's own names (ntrtwl/NitroSDK nitro/snd/ARM9/interface.h). The
 * bodies are in src/engine/snd/SND_Interface.cpp; each queues one command for
 * the ARM7 driver.
 *
 * SNDi_SetSurroundDecay sets the decay of the driver's surround effect. The
 * capture effect sends 0x3000 when it enters its kind 1, and 0 when it leaves
 * it.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif

void SNDi_SetSurroundDecay(s32);

#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif /* NITRO_SND_INTERFACE_H */
