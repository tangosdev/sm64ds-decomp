// @symbol func_ov081_02124e64
// recovered name: Spindrift_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daHuwa_c::Kill - recovered from vtable slot identity */
extern int func_ov081_02125488();
extern int data_ov081_02128e64[];
int func_ov081_02124e64(char *c){
  func_ov081_0212423c(c, 0);
  if(*(unsigned short*)(c+0x100)==0)
    func_ov081_02125488(c, data_ov081_02128e34);
  if(func_ov081_021245e8(c) > 0x320000)
    func_ov081_02125488(c, data_ov081_02128e64);
  return 1;
}
