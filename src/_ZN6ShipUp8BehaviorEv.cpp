//cpp
// @symbol _ZN6ShipUp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipUp.h"
extern int _ZN16MeshColliderBase6EnableEP5Actor(void* m, void* a);
extern void func_020393a4(int* p, int v);
extern int _ZN5Actor13DistToCPlayerEv(void* a);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void* a);
extern short data_02082214[];

int ShipUp::Behavior()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider) == 0){
    _ZN16MeshColliderBase6EnableEP5Actor(((char*)this)+0x124, ((char*)this));
  }
  func_020393a4((int*)((char*)&mMeshCollider), 0x2000000);
  if(mModelIndex == 0){
    *(short*)(((int)((char*)this) + 0x320)) += 0xda;
    unk_08c = (short)((*(short*)((char*)data_02082214 + ((unk_320>>4)<<2)) << 0xa) >> 0xc);
    if(_ZN5Actor13DistToCPlayerEv(((char*)this)) < 0xbb8000){
      unk_324 = _ZN5Sound8PlayLongEjjjRK7Vector3j(unk_324, 3, 0x8b, ((char*)this)+0x74, 0);
    }
    func_ov016_021126a8(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  }
  return 1;
}
