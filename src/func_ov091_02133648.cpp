//cpp
// @symbol func_ov091_02133648
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjPile_c.h"
// recovered name: daObjPile_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* daObjPile_c::OnGroundPounded - verified 2004/b56 byte-match (ov091), strict-reloc */
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* v);
extern void func_ov091_021334b8(void* c, int f);
void func_ov091_02133648(char* c, void* arg){
    struct daObjPile_c *self = (struct daObjPile_c *)(void *)c;
  if(arg==0) return;
  if(self->unk_31e==0) return;
  if(self->unk_31f) return;
  _ZN5Sound9PlayBank3EjRK7Vector3(0x62, c+0x74);
  int f = 0;
  if(*(int*)((char*)arg+8)==2) goto set;
  if(*(unsigned char*)((char*)arg+0x703)==0) goto done;
set:
  f=1;
done:
  func_ov091_021334b8(c, f);
}
}
