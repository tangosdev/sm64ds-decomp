// @symbol func_ov006_0210c674
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgSlot1_c.h"
// recovered name: dScMgSlot1_c_OnYoshiTryEat_0210c674
/* recovered: renamed to Class_Method */
/* dScMgSlot1_c::OnYoshiTryEat - recovered from vtable slot identity */
void func_ov006_0210c674(char* c, int i){
    struct dScMgSlot1_c *self = (struct dScMgSlot1_c *)(void *)c;
  if(i == 4){
    self->unk_4706 = self->unk_4709;
  } else if(i == 3){
    func_ov006_0210c638(c);
  }
  func_ov006_0210c354(c+0x4660);
  self->unk_46b4 = 0;
}
