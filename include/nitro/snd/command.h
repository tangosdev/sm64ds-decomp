#ifndef NITRO_SND_COMMAND_H
#define NITRO_SND_COMMAND_H
#ifndef __cplusplus
#define SND_CommandInit _Z15SND_CommandInitv
#endif

/*
 * The ARM9 sound command record of the Nitro SDK's SND_command.c, under the
 * SDK's own name (nitro/snd/ARM9/command.h). The queue lives in
 * src/engine/snd/SND_Command.cpp; src/engine/snd/SND_Interface.cpp packs each
 * call's arguments into one.
 *
 * SND_CommandInit is the SDK's (ntrtwl/NitroSDK
 * nitro/snd/common/command.h); SND_Init runs it once.
 */

#include "types.h"

/* One command for the ARM7 sound driver: SND_CMD_* id and up to four
 * arguments, chained through next on the free, reserve and waiting lists. */
typedef struct SNDCommand {
    struct SNDCommand *next; /* 0x00 */
    s32 id;                  /* 0x04 */
    u32 arg[4];              /* 0x08 */
} SNDCommand;

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

void SND_CommandInit(void);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif /* NITRO_SND_COMMAND_H */
