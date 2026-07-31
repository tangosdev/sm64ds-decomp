//cpp
// @symbol func_ov075_0211a26c
// @emits dScEntry_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScEntry_c::Render - recovered from vtable slot identity */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x64]; PMF pp[1]; };
extern "C" {
int dScEntry_c_Render(C *c){
  if(*(int*)&c->pp[0]!=0){
    PMF *p = c->pp;
    (c->**p)();
  }
  func_0203083c();
  return 1;
}
}
