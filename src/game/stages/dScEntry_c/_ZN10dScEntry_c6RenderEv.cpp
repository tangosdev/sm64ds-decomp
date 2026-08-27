//cpp
// @symbol _ZN10dScEntry_c6RenderEv
/* dScEntry_c::Render() -- vtable slot 9. Real C++ method over the shared
 * header; local `C` models the pmf field this function touches at +0x64. */
#include "decl_common.h"
#include "dScEntry_c.h"
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x64]; PMF pp[1]; };
s32 dScEntry_c::Render(){
  C *c = (C*)this;
  if(*(int*)&c->pp[0]!=0){
    PMF *p = c->pp;
    (c->**p)();
  }
  func_0203083c();
  return 1;
}
