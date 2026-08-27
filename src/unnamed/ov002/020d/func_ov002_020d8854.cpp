//cpp
// @symbol func_ov002_020d8854
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void Vec3_RotateYAndTranslate(struct Vector3* out, struct Vector3* in, int ang, struct Vector3* tr);
extern int func_ov002_020d8944(char* a, char* b, char* other);
extern void func_ov002_020d8d10(char* self, struct Vector3* v);

void func_ov002_020d8854(char* self){
  char* other = (char*)_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(self+0x338));
  if (other == 0) return;
  {
    int b = (int)(*(unsigned short*)(other+0xc) == 0xbf);
    if (b != 0) {
      if (*(unsigned char*)(other+0x6d8) <= *(unsigned char*)(self+0x6d8)) return;
      if ((*(int*)(self+0x334) & 0x3c0) == 0) return;
      if (func_ov002_020d8944(self, self, other) != 0) {
        *(unsigned char*)(other+0x726) = 1;
        *(unsigned char*)(self+0x726) = 1;
      }
      return;
    }
  }
  {
    struct Vector3 out;
    struct Vector3 tr;
    struct Vector3 cp;
    tr.x = 0;
    tr.y = 0x50000;
    tr.z = 0x64000;
    Vec3_RotateYAndTranslate(&out, (struct Vector3*)(self+0x5c), *(short*)(self+0x8e), &tr);
    cp = out;
    func_ov002_020d8d10(self, &cp);
  }
}
}
