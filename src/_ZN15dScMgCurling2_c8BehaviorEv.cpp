//cpp
// @symbol _ZN15dScMgCurling2_c8BehaviorEv
#include "dScMgCurling2_c.h"
// recovered name: dScMgCurling2_c_Behavior
/* dScMgCurling2_c::Behavior - recovered from vtable slot identity. Indexed
   adjustor-thunk dispatch, kept exactly as recovered -- the table's own
   element type isn't evidenced beyond {int,int}. */
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02141a18[];
int func_ov006_020e4800(void*);
int func_ov006_020e3948(void*);
}
s32 dScMgCurling2_c::Behavior()
{
  char *c = (char *)this;
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
