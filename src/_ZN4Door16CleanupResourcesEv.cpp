//cpp
// @symbol _ZN4Door16CleanupResourcesEv
// recovered name: Door::CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "SharedFilePtr.h"
/* recovered: renamed to Class_Method, vtable slot 3 */
/* Door::CleanupResources -- vtable slot 3, ov100 0x0214542c. Same idiom as
 * _ZN4Door13InitResourcesEv.c: declared as an override in include/Door.h,
 * defined here as an extern "C" free function under the mangled symbol so
 * nothing about the field layout has to change to land the right name. */
struct V { virtual void v0(); virtual void v1(); };
struct Elem { void* a; void* b; char pad[8]; };
extern Elem data_ov100_02148204[];
extern "C" {
extern void* data_ov100_02148744;
int _ZN4Door16CleanupResourcesEv(char* c) {
  int idx = *(int*)(c + 8);
  Elem* e = &data_ov100_02148204[idx];
  V* obj;
  ((SharedFilePtr *)(e->a))->Release();
  ((SharedFilePtr *)(&data_ov100_02148744))->Release();
  obj = (V*)*(void**)(c + 0x138);
  if (obj != 0) {
    if (obj != 0) obj->v1();
    ((SharedFilePtr *)(e->b))->Release();
  }
  if (*(void**)(c + 0x13c) != 0) {
    unsigned int v = *(unsigned int*)(c + 8);
    if (v >= 9 && v <= 0xd) UnloadKeyModels(v - 7);
    ((SharedFilePtr *)(*(void**)(c + 0x13c)))->Release();
  }
  return 1;
}
}
