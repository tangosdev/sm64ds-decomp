//cpp
#include "nitro/mb/mb.h"

extern "C" {

/* This console's player index on the link (see func_0203d974.cpp). */
extern unsigned char data_020a0f10;

/* Nonzero only on the host: not a download-play child
   (MB_IsMultiBootChild) and player 0. The inverse of func_0203d974. */
int func_0203d9b4(void) {
    if (MB_IsMultiBootChild())
        return 0;
    return data_020a0f10 == 0;
}
}
