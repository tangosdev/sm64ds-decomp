//cpp
// @symbol func_ov071_02121ba4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
#include "common.h"

extern "C" {
    extern void func_ov071_02121b08(void*);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, const struct Vector3 *v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);

void func_ov071_02121ba4(char *c){
  void *o;
  struct Vector3 v;
  int t;
  unsigned int id;
  id = *(unsigned int*)(c + 0x120);
  if(id == 0) return;
  o = _ZN8dActor_c10FindWithIDEj(id);
  if(o == 0) return;
  t = (*(unsigned short*)((char*)o + 0xc) == 0xbf);
  if(t == 0) return;
  if(*(int*)(c + 0x11c) & 0x8000) return;
  if(*(unsigned char*)((char*)o + 0x6fb)) return;
  v.x = *(int*)(c + 0x5c);
  v.y = *(int*)(c + 0x60);
  v.z = *(int*)(c + 0x64);
  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(o, &v, 1, 0xc000, 1, 0, 1);
  func_ov071_02121b08(c);
}
}
