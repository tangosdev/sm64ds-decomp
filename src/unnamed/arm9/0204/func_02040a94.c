#include "private/arm9_a1fc0_file_share.h"

/* The file share's mState: 2 once it is ready.
   func_0203db64 waits for that before the download-play handshakes. */
int func_02040a94(void) {
    return data_020a1fc0.mState;
}
