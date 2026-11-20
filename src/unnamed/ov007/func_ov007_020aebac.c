#include "types.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"

/* 1 when no save file is in use and the status word is still 0. */
int func_ov007_020aebac(void) {
    Ov007BootArgs *args = gMenuWork->settings->bootArgs;
    u8 *used = args->fileUsed;
    if (used[0] == 0 && used[1] == 0 && used[2] == 0 && args->status == 0)
        return 1;
    return 0;
}
