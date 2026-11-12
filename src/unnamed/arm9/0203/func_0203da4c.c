#include "private/player_input_record.h"

extern unsigned short data_020a0f1c;
/* Clear the link error bits 0x9f of data_020a0f1c and byte 0x22 of the
   local input record data_020a1040. func_02030aa4 calls it when a session
   starts, and dMeter_c when it sets up. The byte is addressed through a
   byte pointer: the field spelling data_020a1040.pad_22[0] reorders the
   literal pool and the registers (11 words differ). */
void func_0203da4c(void){
  unsigned int v = data_020a0f1c & 0xff60;
  *((unsigned char *)&data_020a1040 + 0x22) = 0;
  data_020a0f1c = v;
  *((unsigned char *)&data_020a1040 + 0x22) = 0;
}
