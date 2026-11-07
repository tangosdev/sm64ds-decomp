#include "private/player_input_record.h"

extern unsigned short data_020a0f30;
/* The link handshake's poll (func_020308a8): keep flag 0x1000 raised in this
   console's input record (data_020a1040.flags) and return 1, clearing the
   latch, once func_0203ea5c has set data_020a0f30 -- which it does when every
   present player's record carries 0x1000. */
int func_0203d81c(void) {
    data_020a1040.flags |= 0x1000;
    if (data_020a0f30 != 0) {
        data_020a0f30 = 0;
        return 1;
    }
    return 0;
}
