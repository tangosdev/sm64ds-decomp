// @symbol func_ov002_020b8d3c
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjPushblock_c.h"
// @emits daObjPushblock_c_OnPushed
/* recovered: renamed to Class_Method */
/* daObjPushblock_c::OnPushed - recovered from vtable slot identity */
void daObjPushblock_c_OnPushed(char* r0, char* r1){
    struct daObjPushblock_c *self = (struct daObjPushblock_c *)(void *)r0;
  if(r1==0) return;
  self->unk_094=*(short*)(r1+0x8e);
  if(*(int*)(r1+8)==2) self->unk_098=0x8000;
  else self->unk_098=0x4000;
}
