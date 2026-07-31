//cpp
// @symbol _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SaveData.h"
struct MinigameSaveData;
extern "C" {
}

void SaveData::SetDefaultValuesMg(MinigameSaveData * mg_)
{
    void* mg = (void*)mg_;

  func_0205a588(((void*)this), 0, 0x2e4);
  *(int*)((void*)this) = 0x30303035;
}
