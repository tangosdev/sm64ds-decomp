//cpp
// @symbol _ZN9LakituBro13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LakituBro.h"
typedef unsigned char u8;
typedef unsigned int u32;
typedef int Fix12;

struct SharedFilePtr { int f0; void* f4; };
struct BMD_File; struct BTP_File; struct BCA_File;
struct ModelBase {};
struct ModelAnim {};
struct ShadowModel {};
struct TextureSequence {};

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase* thiz, BMD_File* f, int a, int b);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern "C" void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
extern "C" void _ZN11ShadowModel12InitCylinderEv(ShadowModel* thiz);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File& a, BTP_File& b);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim* thiz, BCA_File* f, int a, Fix12 b, u32 c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence* thiz, BTP_File& f, int a, Fix12 b, u32 c);

extern SharedFilePtr data_ov085_0213074c;
extern SharedFilePtr data_ov085_02130744;
extern SharedFilePtr data_ov085_0213073c;
extern int data_ov085_021307d0;
extern int data_ov085_021307e0;
extern char data_0209caa0[];

int LakituBro::InitResources()
{
  BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov085_0213074c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)((char*)&mModelAnim1), bmd, 1, -1);
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov085_02130744);
  _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov085_0213073c);
  _ZN11ShadowModel12InitCylinderEv((ShadowModel*)((char*)&mShadowModel1));
  _ZN11ShadowModel12InitCylinderEv((ShadowModel*)((char*)&mShadowModel2));
  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
      *(BMD_File*)data_ov085_0213074c.f4, *(BTP_File*)data_ov085_0213073c.f4);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
      (ModelAnim*)((char*)&mModelAnim1), (BCA_File*)data_ov085_02130744.f4, 0, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      (TextureSequence*)((char*)&mTextureSequence), *(BTP_File*)data_ov085_0213073c.f4, 0, 0x1000, 0);
  unk_16c = 0x1000;
  unk_2d0 = unk_008 & 0xff;
  if (unk_2d0 == 0xff)
    unk_2d0 = 0;
  switch (unk_2d0) {
  case 0:
    func_ov085_0212e728(((char*)this), &data_ov085_021307d0);
    break;
  case 1:
    {
      int v = *(int*)(data_0209caa0 + 8);
      if (v & 0x20000)
        return 0;
      if (v & 0x10000)
        *(int*)(data_0209caa0 + 8) = v & ~0x10000;
      unk_2dc = 1;
      if (!(*(int*)(data_0209caa0 + 8) & 0x80))
        func_ov085_0212e728(((char*)this), &data_ov085_02130790);
      else
        func_ov085_0212e728(((char*)this), &data_ov085_021307e0);
    }
    break;
  }
  return 1;
}
