//cpp
// @symbol func_ov075_0211a26c
// recovered name: dScEntry_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScEntry_c::Render - byte-matched vs the ROM at mwccarm 2004/b56, strict relocs
   (tools/match.py, size from ov075 symbols.txt). The old vtable-slot-guess
   marker was wrong: the body reproduces the ROM's bytes. VS wiring lane. */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x64]; PMF pp[1]; };
extern "C" {
int func_ov075_0211a26c(C *c){
  if(*(int*)&c->pp[0]!=0){
    PMF *p = c->pp;
    (c->**p)();
  }
  func_0203083c();
  return 1;
}
}
