#include "private/player_input_record.h"

extern unsigned short data_020a0f1c;
/* Clear the link error bits 0x9f of data_020a0f1c and unk22 (byte 0x22) of
   the local input record data_020a1040. func_02030aa4 calls it when a session
   starts, and dMeter_c when it sets up. func_0203ea5c runs the same three
   statements inline in its error countdown, so the two shared a translation
   unit. The cartridge stores unk22 twice here as well. */
void func_0203da4c(void) {
    data_020a0f1c &= 0xff60;
    data_020a1040.unk22 = 0;
    data_020a1040.unk22 = 0;
}
