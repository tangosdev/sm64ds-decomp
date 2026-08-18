//cpp
// @symbol _ZN10dScEntry_c6RenderEv
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* dScEntry_c::Render() -- vtable slot 9. extern "C" carries the literal
 * mangled name unmangled; local `C` models the pmf field this function
 * touches rather than including the shared header -- see
 * include/dScEntry_c.h. */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x64]; PMF pp[1]; };
extern "C" {
int _ZN10dScEntry_c6RenderEv(C *c){
  if(*(int*)&c->pp[0]!=0){
    PMF *p = c->pp;
    (c->**p)();
  }
  func_0203083c();
  return 1;
}
}
