#include "private/player_input_record.h"

/* Raise the link handshake flag, 0x1000 of this console's input record
   (func_020308b4; func_0203d81c polls it). */
void func_0203d868(void) {
    data_020a1040.flags |= 4096;
}
