//cpp
// @symbol _ZN9ArrowLift13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ArrowLift.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* bmd, int a, int b);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int fix, int t, unsigned int u, unsigned int v);
}

int ArrowLift::InitResources()
{
  void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_02114270);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, m, 1, -1);
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this)+0x124, ((char*)this), 0x32000, 0x64000, 0x800002, 0);
  unk_158 = 0;
  unk_15c = mParam & 1;
  unk_15d = 0;
  unk_08e = 0;
  return 1;
}
