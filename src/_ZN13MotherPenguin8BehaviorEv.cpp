//cpp
// @symbol _ZN7SkiLift8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_RacingPenguin.h"
/* recovered: named members + shared header, real C++ method */
#include "SkiLift.h"
extern "C" {
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern int func_ov018_02111d28(void*);
}
struct Sub { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); };

int SkiLift::Behavior()
{
  _ZN13RacingPenguin16OnPendingDestroyEv();
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
  _ZN9Animation7AdvanceEv((char*)&mTextureSequence);
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
  ((Sub*)((char*)&mModelAnim))->m3();
  func_ov018_02111d28(((char*)this));
  return 1;
}
