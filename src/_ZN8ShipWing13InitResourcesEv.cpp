//cpp
// @symbol _ZN8ShipWing13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipWing.h"
typedef short s16;
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
}

int ShipWing::InitResources()
{
  void* mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_02114070);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, mdl, 1, -1);
  unk_118 = unk_008 & 1;
  unk_119 = (unk_008 >> 8) & 1;
  if (unk_118 != 0) {
    *(s16 *)(((int)((char*)this) + 0x8c)) += 0x2400;
    *(s16 *)(((int)((char*)this) + 0x8e)) += 0x8000;
    *(s16 *)(((int)((char*)this) + 0x90)) += 0x8000;
  }
  unk_110 = unk_08c;
  unk_112 = unk_08e;
  unk_114 = unk_090;
  unk_116 = 0;
  return 1;
}
