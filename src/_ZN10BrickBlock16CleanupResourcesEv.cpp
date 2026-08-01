//cpp
// @symbol _ZN10BrickBlock16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BrickBlock.h"
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210da18[];

int BrickBlock::CleanupResources()
{
  int v = unk_00c;
  switch(v){
  case 0x141: _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8); break;
  case 0x142: _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da30); break;
  case 0x143: _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da18); break;
  case 0x144: UnloadSilverStarAndNumber(); break;
  }
  return 1;
}
