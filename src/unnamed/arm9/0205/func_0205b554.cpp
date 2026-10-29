//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

/* NitroSDK SNDSharedWork, the ARM9/ARM7 shared sound status block
 * (data_020a64e0, which func_0205b358 = SND_CommandInit hands here and
 * points data_020a7fc0 at; func_0205b5d4 and func_0205b608 read its first two
 * words back after invalidating all 0x280 bytes). SDK names. */
typedef struct SNDSharedPlayer {
    s16 variable[16]; /* 0x00 */
    u32 tickCounter;  /* 0x20 */
} SNDSharedPlayer;

typedef struct SNDSharedWork {
    u32 finishCommandTag;       /* 0x000 */
    u32 playerStatus;           /* 0x004 */
    u16 channelStatus;          /* 0x008 */
    u16 captureStatus;          /* 0x00a */
    u32 padding[5];             /* 0x00c */
    SNDSharedPlayer player[16]; /* 0x020 */
    s16 globalVariable[16];     /* 0x260 */
} SNDSharedWork;

/* NitroSDK SNDi_InitSharedWork(work): clear the status words and tick
 * counters, set every player and global variable to SND_DEFAULT_VARIABLE
 * (-1) and flush the block for ARM7. */
void func_0205b554(char *p)
{
    SNDSharedWork *work = (SNDSharedWork *)p;
    int i, j;

    work->playerStatus = 0;
    work->channelStatus = 0;
    work->captureStatus = 0;
    work->finishCommandTag = 0;

    for (i = 0; i < 16; i++) {
        work->player[i].tickCounter = 0;
        for (j = 0; j < 16; j++)
            work->player[i].variable[j] = -1;
    }

    for (j = 0; j < 16; j++)
        work->globalVariable[j] = -1;

    CP15::FlushAndInvalidateDataCache((u32)work, sizeof(SNDSharedWork));
}
}
