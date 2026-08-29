// @symbol func_ov075_0211b1cc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
// recovered name: dScEntry_c_OnYoshiTryEat_0211b1cc
/* recovered: renamed to Class_Method */
/* The old heuristic label associated this function with dScEntry_c, but the
   typed class layout now proves 0x080/0x0a0/0x0a4 lie inside its icon_c array.
   Keep this function's self type local until its actual owner is established;
   these offsets are not evidence for fields on dScEntry_c. */
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
