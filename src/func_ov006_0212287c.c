// @symbol func_ov006_0212287c
/* recovered: shared common types */
#include "common.h"
extern void _Z15ApproachLinear2Rsss(short* a, short b, short c);

extern void AddVec3(struct Vector3* a, struct Vector3* b, struct Vector3* c);
void func_ov006_0212287c(unsigned char* o){
  _Z15ApproachLinear2Rsss((short*)(o+0x76), 0, 1);
  short* p = (short*)(((int)o + 0x74) & 0xFFFFFFFFFFFFFFFFULL);
  *p = *p + 0x400;
  AddVec3((struct Vector3*)(o+0x5c), (struct Vector3*)(o+0x68), (struct Vector3*)(o+0x5c));
}
