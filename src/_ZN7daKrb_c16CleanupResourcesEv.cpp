//cpp
// @symbol _ZN7daKrb_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKrb_c.h"
#include "SharedFilePtr.h"
extern "C" {
extern void UnloadBlueCoinModel(void* p);
extern void _ZN11dCapEnemy_c14UnloadCapModelEv(char* c);
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
}
extern char data_ov084_02130cf8[];
extern void* data_ov084_02130278[];

int daKrb_c::CleanupResources()
{
  int i;
  if (mGoombaType == 2)
    UnloadBlueCoinModel(((char*)this));
  ((SharedFilePtr *)(data_ov084_02130cf8))->Release();
  for (i = 0; i < 7; ++i)
    ((SharedFilePtr *)(data_ov084_02130278[i]))->Release();
  if ((unsigned char)(mRewardType + 0xff) <= 1)
    UnloadSilverStarAndNumber();
  _ZN11dCapEnemy_c14UnloadCapModelEv(((char*)this));
  if (mGoombaType == 3) {
    unsigned int id = mTargetUniqueID;
    if (id != 0) {
      char* a = _ZN8dActor_c10FindWithIDEj(id);
      if (a != 0) {
        unsigned char *p = (unsigned char*)(((int)a + 0x602));
        *p = *p - 1;
      }
    }
  }
  return 1;
}
