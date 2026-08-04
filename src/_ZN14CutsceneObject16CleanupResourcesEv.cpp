//cpp
// @symbol _ZN14CutsceneObject16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "CutsceneObject.h"
extern "C" {
int func_ov002_020f63a0(void* thiz);
int func_ov002_020f23d0(void* c);
}
struct Obj { virtual void v00(); virtual void m04(); };

int CutsceneObject::CleanupResources()
{
  int r1 = unk_008;
  if (r1 == 0x2e) return func_ov002_020f63a0(((char*)this));
  if (r1 == 0x2f) return func_ov002_020f23d0(((char*)this));
  Obj* a = *(Obj**)((char*)&mModel);
  if (a) if (a) a->m04();
  Obj* b = *(Obj**)((char*)&unk_0e0);
  if (b) if (b) b->m04();
  return 1;
}
