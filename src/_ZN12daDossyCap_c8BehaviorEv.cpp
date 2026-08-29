//cpp
// @symbol _ZN12daDossyCap_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daDossyCap_c.h"
extern "C" int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* pos, void* rot, int e, int f);
extern "C" void func_ov001_020ab228(char* c, char* a1, int idx, int a3, unsigned char a5);
extern "C" void _ZN5dCc_c5ClearEv(void* self);
extern "C" void _ZN5dCc_c6UpdateEv(void* self);

int daDossyCap_c::Behavior()
{
  char* p = *(char**)((char*)&mCarrier);
  if(p == 0) return 1;
  int b = (mFlags & 0x20000) != 0;
  if(b != 0){
    char* s = (char*)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, 0x1210, ((char*)this)+0x5c, ((char*)this)+0x8c, mAreaId, -1);
    if(s != 0){
      *(int*)(s+0xd0) = mEatingPlayer;
      *(char**)(*(char**)((char*)&mEatingPlayer)+0x360) = s;
      int* qs = (int*)((int)s + 0xb0);
      *qs = *qs | 0x20000;
      int* qt = (int*)((int)((char*)this) + 0xb0);
      *qt = *qt & ~0x20000;
      mEatingPlayer = 0;
      mCapIcon.Unlink();
      func_ov001_020ab228(((char*)this)+0xd4, ((char*)this), 2, 0, 0);
    }
    return 1;
  }
  p = (char*)((int)p + 0xd8);
  mPosX = *(int*)p;
  mPosY = *(int*)(p+4);
  mPosZ = *(int*)(p+8);
  mAngleX = *(short*)(*(char**)((char*)&mCarrier)+0xe4);
  mAngleY = *(short*)(*(char**)((char*)&mCarrier)+0x8e);
  if(((unsigned int)mCapIcon.mFlags << 30) >> 31){
    func_ov065_02118c4c(((char*)this));
    if(func_ov065_021180d4(((char*)this)) != 0){
      _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
      return 1;
    }
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
  }
  return 1;
}
