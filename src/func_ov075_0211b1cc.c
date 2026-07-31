// @symbol func_ov075_0211b1cc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScEntry_c.h"
// @emits dScEntry_c_OnYoshiTryEat_0211b1cc
/* recovered: renamed to Class_Method */
/* dScEntry_c::OnYoshiTryEat - recovered from vtable slot identity */
extern unsigned short DecIfAbove0_Short(unsigned short* p);
struct E { char pad[8]; int r8; int rc; char pad2[8]; };
void dScEntry_c_OnYoshiTryEat_0211b1cc(char* c) {
    struct dScEntry_c *self = (struct dScEntry_c *)(void *)c;
  int i;
  if (DecIfAbove0_Short((unsigned short*)(c + 0xa2)) == 0) return;
  for (i = 0; i < self->unk_0a4; i++) {
    struct E* e = (struct E*)(self->unk_080 + i * 0x18);
    e->r8 = func_ov075_0211addc(c, e->rc);
  }
  func_ov075_0211abb0(c);
  func_ov075_0211ad60(c);
  self->unk_0a0 += 0x1111;
}
