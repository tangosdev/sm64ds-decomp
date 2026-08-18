// @symbol func_ov075_0211b1cc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
// recovered name: dScEntry_c_OnYoshiTryEat_0211b1cc
/* recovered: renamed to Class_Method */
/* dScEntry_c's own header now derives from dScene_c -> dBase_c -> fBase_c,
   and those headers have no C spelling (see the note in fBase_c.h), so this
   plain-C file uses a local offset struct instead of including
   dScEntry_c.h -- matching every other dScEntry_c method in this family
   (see include/dScEntry_c.h's derivation note: only InitResourcesEv.cpp,
   which is //cpp, includes the shared header directly). unk_080/unk_0a0/
   unk_0a4 are dScEntry_c's own fields at those absolute offsets. */
extern unsigned short DecIfAbove0_Short(unsigned short* p);
struct E { char pad[8]; int r8; int rc; char pad2[8]; };
struct Self {
    char pad[0x80];
    int unk_080;                  /* 0x080 */
    char pad2[0xa0 - 0x84];
    short unk_0a0;                /* 0x0a0 */
    char pad3[0xa4 - 0xa2];
    unsigned short unk_0a4;       /* 0x0a4 */
};
void func_ov075_0211b1cc(char* c) {
    struct Self *self = (struct Self *)(void *)c;
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
