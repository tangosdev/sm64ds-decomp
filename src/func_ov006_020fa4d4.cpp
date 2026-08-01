//cpp
// @symbol func_ov006_020fa4d4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMCarlo2_c.h"
// recovered name: dScMgMCarlo2_c_OnYoshiTryEat_020fa4d4
/* recovered: renamed to Class_Method */
/* dScMgMCarlo2_c::OnYoshiTryEat - recovered from vtable slot identity */
extern "C" {
extern void func_ov006_020c1604(char* c, int unused, short a2, void* a3);
void func_ov006_020fa4d4(char* c) {
    struct dScMgMCarlo2_c *self = (struct dScMgMCarlo2_c *)(void *)c;
  func_ov006_020f9760(c + 0x51a8);
  data_ov006_0213d6fc = 0;
  self->unk_592e = 0;
  self->unk_511e = 1;
  func_ov006_020c1604(c + 0x4f38, 4, 4, c + 0x592e);
  self->unk_4f52 = 1;
  self->unk_592a = 0;
  func_ov004_020adb1c(0);
  self->unk_5928 = 1;
}
}
