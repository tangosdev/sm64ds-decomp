#include "types.h"
#include "nitro/snd/alarm.h"
#include "nitro/snd/command.h"

/* NitroSDK SND_main.c SND_Init, as in pret's pokediamond, once only. This
 * older SDK has no sound mutex, so SND_Init is the file's one function. */
extern s32 data_020a6480; /* sSndInitialized */

void SND_Init(void) {
    if (data_020a6480)
        return;
    data_020a6480 = 1;
    SND_CommandInit();
    SND_AlarmInit();
}
