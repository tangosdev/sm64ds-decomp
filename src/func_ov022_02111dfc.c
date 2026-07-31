// @symbol func_ov022_02111dfc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjFl_Seesaw_c.h"
// @emits daObjFl_Seesaw_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjFl_Seesaw_c::Behavior - recovered from vtable slot identity */
typedef int Fix12;
typedef short s16;
extern void func_020393a4(int* p, int v);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12 a, Fix12 b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);

int daObjFl_Seesaw_c_Behavior(char* c) {
    struct daObjFl_Seesaw_c *self = (struct daObjFl_Seesaw_c *)(void *)c;
  func_020393a4((int*)(c+0x124), 0x650000);
  if (DecIfAbove0_Byte((unsigned char*)(c+0x320)) == 0) {
    int r1;
    s16* a = (s16*)(((int)c + 0x8c));
    *a += self->unk_31e;
    r1 = self->unk_08c;
    if (r1 >= 0x400 || r1 <= -0x400) {
      self->unk_31e = -self->unk_31e;
      self->unk_320 = 0x1e;
    }
  }
  func_ov022_02111d48(c);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0) {
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  }
  return 1;
}
