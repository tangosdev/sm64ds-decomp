//cpp
// @symbol _ZN5Cloud13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Cloud.h"
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void func_ov039_02111214(char *t);
extern "C" int data_ov039_021118e0;

int Cloud::InitResources()
{
  void *f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov039_021118e4);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, f, 1, 2);
  func_ov039_02111214((char *)this);
  data_ov039_021118e0++;
  return 1;
}
