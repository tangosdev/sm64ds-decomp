//cpp
/* The sound driver's shared work block (the Nitro SDK's SND_work.c), arm9
 * .text 0x0205b554..0x0205b63c.
 *
 * SNDi_SharedWork (data_020a7fc0) points at the SNDSharedWork block the ARM7
 * driver writes its status into. SND_CommandInit points it at
 * data_020a64e0 and SNDi_InitSharedWork clears it. The readers invalidate
 * the cache over the block before they read it.
 *
 * pret's pokediamond SND_work.c defines SND_GetPlayerStatus,
 * SND_GetChannelStatus, SND_GetPlayerTickCounter,
 * SNDi_GetFinishedCommandTag and SNDi_InitSharedWork in this order. mwccarm
 * emits the bodies in reverse, so SNDi_InitSharedWork sits lowest. The
 * linker dropped the two the game never calls. This SDK is older than
 * pret's: each reader invalidates the whole block, not just the word it
 * reads.
 */
#include "types.h"
#include "nitro/os/cache.h"



#define SND_PLAYER_COUNT 16
#define SND_PLAYER_VARIABLE_COUNT 16
#define SND_GLOBAL_VARIABLE_COUNT 16
#define SND_DEFAULT_VARIABLE -1

typedef struct SNDSharedPlayer {
    s16 variable[SND_PLAYER_VARIABLE_COUNT]; /* 0x00 */
    u32 tickCounter;                         /* 0x20 */
}
 SNDSharedPlayer;

typedef struct SNDSharedWork {
    u32 finishCommandTag;                          /* 0x000 */
    u32 playerStatus;                              /* 0x004 */
    u16 channelStatus;                             /* 0x008 */
    u16 captureStatus;                             /* 0x00a */
    u32 padding[5];                                /* 0x00c */
    SNDSharedPlayer player[SND_PLAYER_COUNT];      /* 0x020 */
    s16 globalVariable[SND_GLOBAL_VARIABLE_COUNT]; /* 0x260 */
}
 SNDSharedWork;

extern SNDSharedWork *data_020a7fc0;
 /* SNDi_SharedWork */

u32 SND_GetPlayerStatus(void) {
    DC_InvalidateRange(data_020a7fc0, sizeof(SNDSharedWork));
    return data_020a7fc0->playerStatus;
}

u32 SNDi_GetFinishedCommandTag(void) {
    DC_InvalidateRange(data_020a7fc0, sizeof(SNDSharedWork));
    return data_020a7fc0->finishCommandTag;
}

void SNDi_InitSharedWork(SNDSharedWork *sw) {
    int i, j;

    sw->playerStatus = 0;
    sw->channelStatus = 0;
    sw->captureStatus = 0;
    sw->finishCommandTag = 0;

    for (i = 0; i < SND_PLAYER_COUNT; i++) {
        sw->player[i].tickCounter = 0;
        for (j = 0; j < SND_PLAYER_VARIABLE_COUNT; j++)
            sw->player[i].variable[j] = SND_DEFAULT_VARIABLE;
    }

    for (j = 0; j < SND_GLOBAL_VARIABLE_COUNT; j++)
        sw->globalVariable[j] = SND_DEFAULT_VARIABLE;

    DC_FlushRange(sw, sizeof(SNDSharedWork));
}
