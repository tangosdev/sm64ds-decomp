//cpp
// @symbol func_ov006_020e683c
// recovered name: dScMgCurling2_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgCurling2_c::Behavior - recovered from vtable slot identity */
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02141a18[];
int func_ov006_020e683c(char* c){
  int idx=*(int*)(c+0x5000+0x580);
  Ent* e=&data_ov006_02141a18[idx];
  int adj=e->b;
  char* obj=c+(adj>>1);
  int fn;
  if(adj&1){
    fn=*(int*)(*(int*)obj + e->a);
  } else {
    fn=e->a;
  }
  ((void(*)(void*))fn)(obj);
  func_ov006_020e4800(c);
  func_ov006_020e3948(c);
  return 1;
}
}
