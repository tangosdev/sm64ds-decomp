//cpp
// @symbol _ZN15daObjMarioCap_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjMarioCap_c.h"
#include "SharedFilePtr.h"
extern "C" {
}
extern void* data_ov002_0210de50;
extern void* data_ov002_0210de60;
extern void* data_ov002_0210de48;
extern void* data_ov002_0210de28;
extern void* data_ov002_0210de08;
extern void* data_ov002_0210de20;
extern void* data_ov002_0210de40;
extern void* data_ov002_0210de10;
extern void* data_ov002_0210de00;
extern void* data_ov002_0210de58;
extern void* data_ov002_0210de18;
extern void* data_ov002_0210de30;
extern void* data_ov002_0210de38;

int daObjMarioCap_c::CleanupResources()
{
  int i = mModelIndex;
  if (i >= 3) return 1;
  ((SharedFilePtr *)(data_ov002_020ff0ac[i]))->Release();
  switch (mType) {
  case 0xf:
    ((SharedFilePtr *)(&data_ov002_0210de50))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de60))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de48))->Release();
    break;
  case 0x14:
  case 0x15:
  case 0x16:
    ((SharedFilePtr *)(&data_ov002_0210de28))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de08))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de20))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de40))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de10))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de00))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de58))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de18))->Release();
    break;
  default:
    ((SharedFilePtr *)(&data_ov002_0210de30))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de38))->Release();
    break;
  }
  return 1;
}
