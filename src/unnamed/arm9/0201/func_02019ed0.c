#include "types.h"
#include "nitro/spi/pm.h"
extern u8 data_0209d4e4;
/* Sleep with the lid closed. data_0209d4e4 is up while it sleeps (the
   pull-out thread func_02019f64 reads it). It wakes on the lid opening or
   the card being pulled. The lid check func_02019ac4 calls it. */
void func_02019ed0(void){
  data_0209d4e4 = 1;
  PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, 0, 0);
  data_0209d4e4 = 0;
}
