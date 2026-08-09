//cpp
// @symbol _ZN25SlideDecorationSilverStar13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SlideDecorationSilverStar.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov031_02111214(char*);
extern int* data_ov031_02111424[];
}

int SlideDecorationSilverStar::InitResources()
{
  unsigned short type = unk_00c;
  switch(type){
    case 0x12e: mVariant=0; break;
    case 0x12f: mVariant=1; break;
    case 0x130: mVariant=2; break;
    case 0x131: mVariant=3; break;
  }
  unsigned char idx = mVariant;
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov031_02111424[idx]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this)+0xd4, f, 1, -1);
  func_ov031_02111214(((char *)this));
  return 1;
}
