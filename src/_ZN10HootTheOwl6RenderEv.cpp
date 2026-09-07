//cpp
// @symbol _ZN10HootTheOwl6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "HootTheOwl.h"
extern "C" {
extern char data_ov094_02136b40[];
}
int HootTheOwl::Render()
{
  if (mCurrentState == (State *)data_ov094_02136b40) return 1;
  mModelAnim.Render(0);
  return 1;
}
