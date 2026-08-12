// @symbol func_ov006_0210c674
// recovered name: dScMgSlot1_c_OnYoshiTryEat_0210c674
/* dScMgSlot1_c::OnYoshiTryEat - recovered from vtable slot identity. Slot 18
   stays an unmigrated raw extern "C" helper (see include/dScMgSlot1_c.h),
   so this reaches its own fields via raw offsets on a char* rather than
   including the now-C++ class header -- 0x4706 is touched only here, not
   by any migrated method, same precedent dScMgPachinko2_c.h documents for
   its own offset 0xbc. */
#include "decl_common.h"

void func_ov006_0210c674(char* c, int i){
  if(i == 4){
    *(unsigned char*)(c + 0x4706) = *(unsigned char*)(c + 0x4709);
  } else if(i == 3){
    func_ov006_0210c638(c);
  }
  func_ov006_0210c354(c+0x4660);
  *(int*)(c + 0x46b4) = 0;
}
