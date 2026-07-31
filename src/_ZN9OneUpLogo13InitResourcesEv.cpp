//cpp
// @symbol _ZN9OneUpLogo13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "OneUpLogo.h"
extern "C" {
struct SharedFilePtr { int a, file; };
extern SharedFilePtr data_ov002_02110a9c;
extern SharedFilePtr data_ov002_02110aa4;
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int c, unsigned int n);
}

int OneUpLogo::InitResources()
{
  unsigned short n;
  {
    unsigned int v = mParam;
    n = (unsigned short)(v > 8 ? 7 : (v - 1));
  }
  _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_02110a9c);
  if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_02110aa4), 1, -1) == 0)
    return 0;
  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov002_02110aa4.file, (void*)data_ov002_02110a9c.file);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(((char*)this)+0x124, (void*)data_ov002_02110a9c.file, 0x40000000, 0, n);
  unk_14e = 0;
  unk_13c = mPosX;
  unk_140 = mPosY;
  unk_144 = mPosZ;
  unk_0a8 = 0x14000;
  unk_09c = -0x2000;
  unk_0a0 = -0x32000;
  unk_14c = 0;
  unk_138 = 0;
  unk_148 = 0;
  return 1;
}
