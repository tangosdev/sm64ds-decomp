//cpp
// @symbol _ZN16BowserShockwaves8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserShockwaves.h"
extern "C" void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern "C" int Vec3_HorzDist(const void* a, const void* b);
extern "C" void _ZN6Player5ShockEj(void* p, unsigned int x);
extern "C" void _ZN9Animation7AdvanceEv(void* a);
extern "C" void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern "C" int _ZN9Animation8FinishedEv(void* a);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void* a);

int BowserShockwaves::Behavior()
{
  char* pl;
  unsigned int n;
  int d;
  int s;
  int m0, m1, m2, m3;
  ((*(unsigned short *)(((int)((char*)this) + 0x214))))++;
  pl = (char*)_ZN8dActor_c13ClosestPlayerEv(((char*)this));
  n = *(unsigned short*)(((char*)this)+0x200+0x14);
  s = n * 0x22;
  if(pl != 0 && n < 0x46 && *(unsigned char*)(pl+0x6de) == 0){
    d = Vec3_HorzDist(((char*)this)+0x5c, pl+0x5c);
    m0 = s*data_ov060_0211ab20[0];
    m1 = s*data_ov060_0211ab20[1];
    m2 = s*data_ov060_0211ab20[2];
    m3 = s*data_ov060_0211ab20[3];
    if((m0 < d && d < m1) || (m2 < d && d < m3)){
      _ZN6Player5ShockEj(pl, 1);
    }
  }
  _ZN9Animation7AdvanceEv((char*)&mTextureSequence1);
  _ZN9Animation7AdvanceEv((char*)&mTextureSequence2);
  _ZN9Animation7AdvanceEv((char*)&mMaterialChanger1);
  _ZN9Animation7AdvanceEv((char*)&mMaterialChanger2);
  _ZN9Animation7AdvanceEv((char*)&mTextureTransformer1);
  _ZN9Animation7AdvanceEv((char*)&mTextureTransformer2);
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim1);
  _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim2);
  Matrix4x3_FromTranslation(((char*)this)+0xf0, mPosX>>3, mPosY>>3, mPosZ>>3);
  Matrix4x3_FromTranslation(((char*)this)+0x190, mPosX>>3, mPosY>>3, mPosZ>>3);
  if(_ZN9Animation8FinishedEv((char*)(Animation *)&mModelAnim1)){
    _ZN7fBase_c18MarkForDestructionEv(((char*)this));
  }
  return 1;
}
