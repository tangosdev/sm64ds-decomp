//cpp
#include "nitro/mb/mb.h"

extern "C" {

extern unsigned short data_020a0f1c;

/* Check a file operation's result: nonzero passes (1). Zero sets link
   error bit 0x80 in data_020a0f1c and returns whether this is a
   download-play child (MB_IsMultiBootChild), which cannot read the
   card again; func_02018e68 then sleeps the thread (see myFS_ReadFile.c). */
int func_0203d7d4(int arg) {
    if (arg != 0)
        return 1;
    data_020a0f1c |= 0x80;
    return MB_IsMultiBootChild() != 0;
}
}
