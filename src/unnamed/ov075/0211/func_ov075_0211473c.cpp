//cpp
// @symbol func_ov075_0211473c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;

extern "C" s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
int ApproachLinear(short&,short,short);
extern "C" void func_ov075_0211473c(char* c){
  s16 a=Vec3_HorzAngle((Vector3*)(c+0x118),(Vector3*)(c+0x130));
  unsigned char idx=*(unsigned char*)(c+0x152);
  short lim=data_ov075_0211b52c[idx];
  if(ApproachLinear(*(short*)(c+0x150), a, lim)){
    *(int*)(c+0x110)=4;
    *(int*)(c+0x114)=0;
  }
}
