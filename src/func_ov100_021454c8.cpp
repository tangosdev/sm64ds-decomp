//cpp
// @symbol func_ov100_021454c8
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daDoor_c.h"
// @emits daDoor_c_Render
/* recovered: renamed to Class_Method */
/* daDoor_c::Render - recovered from vtable slot identity */
extern "C" {
extern unsigned char IsAreaShowing(int idx);
}
struct V1 { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct V2 { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void n(int); virtual void m(int); };
extern "C" int daDoor_c_Render(char* c){
    struct daDoor_c *self = (struct daDoor_c *)(void *)c;
  if(IsAreaShowing((char)self->unk_08c)==0){
    if(IsAreaShowing((char)self->unk_090)==0) goto done;
  }
  {
    V1* a = (V1*)(c+0xd4);
    a->m(0);
    V2* o = *(V2**)(c+0x138);
    if(o==0) goto done;
    o->n(self->unk_0e8);
    V1* o2 = *(V1**)(c+0x138);
    o2->m(0);
  }
done:
  return 1;
}
