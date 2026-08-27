//cpp
// @symbol _ZN10BrickBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BrickBlock.h"
#include "SharedFilePtr.h"
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210da18[];

int BrickBlock::CleanupResources()
{
  int v = actorID;
  switch(v){
  case 0x141: ((SharedFilePtr *)(data_ov002_0210d9d8))->Release(); break;
  case 0x142: ((SharedFilePtr *)(data_ov002_0210da30))->Release(); break;
  case 0x143: ((SharedFilePtr *)(data_ov002_0210da18))->Release(); break;
  case 0x144: UnloadSilverStarAndNumber(); break;
  }
  return 1;
}
