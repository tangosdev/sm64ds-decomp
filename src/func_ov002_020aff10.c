// @symbol func_ov002_020aff10
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void* v);
extern void func_ov002_020af248(char* c, int n);
extern void func_ov002_020af218(char* c, int n);
void func_ov002_020aff10(char* c){
  func_ov002_020aefb8(c);
  switch(*(int*)(c+0x388)){
  case 0:
    if(*(unsigned short*)(c+0x100) == 0) _ZN5Sound9PlayBank3EjRK7Vector3(0x68, c+0x74);
    func_ov002_020af474(c);
    if(*(unsigned short*)(c+0x100) != 0x25) break;
    *(int*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFF) &= ~1;
    *(int*)(c+0x388) = 1;
    *(int*)(c+0x98) = 0x2000;
    break;
  case 1:
    if(*(unsigned short*)(c+0x100) > 0x12c) *(int*)(c+0x388) = 2;
    func_ov002_020af3a8(c);
    break;
  case 2:
    func_ov002_020af248(c, 0x1e);
    func_ov002_020af3a8(c);
    break;
  }
  func_ov002_020af218(c, 0xbb8);
  func_ov002_020aeee4(c);
}
