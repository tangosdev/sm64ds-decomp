//cpp
// @symbol _ZN13MotherPenguin8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "MotherPenguin.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN5dCc_c5ClearEv(void*);
extern void _ZN5dCc_c6UpdateEv(void*);
extern int func_ov018_02111d28(void*);
extern void func_ov018_0211235c(void*);
}
struct Sub { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); };

int MotherPenguin::Behavior()
{
  func_ov018_0211235c((char*)this);
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
  _ZN9Animation7AdvanceEv((char*)&mTextureSequence);
  _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
  _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
  ((Sub*)((char*)&mModelAnim))->m3();
  func_ov018_02111d28(((char*)this));
  return 1;
}
