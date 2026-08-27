// @symbol func_ov064_02118d3c
// recovered name: LavaBubble_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daBbl_c::Kill - recovered from vtable slot identity */
extern char *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void func_ov064_02118d3c(char *c){
  if (*(unsigned char*)(c+0x33a) != 0) {
    unsigned int id = *(unsigned int*)(c+0x320);
    if (id != 0) {
      char *a = _ZN8dActor_c10FindWithIDEj(id);
      if (a != 0 && *(unsigned char*)(a+0xd5) == 1) {
        *(char*)(c+0x33a) = 0;
        func_ov064_02118c48(c);
      }
    }
  }
  if (*(unsigned short*)(c+0x334) >= 0x18)
    *(char*)(c+0x339) = 0;
}
