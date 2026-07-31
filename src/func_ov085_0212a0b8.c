// @symbol func_ov085_0212a0b8
// @emits Toad_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daKinopio_c::Kill - recovered from vtable slot identity */
int Toad_Kill(char* c){
  Actor__UpdatePos(c, c+0x160);
  func_ov085_02129ebc(c, c+0x194);
  func_ov085_02129f8c(c);
  return 1;
}
