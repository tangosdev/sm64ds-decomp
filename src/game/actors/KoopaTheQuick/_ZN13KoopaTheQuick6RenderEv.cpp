//cpp
// @symbol _ZN13KoopaTheQuick6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "KoopaTheQuick.h"
/* Model is the real class now, through KoopaTheQuick.h: HideMaterial is its own
   non-virtual and the slot-5 virtual is Render, which ModelAnim overrides. */

int KoopaTheQuick::Render()
{
  mModelAnim.HideMaterial(0, 1);
  mModelAnim.Render((const Vector3*)((char*)&mScaleX));
  return 1;
}
