//cpp
// @symbol _ZN14daObjKi_Fune_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjKi_Fune_c.h"
#include "dBgW.h"
extern "C" {
extern void func_020393a4(int* p, int v);
extern int _ZN8dActor_c13DistToCPlayerEv(void* a);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* a);
}
extern short data_02082214[];

int daObjKi_Fune_c::Behavior()
{
  if(((dBgW *)((char*)&mMeshCollider))->IsEnabled() == 0){
    ((dBgW *)(((char*)this)+0x124))->Enable((dActor_c *)(((char*)this)));
  }
  func_020393a4((int*)((char*)&mMeshCollider), 0x2000000);
  if(mModelIndex == 0){
    *(short*)(((int)((char*)this) + 0x320)) += 0xda;
    mAngleX = (short)((*(short*)((char*)data_02082214 + ((mBobAngle>>4)<<2)) << 0xa) >> 0xc);
    if(_ZN8dActor_c13DistToCPlayerEv(((char*)this)) < 0xbb8000){
      mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundHandle, 3, 0x8b, ((char*)this)+0x74, 0);
    }
    func_ov016_021126a8(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  }
  return 1;
}
