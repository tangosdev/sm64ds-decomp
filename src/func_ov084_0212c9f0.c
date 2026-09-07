// @symbol func_ov084_0212c9f0
/* recovered: shared common types */
#include "common.h"
typedef short s16;

extern void func_02012694(int a, void* p, int c);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* self, void* ab, unsigned int id, const struct Vector3* v, unsigned int a, unsigned int b);

void func_ov084_0212c9f0(char* c, int arg1, unsigned int arg2) {
  void* player = *(void**)(c + 0x194);
  struct Vector3 v;
  int x = *(int*)(c + 0x5c);
  int z = *(int*)(c + 0x64);
  int y = *(int*)(c + 0x60) + 0x32000;
  v.x = x;
  v.y = y;
  v.z = z;
  func_02012694(0x108, c + 0x74, arg2);
  _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, c, (s16)arg1, &v, arg2, 0);
}