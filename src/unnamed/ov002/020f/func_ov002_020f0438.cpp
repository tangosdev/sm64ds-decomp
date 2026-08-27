//cpp
// @symbol func_ov002_020f0438
/* recovered: shared common types */
#include "common.h"

struct dActor_c {
  char pad[0x5c];
  Vector3 pos;          // 0x5c
  char pad2[160];       // 0x68 .. 0x108
  unsigned int id;      // 0x108
  char b10e_pad[2];     // 0x10c .. 0x10e
  unsigned char b10e;   // 0x10e
  unsigned char b10f;   // 0x10f
  unsigned char b110;   // 0x110
  unsigned char b111[2];
  unsigned char b113;   // 0x113
};
extern "C" {
dActor_c* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void func_02012790(int);
short _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(dActor_c* thiz, const Vector3& v, unsigned int a, bool b, unsigned short c, dActor_c* d);
void _ZN7fBase_c18MarkForDestructionEv(dActor_c* thiz);

void func_ov002_020f0438(dActor_c* thiz)
{
  dActor_c* o = _ZN8dActor_c10FindWithIDEj(thiz->id);
  if (!o) return;
  if (thiz->b10e != 0xf && thiz->b10e != o->b110) return;
  func_02012790(0x25);
  (*(unsigned char*)((int)o + 0x110))++;
  {
    Vector3 v;
    v.x = thiz->pos.x;
    v.y = thiz->pos.y;
    v.z = thiz->pos.z;
    _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(thiz, v, o->b110, false, 0, 0);
  }
  *(int*)((int)thiz + 0xec) |= 1;
  if (o->b110 == 5) { thiz->b113 = 0x1e; return; }
  if (thiz->b10f != 2) return;
  _ZN7fBase_c18MarkForDestructionEv(thiz);
}
}
