// @symbol func_ov002_020ecfc8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;
typedef short s16;

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, Fix12i b, unsigned int d);
extern int func_ov002_020ec654(void* c);
extern void func_0203568c(void* p, int v);
extern s16 Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
extern void func_ov002_020edca4(void* c);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* mc, void* a, Fix12i r, unsigned int h, unsigned int x, unsigned int y);
extern void* data_ov002_0210e6b0[];
void func_ov002_020ecfc8(char* c){
  char* r5;
  *(int*)(c + 0x410) = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov002_0210e6b0[1], 0, 0x1000, 0);
  *(int*)(c + 0x98) = 0x64000;
  if (!func_ov002_020ec654(c)){
    *(int*)(c + 0x9c) = 0;
    r5 = (char*)func_ov002_020edb3c(c, 0, 0x7d0000);
    func_0203568c(c + 0x144, 0x2a000);
  } else {
    *(int*)(c + 0x9c) = -0xa000;
    r5 = (char*)func_ov002_020edb3c(c, 0, 0xfa0000);
  }
  if (r5){
    *(s16*)(c + 0x8e) = Vec3_HorzAngle((struct Vector3*)(c + 0x5c), (struct Vector3*)(r5 + 0x5c));
  } else {
    if (*(unsigned char*)(c + 0x41c)){
      if (!func_ov002_020ec654(c))
        func_ov002_020edca4(c);
    }
  }
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x64000, 0xc8000, 0x202000, 0);
  *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);
}
