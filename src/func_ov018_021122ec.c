// @symbol func_ov018_021122ec
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjSm_Lift_c.h"
// recovered name: daObjSm_Lift_c_AfterClsn
/* recovered: renamed to Class_Method */
/* daObjSm_Lift_c::AfterClsn - recovered from vtable slot identity */
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov018_02113be8[];
extern int data_ov018_02113bf0[];
int func_ov018_021122ec(char* c){
    struct daObjSm_Lift_c *self = (struct daObjSm_Lift_c *)(void *)c;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov018_02113bf0[1], 0, 0x1000, 0);
  self->unk_130=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x138, data_ov018_02113be8[1], 0, 0x1000, 0);
  self->unk_098=0;
  self->unk_374=0;
  self->unk_37c=0;
  return 1;
}
