//cpp
// @symbol _ZN6Lakitu8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Lakitu.h"
extern "C" {
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void*, int);
extern int func_ov077_02123d40(void*);
}

int Lakitu::Behavior()
{
  int v = mParam;
  if(_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(((char *)this), v ? 0x1068000 : 0x7d0000)) return 1;
  func_ov077_02124718(((char *)this));
  func_ov077_02123d40(((char *)this));
  return 1;
}
