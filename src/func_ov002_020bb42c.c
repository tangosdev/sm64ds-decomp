#include "types.h"
// @symbol func_ov002_020bb42c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern int _ZN6Player7TryGrabER5Actor(char* p, char* a);

void func_ov002_020bb42c(char* self){
  char* other;
  unsigned int id = *(unsigned int*)(self+0x344);
  if (id == 0) return;
  other = (char*)_ZN5Actor10FindWithIDEj(id);
  if (other == 0) return;
  {
    int b = (int)(*(u16*)(other+0xc) == 0xbf);
    if (b == 0) return;
  }
  if ((*(int*)(self+0x340) & 0x40000) != 0) {
    *(u8*)(self+0x58c) = 0x3c;
    return;
  }
  {
    int ang = Vec3_HorzAngle((struct Vector3*)(self+0x5c), (struct Vector3*)(other+0x5c));
    if (*(int*)(other+8) != 2) return;
    if (AngleDiff(ang, *(s16*)(self+0x8e)) <= 0x4000) return;
  }
  if ((*(int*)(self+0x340) & 0x1000) == 0) return;
  if (_ZN6Player7TryGrabER5Actor(other, self) == 0) return;
  *(int*)(self+0x59c) = (int)other;
  func_ov002_020bbd5c(self, 2);
}
