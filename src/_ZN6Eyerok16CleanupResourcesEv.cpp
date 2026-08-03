//cpp
// @symbol _ZN6Eyerok16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Eyerok.h"
#include "SharedFilePtr.h"
extern char data_ov066_0211ae6c[];
extern char data_ov066_0211ae4c[];
extern char data_ov066_0211aeb4[];
extern char data_ov066_0211aebc[];
extern char data_ov066_0211ae9c[];
extern char data_ov066_0211ae3c[];
extern char data_ov066_0211ae2c[];
extern char data_ov066_0211ae5c[];
extern char data_ov066_0211ae84[];
extern char data_ov066_0211aea4[];
extern char data_ov066_0211ae8c[];
extern char data_ov066_0211ae54[];
extern char data_ov066_0211ae94[];
extern char data_ov066_0211ae64[];
extern char data_ov066_0211ae44[];
extern char data_ov066_0211ae74[];
extern char data_ov066_0211ae7c[];
extern char data_ov066_0211ae24[];
extern char data_ov066_0211aeac[];
extern char data_ov066_0211ae14[];
extern char data_ov066_0211ae1c[];
extern char data_ov066_0211ae34[];

int Eyerok::CleanupResources()
{
  if(_ZN16MeshColliderBase9IsEnabledEv((char *)&unk_674))
    _ZN16MeshColliderBase7DisableEv((char *)&unk_674);
  if(unk_49c==0){
    ((SharedFilePtr *)(data_ov066_0211ae6c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae4c))->Release();
    ((SharedFilePtr *)(data_ov066_0211aeb4))->Release();
    ((SharedFilePtr *)(data_ov066_0211aebc))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae9c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae3c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae2c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae5c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae84))->Release();
    ((SharedFilePtr *)(data_ov066_0211aea4))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae8c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae54))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae94))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae64))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae44))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae74))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae7c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae24))->Release();
    ((SharedFilePtr *)(data_ov066_0211aeac))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae14))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae1c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae34))->Release();
  }
  return 1;
}
